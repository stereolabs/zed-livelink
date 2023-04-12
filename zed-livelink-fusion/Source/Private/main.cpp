///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2022, STEREOLABS.
//
// All rights reserved.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////

#include "Roles/LiveLinkCameraRole.h"
#include "Roles/LiveLinkCameraTypes.h"
#include "LiveLinkProvider.h"
#include "RequiredProgramMainCPPInclude.h"
#include "Modules/ModuleManager.h"
#include "LiveLinkRefSkeleton.h"
#include "Roles/LiveLinkAnimationRole.h"
#include "Roles/LiveLinkAnimationTypes.h"

#include "ZEDCamera.h"
#include "ZEDFusion.h"
#include "ZEDStructs.h"
#include "SenderRunner.hpp"
#include "Utils/Util.h"

IMPLEMENT_APPLICATION(ZEDLiveLinkPlugin, "ZEDLiveLink");

using namespace sl;

static TSharedPtr<ILiveLinkProvider> LiveLinkProvider;

struct StreamedSkeletonData
{
	FName SubjectName;
	double Timestamp;
	TArray<FTransform> Skeleton;

	StreamedSkeletonData() {}
	StreamedSkeletonData(FName inSubjectName) : SubjectName(inSubjectName) {}
};

ZEDFusion FusionHandler;

void LibInit();
void UpdateSkeletonStaticData(FName SubjectName);
void UpdateAnimationFrameData(StreamedSkeletonData& StreamedSkeleton);
void PopulateSkeletonsData(SL_Bodies& objs);
FTransform BuildUETransformFromZEDTransform(SL_PoseData& pose);
StreamedSkeletonData BuildSkeletonsTransformFromZEDObjects(SL_BodyData& bodyData, double timestamp);

bool IsConnected = false;

// Streamed Data
TMap<int, StreamedSkeletonData> StreamedSkeletons;

TArray<FString> targetBone;
TArray<int> parentsIdx;

TArray<SL_CameraIdentifier> Cameras;

///////////////////////////////////////
///////////// MAIN ////////////////////
///////////////////////////////////////

