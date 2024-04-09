#include "ZEDLiveLinkSource.h"

#include <string>

#include "ILiveLinkClient.h"
#include "LiveLinkTypes.h"
#include "Roles/LiveLinkCameraRole.h"
#include "Roles/LiveLinkCameraTypes.h"
#include "Roles/LiveLinkAnimationRole.h"
#include "Roles/LiveLinkAnimationTypes.h"
#include "Roles/LiveLinkTransformRole.h"
#include "Roles/LiveLinkTransformTypes.h"

#include "Async/Async.h"
#include "Common/UdpSocketBuilder.h"
#include "HAL/RunnableThread.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Containers/UnrealString.h"
#include "Misc/Char.h"
#include "Containers/Array.h"

#define LOCTEXT_NAMESPACE "ZEDLiveLinkSource"

#define RECV_BUFFER_SIZE 0

FZEDLiveLinkSource::FZEDLiveLinkSource(const FZEDLiveLinkSettings& InSettings)
: Socket(nullptr)
, Stopping(false)
, Thread(nullptr)
, WaitTime(FTimespan::FromMilliseconds(500))
{
	// defaults
	ZEDSettings = InSettings;

	SourceStatus = LOCTEXT("SourceStatus_NoConnection", "No Connection");
	SourceType = LOCTEXT("ZEDLiveLinkSourceType", "ZED LiveLink");
	SourceMachineName = FText::FromString(ZEDSettings.Endpoint.ToString());

	//setup socket
	if (ZEDSettings.Endpoint.Address.IsMulticastAddress())
	{
		Socket = FUdpSocketBuilder(TEXT("ZEDSOCKET"))
			.AsNonBlocking()
			.AsReusable()
			.BoundToPort(ZEDSettings.Endpoint.Port)
			.WithReceiveBufferSize(RECV_BUFFER_SIZE)

			.BoundToAddress(FIPv4Address::Any)
			.JoinedToGroup(ZEDSettings.Endpoint.Address)
			.WithMulticastLoopback()
			.WithMulticastTtl(2);
					
	}
	else
	{
		Socket = FUdpSocketBuilder(TEXT("ZEDSOCKET"))
			.AsNonBlocking()
			.AsReusable()
			.BoundToAddress(ZEDSettings.Endpoint.Address)
			.BoundToPort(ZEDSettings.Endpoint.Port)
			.WithReceiveBufferSize(RECV_BUFFER_SIZE);

			//UE_LOG(LogTemp, Warning, TEXT("Connect to %s : %d"), *ZEDSettings.Endpoint.Address.ToString(), ZEDSettings.Endpoint.Port);
	}

	RecvBuffer.SetNumUninitialized(RECV_BUFFER_SIZE);

	if ((Socket != nullptr) && (Socket->GetSocketType() == SOCKTYPE_Datagram))
	{
		SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);

		Start();

		//SourceStatus = LOCTEXT("SourceStatus_Connecting", "Connecting");
	}
}

FZEDLiveLinkSource::~FZEDLiveLinkSource()
{
	Stop();
	if (Thread != nullptr)
	{
		Thread->WaitForCompletion();
		delete Thread;
		Thread = nullptr;
	}
	if (Socket != nullptr)
	{
		Socket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);
	}
}

void FZEDLiveLinkSource::ReceiveClient(ILiveLinkClient* InClient, FGuid InSourceGuid)
{
	Client = InClient;
	SourceGuid = InSourceGuid;
}

bool FZEDLiveLinkSource::IsSourceStillValid() const
{
	// Source is valid if we have a valid thread and socket
	bool bIsSourceValid = !Stopping && Thread != nullptr && Socket != nullptr;
	return bIsSourceValid;
}

bool FZEDLiveLinkSource::RequestSourceShutdown()
{
	Stop();

	SourceStatus = LOCTEXT("SourceStatus_Disconnected", "Disconnected");

	return true;
}
// FRunnable interface

void FZEDLiveLinkSource::Start()
{
	ThreadName = "ZED UDP Receiver ";
	ThreadName.AppendInt(FAsyncThreadIndex::GetNext());
	
	Thread = FRunnableThread::Create(this, *ThreadName, 0, TPri_AboveNormal);
}

void FZEDLiveLinkSource::Stop()
{
	Stopping = true;
}

uint32 FZEDLiveLinkSource::Run()
{
	TSharedRef<FInternetAddr> Sender = SocketSubsystem->CreateInternetAddr();
	while (!Stopping)
	{
		if (Socket->Wait(ESocketWaitConditions::WaitForRead, WaitTime))
		{
			FirstConnection = false;
			SourceStatus = LOCTEXT("SourceStatus_Connected", "Connected");
			uint32 Size;
			while (Socket->HasPendingData(Size))
			{
				int32 Read = 0;
				RecvBuffer.SetNum(Size);
				bool recv = Socket->RecvFrom(RecvBuffer.GetData(), RecvBuffer.Num(), Read, *Sender);

				if (recv)
				{
					if (Read > 0)
					{
						TSharedPtr<TArray<uint8>> ReceivedData = MakeShareable(new TArray<uint8>());
						ReceivedData->SetNumUninitialized(Read);
						memcpy(ReceivedData->GetData(), RecvBuffer.GetData(), Read);
						AsyncTask(ENamedThreads::GameThread, [this, ReceivedData]() { ProcessReceivedData(ReceivedData); });
					}
				}
			}
		}
		else
		{
			if (!FirstConnection)
			{
				SourceStatus = LOCTEXT("SourceStatus_NoData", "No Data");
				ClearSubjects(); // clear subjects if no msg received for WaitTime (1sec).
			}
		}
	}
	return 0;
}

