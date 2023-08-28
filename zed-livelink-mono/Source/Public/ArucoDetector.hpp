#ifndef __ARUCO_DETECTOR_H__
#define __ARUCO_DETECTOR_H__

#include "Roles/LiveLinkCameraRole.h"
#include "Roles/LiveLinkCameraTypes.h"
#include "LiveLinkProvider.h"

#include "RequiredProgramMainCPPInclude.h"
#include "Modules/ModuleManager.h"

#include "LiveLinkRefSkeleton.h"
#include "Roles/LiveLinkAnimationRole.h"
#include "Roles/LiveLinkAnimationTypes.h"
#include "ZEDStructs.h"


typedef void(*__InitArucoDetector)(float actual_marker_size_meters, PREDEFINED_DICTIONARY_NAME dictionary_name, float fx, float fy, float cx, float cy, bool display_image);
typedef void(*__DetectMarkers)(int width, int height, unsigned char* img_ptr);
typedef bool(*__GetArucoPose)(float& t_x, float& t_y, float& t_z, float& q_x, float& q_y, float& q_z, float& q_w);

class ArucoDetector
{
public :
	ArucoDetector();
	~ArucoDetector();

	void Init(float actual_marker_size_meters, PREDEFINED_DICTIONARY_NAME dictionary_name, float fx, float fy, float cx, float cy, bool display_image = false);

	void DetectMarkers(int width, int height, unsigned char* img_ptr);

	bool GetPose(sl::Transform& pose);

private:

	void* v_dllHandle;

	__InitArucoDetector m_func_InitArucoDetector;
	__DetectMarkers m_func_DetectMarkers;
	__GetArucoPose m_func_GetArucoPose;

	bool ImportMethod_Init();
	bool ImportMethod_DetectMarkers();
	bool ImportMethod_GetPose();

	bool LoadDll(FString DLLName);
	void UnloadDll();
};


#endif