int main(int argc, char **argv)
{
	ZEDFusionConfig zed_fusion_config;
	std::string zed_fusion_config_file("ZEDFusionLiveLinkConfig.json"); // Default name and location.
	if (argc == 2)
	{
		zed_fusion_config_file = argv[1];
		std::cout << "Loading " << zed_fusion_config_file << "fusion config file." << std::endl;
	}
	else if (argc > 2)
	{
		std::cout << "Unexecpected arguments, exiting..." << std::endl;
		return EXIT_FAILURE;
	}
	else {
		std::cout << "Trying to load default config file 'ZEDFusionLiveLinkConfig.json' " << std::endl;
	}
	readZEDFusionConfig(zed_fusion_config_file, zed_fusion_config);

	std::string json_config_filename = zed_fusion_config.json_config_filename;

	TArray<SL_FusionConfiguration> z_inputs;
	z_inputs.SetNum(MAX_FUSED_CAMERAS, false);
	int nb_cameras;

	FusionHandler.ReadFusionConfigFile(json_config_filename, COORD_SYS, UNIT_SYS, z_inputs.GetData(), nb_cameras);
	if (nb_cameras == 0) {
		std::cout << "Empty File " << std::endl;
		return 1;
	}

	std::vector<SenderRunner> senders(nb_cameras);
	for (int z = 0; z < nb_cameras; z++) {
		if (z_inputs[z].input.input_type != sl::INPUT_TYPE::STREAM) // not stream
		{
			std::cout << "Try to open ZED " << z_inputs[z].serial_number << ".." << std::endl;
			auto state = senders[z].open(z_inputs[z], z, zed_fusion_config);
			if (state)
				std::cout << "ZED " << z_inputs[z].serial_number << " is Running " << std::endl;
			else
				std::cout << "Fail to start ZED " << z_inputs[z].serial_number << std::endl;
		}
	}

	// start sender at the same time (better suited when playing back svos)
	for (int z = 0; z < nb_cameras; z++)
	{
		if (z_inputs[z].input.input_type != sl::INPUT_TYPE::STREAM)
		{
			senders[z].start();
		}
	}


	sl::sleep_ms(100);

	SL_InitFusionParameters init_fusion_params;
	// Do NOT change these parameters. They need to match UE's internal parameters.
	init_fusion_params.coordinate_system = sl::COORDINATE_SYSTEM::LEFT_HANDED_Z_UP;
	init_fusion_params.coordinate_units = sl::UNIT::CENTIMETER;
	init_fusion_params.timeout_period_number = 20;
	init_fusion_params.verbose = zed_fusion_config.verbose;
	init_fusion_params.output_performance_metrics = zed_fusion_config.output_performance_metrics;

	FusionHandler.Init(init_fusion_params);

	std::cout << "Starting ZEDLiveLink sample" << std::endl;

	sl::FUSION_ERROR_CODE e = sl::FUSION_ERROR_CODE::FAILURE;

	for (int z = 0; z < nb_cameras; z++) {

		auto z_input = z_inputs[z];

		SL_CameraIdentifier uuid;
		uuid.sn = z_input.serial_number;

		e = FusionHandler.Subscribe(uuid, z_input.comm_param, z_input.position, z_input.rotation);

		if (e != sl::FUSION_ERROR_CODE::SUCCESS)
		{
			std::cout << "Unable to subscribe to " << std::to_string(uuid.sn) << " . " << e << std::endl;
		}
		else
		{
			std::cout << "subscribe to " << std::to_string(uuid.sn) << " . " << e << std::endl;
			Cameras.Push(uuid);
		}
	}

	if (zed_fusion_config.sender_body_format == sl::BODY_FORMAT::BODY_34)
	{
		targetBone = targetBone34;
		parentsIdx = parents34Idx;
	}
	else if (zed_fusion_config.sender_body_format == sl::BODY_FORMAT::BODY_38)
	{
		targetBone = targetBone38;
		parentsIdx = parents38Idx;
	}
	else if (zed_fusion_config.sender_body_format == sl::BODY_FORMAT::BODY_70)
	{
		targetBone = targetBone70;
		parentsIdx = parents70Idx;
	}
	else if (zed_fusion_config.sender_body_format == sl::BODY_FORMAT::BODY_18)
	{
		std::cout << "Body 18 is not compatible. Please use Body 34/38 or 70" << std::endl;
		return EXIT_FAILURE;
	}

	LibInit();
	LiveLinkProvider = ILiveLinkProvider::CreateLiveLinkProvider(TEXT("ZED"));

	std::cout << "Waiting for connection..." << std::endl;

	SL_BodyTrackingFusionParameters BTFusionParams;
	BTFusionParams.enable_tracking = true;
	BTFusionParams.enable_body_fitting = true;
	e = FusionHandler.EnableBodyTracking(BTFusionParams);

	if (e != sl::FUSION_ERROR_CODE::SUCCESS)
	{
		std::cout << "Unable to Enable OD " << e << std::endl;
		return EXIT_FAILURE;
	}

	SL_Bodies bodies;
	SL_BodyTrackingFusionRuntimeParameters BodyTrackingRuntimeParams;
	BodyTrackingRuntimeParams.skeleton_minimum_allowed_keypoints = zed_fusion_config.skeleton_minimm_allowed_keypoints;
	BodyTrackingRuntimeParams.skeleton_smoothing = zed_fusion_config.skeleton_smoothing;
	BodyTrackingRuntimeParams.skeleton_minimum_allowed_camera = zed_fusion_config.skeleton_minimum_allowed_camera;

	SetCtrlHandler();
	int i = 0;
	//// Loop
	while (!exit_app) {
		//// Display status
		if (LiveLinkProvider->HasConnection()) 
		{
			if (!IsConnected) 
			{
				IsConnected = true;
				std::cout << "ZEDLiveLink is connected " << std::endl;
			}

			// Process the fusion
			e = FusionHandler.Process();
			if (e == sl::FUSION_ERROR_CODE::SUCCESS) {
				///// Update Streamed data
				// Retrieve fused detections

				e = FusionHandler.RetrieveBodies(bodies, BodyTrackingRuntimeParams);

				if (e != FUSION_ERROR_CODE::SUCCESS)
				{
					std::cout << "ERROR : retrieve objects : " << e << std::endl;
					return EXIT_FAILURE;
				}

				// Prepare data to be sent to Livelink
				if (bodies.is_new)
				{
					PopulateSkeletonsData(bodies);

					for (auto& it : StreamedSkeletons) {
						UpdateAnimationFrameData(it.Value);
					}
				}
			}
			sl::sleep_ms(10);
		}
		else if (IsConnected == true) {
			std::cout << "Source ZED removed" << std::endl;
			IsConnected = false;
		}
	}

	std::cout << "Closing .. " << std::endl;
	for (int z = 0; z < nb_cameras; z++)
	{
		if (z_inputs[z].input.input_type != sl::INPUT_TYPE::STREAM)
		{
			senders[z].stop();
		}
	}


	FusionHandler.Close();
	LiveLinkProvider.Reset();

	return EXIT_SUCCESS;
}

