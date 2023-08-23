#ifndef __ARUCO_DETECTOR_CPP__
#define __ARUCO_DETECTOR_CPP__

#include "ArucoDetector.hpp"

ArucoDetector::ArucoDetector()
{
	FString name_dll = "zed_opencv.dll"; 
	LoadDll(name_dll);
}

ArucoDetector::~ArucoDetector()
{
	UnloadDll();
}

bool ArucoDetector::ImportMethod_Init() {
	if (v_dllHandle != NULL)
	{
		m_func_InitArucoDetector = NULL;
		FString procName = "init";// Needs to be the exact name of the DLL method.
		m_func_InitArucoDetector = (__InitArucoDetector)FPlatformProcess::GetDllExport(v_dllHandle, *procName);
		if (m_func_InitArucoDetector != NULL)
		{
			return true;
		}
	}
	return false;// Return an error.
}

bool ArucoDetector::ImportMethod_DetectMarkers() {
	if (v_dllHandle != NULL)
	{
		m_func_DetectMarkers = NULL;
		FString procName = "detectMarkers";// Needs to be the exact name of the DLL method.
		m_func_DetectMarkers = (__DetectMarkers)FPlatformProcess::GetDllExport(v_dllHandle, *procName);
		if (m_func_DetectMarkers != NULL)
		{
			return true;
		}
	}
	return false;// Return an error.
}

bool ArucoDetector::ImportMethod_GetPose() {
	if (v_dllHandle != NULL)
	{
		m_func_GetArucoPose = NULL;
		FString procName = "getPose";// Needs to be the exact name of the DLL method.
		m_func_GetArucoPose = (__GetArucoPose)FPlatformProcess::GetDllExport(v_dllHandle, *procName);
		if (m_func_GetArucoPose != NULL)
		{
			return true;
		}
	}
	return false;// Return an error.
}

bool ArucoDetector::LoadDll(FString DLLName)
{
	if (FPaths::FileExists(DLLName))
	{
		v_dllHandle = FPlatformProcess::GetDllHandle(*DLLName); // Retrieve the DLL.
		if (v_dllHandle != NULL)
		{
			ImportMethod_Init();
			ImportMethod_DetectMarkers();
			ImportMethod_GetPose();
			return true;
		}
	}
	return false;// Return an error.
}

void ArucoDetector::UnloadDll() {
	if (v_dllHandle == NULL)
	{
		return;
	}
	// release dll handle
	FPlatformProcess::FreeDllHandle(v_dllHandle);
	v_dllHandle = NULL;
}

void ArucoDetector::Init(float actual_marker_size_meters, PREDEFINED_DICTIONARY_NAME dictionary_name, float fx, float fy, float cx, float cy)
{
	if (m_func_InitArucoDetector == NULL)
	{
		return;
	}
	m_func_InitArucoDetector(actual_marker_size_meters, dictionary_name, fx, fy, cx, cy);
}

void ArucoDetector::DetectMarkers(int width, int height, unsigned char* img_ptr)
{
	if (m_func_DetectMarkers == NULL)
	{
		return;
	}
	m_func_DetectMarkers(width, height, img_ptr);
}

bool ArucoDetector::GetPose(float& t_x, float& t_y, float& t_z, float& q_x, float& q_y, float& q_z, float& q_w)
{
	if (m_func_DetectMarkers == NULL)
	{
		return false;
	}
	return m_func_GetArucoPose(t_x, t_y, t_z, q_x, q_y, q_z, q_w);
}

#endif