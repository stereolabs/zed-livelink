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

#include <sl/Camera.hpp>

#include "Roles/LiveLinkCameraRole.h"
#include "Roles/LiveLinkCameraTypes.h"
#include "LiveLinkProvider.h"
#include "RequiredProgramMainCPPInclude.h"
#include "Modules/ModuleManager.h"
#include "LiveLinkRefSkeleton.h"
#include "Roles/LiveLinkAnimationRole.h"
#include "Roles/LiveLinkAnimationTypes.h"

#include "ZEDCamera.h"
#include "ZEDStructs.h"
#include "Utils/Util.h"
#include "ArucoDetector.hpp"

IMPLEMENT_APPLICATION(ZEDLiveLinkPlugin, "ZEDLiveLink");

using namespace sl;
using namespace std;

static TSharedPtr<ILiveLinkProvider> LiveLinkProvider;

struct StreamedSkeletonData
{
	FName SubjectName;
	double Timestamp;
	TArray<FTransform> Skeleton;

	StreamedSkeletonData() {}
	StreamedSkeletonData(FName inSubjectName) : SubjectName(inSubjectName) {}
};

struct StreamedCameraData
{
	FName SubjectName;
	ZEDCamera* Cam;
	StreamedCameraData() {}
	StreamedCameraData(FName inSubjectName, ZEDCamera* InCam) : SubjectName(inSubjectName), Cam(InCam) {}
};


void LibInit();
void parseArgs(int argc, char** argv, SL_InitParameters& init_parameters, string& pathSVO, string& ip, int& port);
void UpdateCameraStaticData(FName SubjectName);
void UpdateArucoStaticData(FName SubjectName);
void UpdateArucoFrameData(FName SubjectName, SL_PoseData pose);
void UpdateCameraFrameData(FName SubjectName, ZEDCamera& camera);
void UpdateSkeletonStaticData(FName SubjectName);
void UpdateAnimationFrameData(StreamedSkeletonData StreamedSkeleton);
ERROR_CODE PopulateSkeletonsData(ZEDCamera* cam, ZEDConfig& config);
ERROR_CODE InitCamera(int argc, char **argv, ZEDConfig& config);
FTransform BuildUETransformFromZEDTransform(SL_PoseData& pose);
StreamedSkeletonData BuildSkeletonsTransformFromZEDObjects(SL_BodyData& objectData, double timestamp);

bool IsConnected = false;
bool enableBodyTracking = false;

bool enableArucoDetection = true;
FName ArucoSubjectName = "Aruco";

// Streamed Data
StreamedCameraData StreamedCamera;
TMap<int, StreamedSkeletonData> StreamedSkeletons;

TArray<FString> targetBone;
TArray<int> parentsIdx;

////////////////////////////////////////
///////////// MAIN ////////////////////
///////////////////////////////////////