//// Initialize tool
void LibInit()
{
	GEngineLoop.PreInit(TEXT("ZEDLiveLink -Messaging"));
	ProcessNewlyLoadedUObjects();
	// Tell the module manager that it may now process newly-loaded UObjects when new C++ modules are loaded
	FModuleManager::Get().StartProcessingNewlyLoadedObjects();
	FModuleManager::Get().LoadModule(TEXT("UdpMessaging"));
}

//// Convert ZED transform to UE transform
FTransform BuildUETransformFromZEDTransform(SL_PoseData& pose)
{
	FTransform UETransform;
	SL_Vector3 zedTranslation = pose.translation;
	FVector UETranslation = FVector(zedTranslation.x, zedTranslation.y, zedTranslation.z);
	UETransform.SetTranslation(UETranslation);
	UETransform.SetRotation(FQuat(pose.rotation.x, pose.rotation.y, pose.rotation.z, pose.rotation.w));
	UETransform.SetScale3D(FVector(1, 1, 1));
	return UETransform;
}


StreamedSkeletonData BuildSkeletonsTransformFromZEDObjects(SL_BodyData& bodyData, double timestamp)
{
	StreamedSkeletonData SkeletonsData = StreamedSkeletonData(FName(FString::FromInt(bodyData.id)));
	SkeletonsData.Timestamp = timestamp;
	TMap<FString, FTransform> rigBoneTarget;
	sl::float3 bodyPosition = bodyData.keypoint[0];
	sl::float4 bodyRotation =  bodyData.global_root_orientation;

	for (int i = 0; i < targetBone.Num(); i++)
	{
		rigBoneTarget.Add(targetBone[i], FTransform::Identity);
	}
	FVector rootPosition = FVector(bodyPosition.x, bodyPosition.y, bodyPosition.z);
	FQuat global_rotation = FQuat(bodyRotation.x, bodyRotation.y, bodyRotation.z, bodyRotation.w);
	if (rootPosition.ContainsNaN())
	{
		std::cout << "rootPosition  of index : 0 is nan " << std::endl;
		rootPosition = FVector::ZeroVector;
	}
	if (global_rotation.ContainsNaN())
	{
		std::cout << "global_rotation  of index : 0 is nan " << std::endl;
		global_rotation = FQuat::Identity;
	}

	rigBoneTarget["PELVIS"].SetLocation(rootPosition);
	rigBoneTarget["PELVIS"].SetRotation(global_rotation);

	for (int i = 1; i < targetBone.Num() / 2; i++)
	{
		sl::float3 localTranslation = bodyData.local_position_per_joint[i];
		sl::float4 localRotation = bodyData.local_orientation_per_joint[i];
		sl::float3 worldTranslation = bodyData.keypoint[i];

		FVector position = FVector(worldTranslation.x, worldTranslation.y, worldTranslation.z);
		if (position.ContainsNaN())
		{
			std::cout << "worldTranslation  of index : "<< i <<"  is nan " << std::endl;
			position = FVector::ZeroVector;
		}
		FQuat rotation = FQuat(localRotation.x, localRotation.y, localRotation.z, localRotation.w);
		if (rotation.ContainsNaN())
		{
			std::cout << "rotation  of index " << i << "is nan " << std::endl;
			rotation = FQuat::Identity;
		}

		rigBoneTarget[targetBone[i]].SetLocation(position);
		rigBoneTarget[targetBone[i]].SetRotation(rotation);
	}

	TArray<FTransform> transforms;
	for (int i = 0; i < targetBone.Num() / 2; i++)
	{
		transforms.Push(rigBoneTarget[targetBone[i]]);
	}

	// Add keypoints confidence at the end of the Array of transforms.
	for (int i = 0; i < targetBone.Num() / 2; i++)
	{
		FTransform kp_conf = FTransform::Identity;
		float conf = bodyData.keypoint_confidence[i];
		FVector vec_conf = FVector(conf,conf,conf);
		if (vec_conf.ContainsNaN())
		{
			kp_conf.SetLocation(vec_conf);
		}
		transforms.Push(kp_conf);
	}
	SkeletonsData.Skeleton = transforms;
	return SkeletonsData;
}

