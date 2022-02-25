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


typedef int(*__ObjectData)(int id, SL_ObjectDetectionRuntimeParameters& od_params, SL_Objects& objs);
typedef int(*__Open)(int id, SL_InitParameters& initParameters, const char* pathSVO, const char* ip, int streamPort, const char* outputFile,
const char* opt_settings_path, const char* opencv_calib_path);
typedef void(*__Close)(int id);
typedef int(*__EnableTracking)(int id, SL_PositionalTrackingParameters tracking_param, const char* areaDBpath);
typedef void(*__DisableTracking)(int id, const char* path);
typedef int(*__EnableOD)(int, SL_ObjectDetectionParameters& od_params);
typedef void(*__DisableOD)(int);
typedef void(*__DisableOD)(int id);
typedef int(*__GetPosition)(int id, PoseData& poseData, sl::REFERENCE_FRAME reference_frame);
typedef bool(*__CreateCamera)(int id, bool verbose);
typedef int(*__SN)(int sn);
typedef int(*__Grab)(int id, SL_RuntimeParameters& rt_params);
typedef SL_CalibrationParameters*(*__CalibParams)(int id, bool raw_params);
typedef void(*__SetSVOPosition)(int id, int frame_nb);


class ZEDCamera {
public:
	ZEDCamera();
	~ZEDCamera();

	bool CreateCamera(int id, bool verbose);
	sl::ERROR_CODE Open(SL_InitParameters& initParameters, const char* pathSVO, const char* ip,
		int streamPort, const char* outputFile = "", const char* opt_settings_path = "", const char* opencv_calib_path = "");
	void Close();
	sl::ERROR_CODE Grab(SL_RuntimeParameters& runtimeParams);
	sl::ERROR_CODE EnableTracking(SL_PositionalTrackingParameters tracking_params);
	void DisableTracking(const char* path = "");
	sl::ERROR_CODE EnableObjectDetection(SL_ObjectDetectionParameters& params);
	void DisableObjectDetection();
	sl::POSITIONAL_TRACKING_STATE GetPosition(PoseData& poseData, sl::REFERENCE_FRAME reference_frame);
	int GetSerialNumber();
	sl::ERROR_CODE RetrieveObjects(SL_ObjectDetectionRuntimeParameters& od_params, SL_Objects& objs);
	SL_CalibrationParameters* GetCalibrationParameters(bool raw_params = false);
	void setSVOPosition(int frame_number);

private:
	void *v_dllHandle;
	__CreateCamera m_funcCreateCamera;
	__Open m_funcOpen;
	__Close m_funcClose;
	__EnableTracking m_funcEnableTracking;
	__DisableTracking m_funcDisableTracking;
	__EnableOD m_funcEnableOD;
	__DisableOD m_funcDisableOD;
	__GetPosition m_funcGetPosition;
	__ObjectData m_funcRetrieveOD;
	__Grab m_funcGrab;
	__SN m_funcGetSN;
	__CalibParams m_funcGetCalibParams;
	__SetSVOPosition m_funcSetSVOPosition;

	int camera_id;

	bool ImportMethod_CreateCamera();
	bool ImportMethod_Open();
	bool ImportMethod_Close();
	bool ImportMethod_Grab();
	bool ImportMethod_EnableTracking();
	bool ImportMethod_DisableTracking();
	bool ImportMethod_EnableOD();
	bool ImportMethod_DisableOD();
	bool ImportMethod_GetPosition();
	bool ImportMethod_GetSerialNumber();
	bool ImportMethod_RetrieveObjects();
	bool ImportMethod_GetCalibParams();
	bool ImportMethod_SetSVOPosition();

	bool LoadDll(FString DLLName);
	void UnloadDll();

};
#endif