int main(int argc, char **argv)
{
	ZEDConfig zed_config;
	std::string zed_config_file("ZEDLiveLinkConfig.json"); // Default name and location.
	if (argc == 2)
	{
		zed_config_file = argv[1];
		std::cout << "Loading " << zed_config_file << " config file.";
	}
	else if (argc > 2)
	{
		std::cout << "Unexecpected arguments, exiting..." << std::endl;
		return EXIT_FAILURE;
	}
	else {
		std::cout << "Trying to load default config file 'ZEDLiveLinkConfig.json' " << std::endl;
	}
	readZEDConfig(zed_config_file, zed_config);
	std::cout << "Starting ZEDLiveLink tool" << endl;
	cout << "Opening camera..." << endl;
	//// Create camera
	ERROR_CODE e = InitCamera(argc, argv, zed_config);
	if (e != ERROR_CODE::SUCCESS) {
		cout << "Error " << e << ", exit program.\n";
		return EXIT_FAILURE;
	}

	if (zed_config.body_format == sl::BODY_FORMAT::BODY_34)
	{
		targetBone = targetBone34;
		parentsIdx = parents34Idx;
	}
	else if (zed_config.body_format == sl::BODY_FORMAT::BODY_38)
	{
		targetBone = targetBone38;
		parentsIdx = parents38Idx;
	}
#if 0
	else if (zed_config.body_format == sl::BODY_FORMAT::BODY_70)
	{
		targetBone = targetBone70;
		parentsIdx = parents70Idx;
	}
#endif
	else if (zed_config.body_format == sl::BODY_FORMAT::BODY_18)
	{
		std::cout << "Body 18 is not compatible. Please use Body 34/38 " << std::endl;
		return EXIT_FAILURE;
	}

	LibInit();
	LiveLinkProvider = ILiveLinkProvider::CreateLiveLinkProvider(TEXT("ZED") + StreamedCamera.SubjectName.ToString());

	cout << "Waiting for connection..." << endl;
	//// Update static camera data.
	if (LiveLinkProvider.IsValid()) {
		UpdateCameraStaticData(StreamedCamera.SubjectName);

		if (enableArucoDetection)
		{
			UpdateArucoStaticData(ArucoSubjectName);
		}
	}

	ArucoDetector detector;
	SL_CalibrationParameters calibration_parameters = *StreamedCamera.Cam->GetCalibrationParameters();

	detector.Init(0.135, PREDEFINED_DICTIONARY_NAME::DICT_6X6_100, calibration_parameters.left_cam.fx, calibration_parameters.left_cam.fy, calibration_parameters.left_cam.cx, calibration_parameters.left_cam.cy);

	sl::Mat img(sl::Resolution(calibration_parameters.left_cam.image_size.width, calibration_parameters.left_cam.image_size.height), sl::MAT_TYPE::U8_C4);
	//// Loop
	while (true) {
		//// Display status
		if (LiveLinkProvider->HasConnection()) 
		{
			if (!IsConnected) {
				IsConnected = true;
				cout << "ZEDLiveLink is connected " << endl;
				cout << "ZED Camera added : " << TCHAR_TO_UTF8(*StreamedCamera.SubjectName.ToString()) << endl;
			}
			bool saved = false;
			// Grab
			///// Update Streamed data
			SL_RuntimeParameters rt_params;
			rt_params.reference_frame = sl::REFERENCE_FRAME::WORLD;
			sl::ERROR_CODE err = StreamedCamera.Cam->Grab(rt_params);
			if (err == sl::ERROR_CODE::SUCCESS) {
				UpdateCameraFrameData(StreamedCamera.SubjectName, *StreamedCamera.Cam);
		
				if (enableArucoDetection) {
					err = StreamedCamera.Cam->RetrieveImage(img, sl::VIEW::LEFT, sl::MEM::CPU);

					if (err == sl::ERROR_CODE::SUCCESS)
					{
						detector.DetectMarkers(img.getWidth(), img.getHeight(), img.getPtr<sl::uchar1>());

						float t_x, t_y, t_z, q_x, q_y, q_z, q_w;
						bool status = detector.GetPose(t_x, t_y, t_z, q_x, q_y, q_z, q_w);

						if (status)
						{
							std::cout << t_x << ", " << t_y << ", " << t_z << std::endl;

							SL_PoseData pose_data;
							SL_Vector3 position(t_x, t_y, t_z);
							SL_Quaternion rotation(q_x, q_y, q_z, q_w);
							
							pose_data.translation = position;
							pose_data.rotation = rotation;

							UpdateArucoFrameData(ArucoSubjectName, pose_data);
						}
					}
				}

				if (enableBodyTracking) {
					e = PopulateSkeletonsData(StreamedCamera.Cam, zed_config);
					if (e != ERROR_CODE::SUCCESS) {
						cout << "Error " << e << ", exit program.\n";
						return EXIT_FAILURE;
					}
					for (auto& it : StreamedSkeletons) {
						UpdateAnimationFrameData(it.Value);
					}
				}
			}
			else if (err == sl::ERROR_CODE::END_OF_SVOFILE_REACHED) {
				std::cout << "End of SVO reached " << std::endl;
				StreamedCamera.Cam->SetSVOPosition(0);
			}
			else {
				std::cout << "Grab Failed " << std::endl;
			}
		}
		else if (IsConnected == true) {
			cout << "Source ZED removed" << endl;
			IsConnected = false;
		}
	}
	// Disable positional tracking and close the camera
	StreamedCamera.Cam->DisableTracking();
	if (enableBodyTracking) {
		StreamedCamera.Cam->DisableBodyTracking();
	}
	StreamedCamera.Cam->Close();

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

ERROR_CODE InitCamera(int argc, char **argv, ZEDConfig& config)
{
	string pathSVO = config.svo_path;
	string ip = config.ip;
	int port = config.port;
	ZEDCamera* zed = new ZEDCamera();
	if (!zed->CreateCamera(config.id))
	{
		std::cout << " ERROR :Create cam" << std::endl;
		return sl::ERROR_CODE::FAILURE;
	}
	SL_InitParameters init_params;
	init_params.camera_resolution = config.resolution;
	init_params.camera_fps = config.fps;
	init_params.coordinate_system = sl::COORDINATE_SYSTEM::LEFT_HANDED_Z_UP;
	init_params.coordinate_units = sl::UNIT::CENTIMETER;
	init_params.depth_mode = config.depth_mode;
	init_params.sdk_verbose = 1;
	init_params.input_type = config.input_type;
	init_params.grab_compute_capping_fps = config.grab_compute_capping_fps;
	//parseArgs(argc, argv, init_params, pathSVO, ip, port);
	ERROR_CODE err = zed->Open(init_params, config.serial_number, pathSVO.c_str(), ip.c_str(), port);

	if (err != ERROR_CODE::SUCCESS)
	{
		std::cout << "ERROR : Open" << std::endl;
		return err;
	}

	StreamedCamera = StreamedCameraData(FName(FString::FromInt((zed->GetSerialNumber()))), zed);

	SL_PositionalTrackingParameters tracking_param;
	tracking_param.set_floor_as_origin = true;
	tracking_param.enable_pose_smoothing = config.enable_pose_smoothing;
	tracking_param.enable_area_memory = config.enable_area_memory;
	err = zed->EnablePositionalTracking(tracking_param);
	if (err != ERROR_CODE::SUCCESS)
	{
		std::cout << " ERROR : Enable Tracking" << std::endl;
		return err;
	}

	enableBodyTracking = config.enable_body_tracking_module;
	if (enableBodyTracking) {
		SL_BodyTrackingParameters body_tracking_params;
		body_tracking_params.image_sync = true;
		body_tracking_params.enable_body_fitting = true;
		body_tracking_params.enable_tracking = true;
		body_tracking_params.detection_model = config.detection_model;
		body_tracking_params.body_format = config.body_format;
		body_tracking_params.body_selection = config.body_selection;
		body_tracking_params.max_range = config.max_range;
		err = zed->EnableBodyTracking(body_tracking_params);
		if (err != ERROR_CODE::SUCCESS)
		{
			std::cout << "ERROR : Enable OD" << std::endl;
			return err;
		}
	}
	return err;
}

StreamedSkeletonData BuildSkeletonsTransformFromZEDObjects(SL_BodyData& bodyData, double timestamp)
{
	StreamedSkeletonData SkeletonsData = StreamedSkeletonData(FName(FString::FromInt(bodyData.id)));
	SkeletonsData.Timestamp = timestamp;
	TMap<FString, FTransform> rigBoneTarget;
	sl::float3 bodyPosition = bodyData.keypoint[0];
	sl::float4 bodyRotation = bodyData.global_root_orientation;

	for (int i = 0; i < targetBone.Num(); i++)
	{
		rigBoneTarget.Add(targetBone[i], FTransform::Identity);
	}
	
	FVector position = FVector(bodyPosition.x, bodyPosition.y, bodyPosition.z);
	FQuat global_rotation = FQuat(bodyRotation.x, bodyRotation.y, bodyRotation.z, bodyRotation.w);

	if (position.ContainsNaN())
	{
		position = FVector::ZeroVector;
	}

	rigBoneTarget["PELVIS"].SetLocation(position);
	rigBoneTarget["PELVIS"].SetRotation(global_rotation.GetNormalized());


	for (int i = 1; i < targetBone.Num() / 2; i++)
	{
		sl::float3 localTranslation = bodyData.local_position_per_joint[i];
		sl::float4 localRotation = bodyData.local_orientation_per_joint[i];
		sl::float3 worldTranslation = bodyData.keypoint[i];

		position = FVector(worldTranslation.x, worldTranslation.y, worldTranslation.z);
		if (position.ContainsNaN())
		{
			position = FVector::ZeroVector;
		}

		FQuat jointRotation = FQuat(localRotation.x, localRotation.y, localRotation.z, localRotation.w).GetNormalized();

		if (jointRotation.ContainsNaN())
		{
			position = FVector::ZeroVector;
		}

		rigBoneTarget[targetBone[i]].SetLocation(position);
		rigBoneTarget[targetBone[i]].SetRotation(jointRotation);

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
		kp_conf.SetLocation(FVector(bodyData.keypoint_confidence[i], bodyData.keypoint_confidence[i], bodyData.keypoint_confidence[i]));
		transforms.Push(kp_conf);
	}
	SkeletonsData.Skeleton = transforms;
	return SkeletonsData;
}

ERROR_CODE PopulateSkeletonsData(ZEDCamera* zed, ZEDConfig& config)
{
	ERROR_CODE e = ERROR_CODE::FAILURE;
	SL_BodyTrackingRuntimeParameters body_tracking_rt_params;
	body_tracking_rt_params.detection_confidence_threshold = config.detection_confidence;
	body_tracking_rt_params.minimum_keypoints_threshold = config.minimum_keypoints_threshold;
	body_tracking_rt_params.skeleton_smoothing = config.skeleton_smoothing;
	SL_Bodies bodies;
	e = zed->RetrieveBodies(body_tracking_rt_params, bodies, 0);
	if (e != ERROR_CODE::SUCCESS)
	{
		cout << "ERROR : retrieve objects : " << e << std::endl;
		return e;
	}
	if (bodies.is_new == 1)
	{
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
		}
		for (int index = 0; index < remainingKeyList.Num(); index++)
		{
			LiveLinkProvider->RemoveSubject(FName(FString::FromInt(remainingKeyList[index])));
			StreamedSkeletons.Remove(remainingKeyList[index]);
		}
	}
	return e;
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

void UpdateArucoStaticData(FName SubjectName)
{
	FLiveLinkStaticDataStruct StaticData(FLiveLinkTransformStaticData::StaticStruct());
	FLiveLinkTransformStaticData& TransformData = *StaticData.Cast<FLiveLinkTransformStaticData>();
	TransformData.bIsLocationSupported = true;
	TransformData.bIsRotationSupported = true;

	LiveLinkProvider->UpdateSubjectStaticData(SubjectName, ULiveLinkTransformRole::StaticClass(), MoveTemp(StaticData));
}

void UpdateArucoFrameData(FName SubjectName, SL_PoseData pose)
{
	FLiveLinkFrameDataStruct FrameData(FLiveLinkTransformFrameData::StaticStruct());
	FLiveLinkTransformFrameData& TransformData = *FrameData.Cast<FLiveLinkTransformFrameData>();

	FTransform Transform = BuildUETransformFromZEDTransform(pose);
	TransformData.Transform = Transform;
	double StreamTime = FPlatformTime::Seconds();
	TransformData.WorldTime = StreamTime;
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
void UpdateAnimationFrameData(StreamedSkeletonData StreamedSkeleton)
{
	FLiveLinkFrameDataStruct FrameData(FLiveLinkAnimationFrameData::StaticStruct());
	FLiveLinkAnimationFrameData& AnimationData = *FrameData.Cast<FLiveLinkAnimationFrameData>();

	double timestamp = (StreamedSkeleton.Timestamp / 1000000000.0f);// ns to seconds
	double StreamTime = FPlatformTime::Seconds();
	AnimationData.WorldTime = StreamTime;
	AnimationData.Transforms = StreamedSkeleton.Skeleton;
	LiveLinkProvider->UpdateSubjectFrameData(StreamedSkeleton.SubjectName, MoveTemp(FrameData));

}

void parseArgs(int argc, char **argv, SL_InitParameters& param, string& pathSVO, string& ip, int& port)
{
	if (argc > 1 && string(argv[1]).find(".svo") != string::npos) {
		// SVO input mode
		param.input_type = sl::INPUT_TYPE::SVO;
		pathSVO = string(argv[1]);
		cout << "[Sample] Using SVO File input: " << argv[1] << endl;
	}
	else if (argc > 1 && string(argv[1]).find(".svo") == string::npos) {
		string arg = string(argv[1]);
		unsigned int a, b, c, d, p;
		if (sscanf(arg.c_str(), "%u.%u.%u.%u:%d", &a, &b, &c, &d, &p) == 5) {
			// Stream input mode - IP + port
			string ip_adress = to_string(a) + "." + to_string(b) + "." + to_string(c) + "." + to_string(d);
			param.input_type = sl::INPUT_TYPE::STREAM;
			ip = string(ip_adress);
			port = p;
			cout << "[Sample] Using Stream input, IP : " << ip_adress << ", port : " << p << endl;
		}
		else if (sscanf(arg.c_str(), "%u.%u.%u.%u", &a, &b, &c, &d) == 4) {
			// Stream input mode - IP only
			param.input_type = sl::INPUT_TYPE::STREAM;
			ip = argv[1];
			cout << "[Sample] Using Stream input, IP : " << argv[1] << endl;
		}
		else if (arg.find("HD2K") != string::npos) {
			param.camera_resolution = RESOLUTION::HD2K;
			cout << "[Sample] Using Camera in resolution HD2K" << endl;
		}
		else if (arg.find("HD1080") != string::npos) {
			param.camera_resolution = RESOLUTION::HD1080;
			cout << "[Sample] Using Camera in resolution HD1080" << endl;
		}
		else if (arg.find("HD720") != string::npos) {
			param.camera_resolution = RESOLUTION::HD720;
			cout << "[Sample] Using Camera in resolution HD720" << endl;
		}
		else if (arg.find("VGA") != string::npos) {
			param.camera_resolution = RESOLUTION::VGA;
			cout << "[Sample] Using Camera in resolution VGA" << endl;
		}
	}
}