void PopulateSkeletonsData(SL_Bodies& bodies) {

	TArray<int> remainingKeyList;
	StreamedSkeletons.GetKeys(remainingKeyList);
	for (int i = 0; i < bodies.nb_bodies; i++)
	{
		SL_BodyData bodyData = bodies.body_list[i];

		if (bodyData.tracking_state == sl::OBJECT_TRACKING_STATE::OK)
		{
			if (!StreamedSkeletons.Contains(bodyData.id))  // If it's a new ID
			{
 				UpdateSkeletonStaticData(FName(FString::FromInt(bodyData.id)));
				StreamedSkeletonData data = BuildSkeletonsTransformFromZEDObjects(bodyData, bodies.timestamp);
				StreamedSkeletons.Add(bodyData.id, data);
			}
			else
			{
				StreamedSkeletons[bodyData.id] = BuildSkeletonsTransformFromZEDObjects(bodyData, bodies.timestamp);
				remainingKeyList.Remove(bodyData.id);
			}
		}
		else
		{
		} 
	}
	for (int index = 0; index < remainingKeyList.Num(); index++)
	{
		LiveLinkProvider->RemoveSubject(FName(FString::FromInt(remainingKeyList[index])));
		StreamedSkeletons.Remove(remainingKeyList[index]);
	}
}

//// Update Camera static data
void UpdateCameraStaticData(FName SubjectName)
{
	FLiveLinkStaticDataStruct StaticData(FLiveLinkCameraStaticData::StaticStruct());
	FLiveLinkCameraStaticData& CameraData = *StaticData.Cast<FLiveLinkCameraStaticData>();
	CameraData.bIsAspectRatioSupported = true;
	CameraData.bIsFieldOfViewSupported = true;
	CameraData.bIsFocalLengthSupported = false;
	CameraData.bIsFocusDistanceSupported = false;
	CameraData.bIsProjectionModeSupported = true;
	LiveLinkProvider->UpdateSubjectStaticData(SubjectName, ULiveLinkCameraRole::StaticClass(), MoveTemp(StaticData));
}

//// Update Camera Frame data
void UpdateCameraFrameData(FName SubjectName, ZEDCamera& zed)
{
	FLiveLinkFrameDataStruct FrameData(FLiveLinkCameraFrameData::StaticStruct());
	FLiveLinkCameraFrameData& CameraData = *FrameData.Cast<FLiveLinkCameraFrameData>();
	SL_PoseData pose;
	zed.GetPosition(pose, sl::REFERENCE_FRAME::WORLD);
	FTransform Pose = BuildUETransformFromZEDTransform(pose);
	CameraData.AspectRatio = 16. / 9;
	CameraData.FieldOfView = zed.GetCalibrationParameters(false)->left_cam.h_fov;
	CameraData.ProjectionMode = ELiveLinkCameraProjectionMode::Perspective;
	CameraData.Transform = Pose;
	double StreamTime = FPlatformTime::Seconds();
	CameraData.WorldTime = StreamTime;
	LiveLinkProvider->UpdateSubjectFrameData(SubjectName, MoveTemp(FrameData));
}

//// Update Skeleton static data
void UpdateSkeletonStaticData(FName SubjectName)
{
	FLiveLinkStaticDataStruct StaticData(FLiveLinkSkeletonStaticData::StaticStruct());
	FLiveLinkSkeletonStaticData& AnimationData = *StaticData.Cast<FLiveLinkSkeletonStaticData>();

	for (int i = 0; i < targetBone.Num(); i++)
	{
		AnimationData.BoneNames.Add(FName(targetBone[i]));
		AnimationData.BoneParents.Add(parentsIdx[i]);
	}

	LiveLinkProvider->UpdateSubjectStaticData(SubjectName, ULiveLinkAnimationRole::StaticClass(), MoveTemp(StaticData));
}

//// Update Skeleton frame data
void UpdateAnimationFrameData(StreamedSkeletonData& StreamedSkeleton)
{
	FLiveLinkFrameDataStruct FrameData(FLiveLinkAnimationFrameData::StaticStruct());
	FLiveLinkAnimationFrameData& AnimationData = *FrameData.Cast<FLiveLinkAnimationFrameData>();

	double timestamp = (StreamedSkeleton.Timestamp / 1000000000.0f);// ns to seconds
	double StreamTime = FPlatformTime::Seconds();
	AnimationData.WorldTime = StreamTime;
	AnimationData.Transforms = StreamedSkeleton.Skeleton;
	LiveLinkProvider->UpdateSubjectFrameData(StreamedSkeleton.SubjectName, MoveTemp(FrameData));
}