void FZEDLiveLinkSource::ProcessReceivedData(TSharedPtr<TArray<uint8>> ReceivedData)
{
	/// CONVERTNG TO STRING
	FString recvedString;
	int32 Read = ReceivedData->Num();
	recvedString.Empty(ReceivedData->Num());
	for (uint8& Byte : *ReceivedData.Get())
	{
		recvedString += TCHAR(Byte);
	}

	FrameData frameData = FrameData(recvedString);
	FName SubjectName = FName(*frameData.Subject);

	FLiveLinkFrameDataStruct FrameData;

	FLiveLinkSubjectKey Key = FLiveLinkSubjectKey(SourceGuid, SubjectName);
	if (Client) 
	{
		if (frameData.bIsValid) 
		{
			if (CurrentTimeStamp - frameData.Timestamp > 0) // clean all subjects when current ts > new ts. It happens if a SVO loops for example.
			{
				ClearSubjects();
			}
			else if (frameData.SubjectRole == ULiveLinkAnimationRole::StaticClass() && frameData.BodyTrackingState != EZEDTrackingState::Ok)
			{
				Client->RemoveSubject_AnyThread(Key);
				Subjects.Remove(SubjectName);
			}
			else
			{
				if (!Subjects.Contains(SubjectName) && !Client->GetSubjects(true, false).Contains(Key))
				{
					FLiveLinkSubjectPreset Preset;
					Preset.Key = Key;
					Preset.Role = frameData.SubjectRole;
					//Preset.bEnabled = true;

					if (Client->GetSources().Num() > 0)
					{
						Client->CreateSubject(Preset);
						Client->SetSubjectEnabled(Key, true);

						Subjects.Push(SubjectName);

						if (frameData.SubjectRole == ULiveLinkCameraRole::StaticClass())
						{
							UpdateCameraStaticData(SubjectName, frameData.CameraTransform);
						}
						else if (frameData.SubjectRole == ULiveLinkAnimationRole::StaticClass())
						{
							UpdateAnimationStaticData(SubjectName, frameData.ParentsIdx, frameData.TargetBones);
						}
					}
				}

				if (frameData.SubjectRole == ULiveLinkCameraRole::StaticClass())
				{
					FrameData = FLiveLinkCameraFrameData::StaticStruct();
					FLiveLinkCameraFrameData& CameraData = *FrameData.Cast<FLiveLinkCameraFrameData>();

					CameraData.WorldTime = FPlatformTime::Seconds();
					CameraData.Transform = frameData.CameraTransform;

				}
				else if (frameData.SubjectRole == ULiveLinkAnimationRole::StaticClass())
				{
					FrameData = FLiveLinkAnimationFrameData::StaticStruct();
					FLiveLinkAnimationFrameData& AnimData = *FrameData.Cast<FLiveLinkAnimationFrameData>();

					AnimData.WorldTime = FPlatformTime::Seconds();
					AnimData.Transforms = frameData.BoneTransform;
				}

				Client->PushSubjectFrameData_AnyThread(Key, MoveTemp(FrameData));
			}
			CurrentTimeStamp = frameData.Timestamp;
		}
		else
		{
			SourceStatus = LOCTEXT("SourceStatus_DataError", "Data Error");

		}
	}
}

void FZEDLiveLinkSource::UpdateCameraStaticData(FName SubjectName, FTransform CameraPose)
{
	FLiveLinkSubjectKey Key = FLiveLinkSubjectKey(SourceGuid, SubjectName);
	FLiveLinkStaticDataStruct StaticData(FLiveLinkCameraStaticData::StaticStruct());
	FLiveLinkCameraStaticData& CameraData = *StaticData.Cast<FLiveLinkCameraStaticData>();

	CameraData.bIsAspectRatioSupported = false;
	CameraData.bIsFieldOfViewSupported = false;
	CameraData.bIsFocalLengthSupported = false;
	CameraData.bIsFocusDistanceSupported = false;
	CameraData.bIsProjectionModeSupported = false;

	Client->PushSubjectStaticData_AnyThread(Key, ULiveLinkCameraRole::StaticClass(), MoveTemp(StaticData));
}

void FZEDLiveLinkSource::UpdateAnimationStaticData(FName SubjectName, TArray<int>ParentsIdx, TArray<FName> TargetBones)
{
	FLiveLinkSubjectKey Key = FLiveLinkSubjectKey(SourceGuid, SubjectName);
	FLiveLinkStaticDataStruct StaticData(FLiveLinkSkeletonStaticData::StaticStruct());
	FLiveLinkSkeletonStaticData* SkeletonData = StaticData.Cast<FLiveLinkSkeletonStaticData>();
	SkeletonData->SetBoneNames(TargetBones);
	SkeletonData->SetBoneParents(ParentsIdx);
	Client->PushSubjectStaticData_AnyThread(Key, ULiveLinkAnimationRole::StaticClass(), MoveTemp(StaticData));
}

void FZEDLiveLinkSource::ClearSubjects()
{
	while (Subjects.Num() > 0)
	{
		auto Subject = Subjects.Pop();
		FLiveLinkSubjectKey KeyToRemove = FLiveLinkSubjectKey(SourceGuid, Subject);
		Client->RemoveSubject_AnyThread(KeyToRemove);
	}
}

#undef LOCTEXT_NAMESPACE
