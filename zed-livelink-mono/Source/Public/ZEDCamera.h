#ifndef __ZED_CAMERA_H__
#define __ZED_CAMERA_H__

#include "Roles/LiveLinkCameraRole.h"
#include "Roles/LiveLinkCameraTypes.h"
#include "LiveLinkProvider.h"

#include "RequiredProgramMainCPPInclude.h"
#include "Modules/ModuleManager.h"

#include "LiveLinkRefSkeleton.h"
#include "Roles/LiveLinkAnimationRole.h"
#include "Roles/LiveLinkAnimationTypes.h"
#include "ZEDStructs.h"


typedef int(*__RetrieveBodies)(int id, SL_BodyTrackingRuntimeParameters& bt_params, SL_Bodies& bodies, unsigned int instance_id);
typedef int(*__Open)(int id, SL_InitParameters& initParameters, const unsigned int serial_number, const char* pathSVO, const char* ip, int streamPort, const char* outputFile,
	const char* opt_settings_path, const char* opencv_calib_path);
typedef void(*__Close)(int id);
typedef int(*__EnableTracking)(int id, SL_PositionalTrackingParameters tracking_param, const char* areaDBpath);
typedef void(*__DisableTracking)(int id, const char* path);
typedef int(*__EnableBodyTracking)(int, SL_BodyTrackingParameters& bt_params);
typedef void(*__DisableBodyTracking)(int id, unsigned int instance_id, bool force_disable_all_instances);
typedef int(*__GetPosition)(int id, SL_PoseData& poseData, sl::REFERENCE_FRAME reference_frame);
typedef bool(*__CreateCamera)(int id);
typedef int(*__SN)(int sn);
typedef int(*__Grab)(int id, SL_RuntimeParameters& rt_params);
typedef SL_CalibrationParameters* (*__CalibParams)(int id, bool raw_params);
typedef void(*__SetSVOPosition)(int id, int frame_nb);
typedef void(*__StartPublishing)(int id, const char* json_config_filename);


class ZEDCamera {
public:
	ZEDCamera();
	~ZEDCamera();

	bool CreateCamera(int id);
	sl::ERROR_CODE Open(SL_InitParameters& initParameters, const unsigned int serialNumber, const char* pathSVO, const char* ip,
		int streamPort, const char* outputFile = "", const char* optSettingsPath = "", const char* opencvCalibPath = "");
	void Close();
	sl::ERROR_CODE Grab(SL_RuntimeParameters& runtimeParams);
	sl::ERROR_CODE EnablePositionalTracking(SL_PositionalTrackingParameters tracking_params);
	void DisableTracking(const char* path = "");
	sl::ERROR_CODE EnableBodyTracking(SL_BodyTrackingParameters& params);
	void DisableBodyTracking(unsigned int instanceId = 0, bool forceDisableAllInstances = false);
	sl::POSITIONAL_TRACKING_STATE GetPosition(SL_PoseData& poseData, sl::REFERENCE_FRAME referenceFrame);
	int GetSerialNumber();
	sl::ERROR_CODE RetrieveBodies(SL_BodyTrackingRuntimeParameters& bt_params, SL_Bodies& bodies, unsigned int instanceId = 0);
	SL_CalibrationParameters* GetCalibrationParameters(bool raw_params = false);
	void SetSVOPosition(int frame_number);
	void StartPublishing(std::string jsonConfigFilename);

private:
	void* v_dllHandle;
	__CreateCamera m_funcCreateCamera;
	__Open m_funcOpen;
	__Close m_funcClose;
	__EnableTracking m_funcEnableTracking;
	__DisableTracking m_funcDisableTracking;
	__EnableBodyTracking m_funcEnableBT;
	__DisableBodyTracking m_funcDisableBT;
	__GetPosition m_funcGetPosition;
	__RetrieveBodies m_funcRetrieveBT;
	__Grab m_funcGrab;
	__SN m_funcGetSN;
	__CalibParams m_funcGetCalibParams;
	__SetSVOPosition m_funcSetSVOPosition;
	__StartPublishing m_funcStartPublishing;

	int camera_id;

	bool ImportMethod_CreateCamera();
	bool ImportMethod_Open();
	bool ImportMethod_Close();
	bool ImportMethod_Grab();
	bool ImportMethod_EnableTracking();
	bool ImportMethod_DisableTracking();
	bool ImportMethod_EnableBT();
	bool ImportMethod_DisableBT();
	bool ImportMethod_GetPosition();
	bool ImportMethod_GetSerialNumber();
	bool ImportMethod_RetrieveBodies();
	bool ImportMethod_GetCalibParams();
	bool ImportMethod_SetSVOPosition();
	bool ImportMethod_StartPublishing();

	bool LoadDll(FString DLLName);
	void UnloadDll();

};
#endif