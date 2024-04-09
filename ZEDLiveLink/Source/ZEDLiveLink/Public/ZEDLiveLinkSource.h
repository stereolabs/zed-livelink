#pragma once

#include "CoreMinimal.h"
#include <ILiveLinkSource.h>
#include "HAL/Runnable.h"
#include "HAL/ThreadSafeBool.h"
#include "IMessageContext.h"
#include "Roles/LiveLinkAnimationTypes.h"
#include "ZEDLiveLinkSettings.h"
#include "FrameData.h"


class FRunnableThread;
class FSocket;
class ILiveLinkClient;
class ISocketSubsystem;


class ZEDLIVELINK_API FZEDLiveLinkSource : public ILiveLinkSource, public FRunnable
{
public:
	FZEDLiveLinkSource(const FZEDLiveLinkSettings& InSettings);

	virtual ~FZEDLiveLinkSource();

	// Begin ILiveLinkSource Interface
	
	virtual void ReceiveClient(ILiveLinkClient* InClient, FGuid InSourceGuid) override;

	virtual bool IsSourceStillValid() const override;

	virtual bool RequestSourceShutdown() override;

	virtual FText GetSourceType() const override { return SourceType; };
	virtual FText GetSourceMachineName() const override { return SourceMachineName; }
	virtual FText GetSourceStatus() const override { return SourceStatus; }

	// End ILiveLinkSource Interface

	// Begin FRunnable Interface

	virtual bool Init() override { return true; }
	virtual uint32 Run() override;
	void Start();
	virtual void Stop() override;
	virtual void Exit() override { }

	// End FRunnable Interface

	void ProcessReceivedData(TSharedPtr<TArray<uint8>> ReceivedData);

private:

	ILiveLinkClient* Client;

	// Our identifier in LiveLink
	FGuid SourceGuid;

	FMessageAddress ConnectionAddress;

	FText SourceType;
	FText SourceMachineName;
	FText SourceStatus;

	FZEDLiveLinkSettings ZEDSettings;
	// Socket to receive data on
	FSocket* Socket;

	// Subsystem associated to Socket
	ISocketSubsystem* SocketSubsystem;

	// Threadsafe Bool for terminating the main thread loop
	FThreadSafeBool Stopping;

	// Thread to run socket operations on
	FRunnableThread* Thread;

	// Name of the sockets thread
	FString ThreadName;

	// Time to wait between attempted receives
	FTimespan WaitTime;

	// List of subjects we've already encountered
	TArray<FName> Subjects;

	// Buffer to receive socket data into
	TArray<uint8> RecvBuffer;


	bool FirstConnection = true;
	// Check if static data is setup

	// current timestamp
	double CurrentTimeStamp = 0;

	void UpdateAnimationStaticData(FName SubjectName, TArray<int>ParentsIdx, TArray<FName> TargetBones);

	void UpdateCameraStaticData(FName SubjectName, FTransform CameraPose);

	void ClearSubjects();
};
