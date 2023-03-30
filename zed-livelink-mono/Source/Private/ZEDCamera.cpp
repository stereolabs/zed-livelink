#ifndef __ZED_CAMERA_CPP__
#define __ZED_CAMERA_CPP__

#include "ZEDCamera.h"

///////////////////////////////////////
//////////// IMPORT DLL ///////////////
///////////////////////////////////////

ZEDCamera::ZEDCamera() {
	FString name_dll;

#if PLATFORM_LINUX
	name_dll = "libsl_zed_c.so";
#elif PLATFORM_WINDOWS
	name_dll = "sl_zed_c.dll";
#endif 
	LoadDll(name_dll);
}

ZEDCamera::~ZEDCamera() {
	UnloadDll();
}

bool ZEDCamera::ImportMethod_CreateCamera() {
	if (v_dllHandle != NULL)
	{
		m_funcCreateCamera = NULL;
		FString procName = "sl_create_camera";// Needs to be the exact name of the DLL method.
		m_funcCreateCamera = (__CreateCamera)FPlatformProcess::GetDllExport(v_dllHandle, *procName);
		if (m_funcCreateCamera != NULL)
		{
			return true;
		}
	}
	return false;// Return an error.
}

bool ZEDCamera::ImportMethod_GetCalibParams() {
	if (v_dllHandle != NULL)
	{
		m_funcGetCalibParams = NULL;
		FString procName = "sl_get_calibration_parameters";// Needs to be the exact name of the DLL method.
		m_funcGetCalibParams = (__CalibParams)FPlatformProcess::GetDllExport(v_dllHandle, *procName);
		if (m_funcGetCalibParams != NULL)
		{
			return true;
		}
	}
	return false;// Return an error.
}

bool ZEDCamera::ImportMethod_Open()
{
	if (v_dllHandle != NULL)
	{
		m_funcOpen = NULL;
		FString procName = "sl_open_camera";// Needs to be the exact name of the DLL method.
		m_funcOpen = (__Open)FPlatformProcess::GetDllExport(v_dllHandle, *procName);
		if (m_funcOpen != NULL)
		{
			return true;
		}
	}
	return false;// Return an error.
}

bool ZEDCamera::ImportMethod_Close()
{
	if (v_dllHandle != NULL)
	{
		m_funcClose = NULL;
		FString procName = "sl_close_camera";// Needs to be the exact name of the DLL method.
		m_funcClose = (__Close)FPlatformProcess::GetDllExport(v_dllHandle, *procName);
		if (m_funcOpen != NULL)
		{
			return true;
		}
	}
	return false;// Return an error.
}

bool ZEDCamera::ImportMethod_Grab() {
	if (v_dllHandle != NULL)
	{
		m_funcGrab = NULL;
		FString procName = "sl_grab";// Needs to be the exact name of the DLL method.
		m_funcGrab = (__Grab)FPlatformProcess::GetDllExport(v_dllHandle, *procName);
		if (m_funcGrab != NULL)
		{
			return true;
		}
	}
	return false;// Return an error.
}

bool ZEDCamera::ImportMethod_EnableTracking()
{
	if (v_dllHandle != NULL)
	{
		m_funcEnableTracking = NULL;
		FString procName = "sl_enable_positional_tracking";// Needs to be the exact name of the DLL method.
		m_funcEnableTracking = (__EnableTracking)FPlatformProcess::GetDllExport(v_dllHandle, *procName);
		if (m_funcEnableTracking != NULL)
		{
			return true;
		}
	}
	return false;// Return an error.
}

bool ZEDCamera::ImportMethod_DisableTracking()
{
	if (v_dllHandle != NULL)
	{
		m_funcDisableTracking = NULL;
		FString procName = "sl_disable_positional_tracking";// Needs to be the exact name of the DLL method.
		m_funcDisableTracking = (__DisableTracking)FPlatformProcess::GetDllExport(v_dllHandle, *procName);
		if (m_funcDisableTracking != NULL)
		{
			return true;
		}
	}
	return false;// Return an error.
}

bool ZEDCamera::ImportMethod_EnableBT()
{
	if (v_dllHandle != NULL)
	{
		m_funcEnableBT = NULL;
		FString procName = "sl_enable_body_tracking";// Needs to be the exact name of the DLL method.
		m_funcEnableBT = (__EnableBodyTracking)FPlatformProcess::GetDllExport(v_dllHandle, *procName);
		if (m_funcEnableBT != NULL)
		{
			return true;
		}
	}
	return false;// Return an error.
}

bool ZEDCamera::ImportMethod_DisableBT()
{
	if (v_dllHandle != NULL)
	{
		m_funcDisableBT = NULL;
		FString procName = "sl_disable_body_tracking";// Needs to be the exact name of the DLL method.
		m_funcDisableBT = (__DisableBodyTracking)FPlatformProcess::GetDllExport(v_dllHandle, *procName);
		if (m_funcDisableBT != NULL)
		{
			return true;
		}
	}
	return false;// Return an error.
}

bool ZEDCamera::ImportMethod_GetPosition() {
	if (v_dllHandle != NULL)
	{
		m_funcGetPosition = NULL;
		FString procName = "sl_get_position_data";// Needs to be the exact name of the DLL method.
		m_funcGetPosition = (__GetPosition)FPlatformProcess::GetDllExport(v_dllHandle, *procName);
		if (m_funcGetPosition != NULL)
		{
			return true;
		}
	}
	return false;// Return an error.
}

bool ZEDCamera::ImportMethod_GetSerialNumber()
{
	if (v_dllHandle != NULL)
	{
		m_funcGetSN = NULL;
		FString procName = "sl_get_zed_serial";// Needs to be the exact name of the DLL method.
		m_funcGetSN = (__SN)FPlatformProcess::GetDllExport(v_dllHandle, *procName);
		if (m_funcGetSN != NULL)
		{
			return true;
		}
	}
	return false;// Return an error.
}

bool ZEDCamera::ImportMethod_RetrieveBodies()
{
	if (v_dllHandle != NULL)
	{
		m_funcRetrieveBT = NULL;
		FString procName = "sl_retrieve_bodies";// Needs to be the exact name of the DLL method.
		m_funcRetrieveBT = (__RetrieveBodies)FPlatformProcess::GetDllExport(v_dllHandle, *procName);
		if (m_funcRetrieveBT != NULL)
		{
			return true;
		}
	}
	return false;// Return an error.
}

bool ZEDCamera::ImportMethod_SetSVOPosition()
{
	if (v_dllHandle != NULL)
	{
		m_funcSetSVOPosition = NULL;
		FString procName = "sl_set_svo_position";// Needs to be the exact name of the DLL method.
		m_funcSetSVOPosition = (__SetSVOPosition)FPlatformProcess::GetDllExport(v_dllHandle, *procName);
		if (m_funcSetSVOPosition != NULL)
		{
			return true;
		}
	}
	return false;// Return an error.
}

bool ZEDCamera::ImportMethod_StartPublishing()
{
	if (v_dllHandle != NULL)
	{
		m_funcStartPublishing = NULL;
		FString procName = "sl_start_publishing";// Needs to be the exact name of the DLL method.
		m_funcStartPublishing = (__StartPublishing)FPlatformProcess::GetDllExport(v_dllHandle, *procName);
		if (m_funcStartPublishing != NULL)
		{
			return true;
		}
	}
	return false;// Return an error.
}

bool ZEDCamera::LoadDll(FString DLLName)
{
	if (FPaths::FileExists(DLLName))
	{
		v_dllHandle = FPlatformProcess::GetDllHandle(*DLLName); // Retrieve the DLL.
		if (v_dllHandle != NULL)
		{
			ImportMethod_CreateCamera();
			ImportMethod_Open();
			ImportMethod_GetCalibParams();
			ImportMethod_Close();
			ImportMethod_GetSerialNumber();
			ImportMethod_Grab();
			ImportMethod_EnableTracking();
			ImportMethod_DisableTracking();
			ImportMethod_EnableBT();
			ImportMethod_DisableBT();
			ImportMethod_GetPosition();
			ImportMethod_RetrieveBodies();
			ImportMethod_SetSVOPosition();
			ImportMethod_StartPublishing();
			return true;
		}
	}
	return false;// Return an error.
}

void ZEDCamera::UnloadDll() {
	if (v_dllHandle == NULL)
	{
		return;
	}
	// release dll handle
	FPlatformProcess::FreeDllHandle(v_dllHandle);
	v_dllHandle = NULL;
}

bool ZEDCamera::CreateCamera(int id) {
	if (m_funcCreateCamera == NULL)
	{
		return false;
	}

	camera_id = id;
	return m_funcCreateCamera(id);
}

sl::ERROR_CODE ZEDCamera::Open(SL_InitParameters& initParameters, const unsigned int serial_number, const char* pathSVO, const char* ip, int streamPort, const char* outputFile, const char* opt_settings_path, const char* opencv_calib_path) {
	if (m_funcOpen == NULL)
	{
		return sl::ERROR_CODE::FAILURE;
	}
	sl::ERROR_CODE e = (sl::ERROR_CODE)m_funcOpen(camera_id, initParameters, serial_number, pathSVO, ip, streamPort, outputFile, opt_settings_path, opencv_calib_path);
	return e;
}

SL_CalibrationParameters* ZEDCamera::GetCalibrationParameters(bool raw_params) {
	if (m_funcGetCalibParams == NULL)
	{
		return new SL_CalibrationParameters();
	}
	SL_CalibrationParameters* e = (SL_CalibrationParameters*)m_funcGetCalibParams(camera_id, raw_params);
	return e;
}

void ZEDCamera::Close() {
	if (m_funcClose == NULL)
	{
		return;
	}
	m_funcClose(camera_id);
}

sl::ERROR_CODE ZEDCamera::Grab(SL_RuntimeParameters& rt_params) {
	if (m_funcGrab == NULL)
	{
		return sl::ERROR_CODE::FAILURE;
	}
	sl::ERROR_CODE e = (sl::ERROR_CODE)m_funcGrab(camera_id, rt_params);
	return e;
}

sl::ERROR_CODE ZEDCamera::EnablePositionalTracking(SL_PositionalTrackingParameters tracking_params)
{
	if (m_funcEnableTracking == NULL)
	{
		return sl::ERROR_CODE::FAILURE;
	}

	sl::ERROR_CODE e = (sl::ERROR_CODE)m_funcEnableTracking(camera_id, tracking_params, tracking_params.area_file_path);

	return e;
}

void ZEDCamera::DisableTracking(const char* path) {
	if (m_funcDisableTracking == NULL)
	{
		return;
	}
	m_funcDisableTracking(camera_id, path);
}

sl::ERROR_CODE ZEDCamera::EnableBodyTracking(SL_BodyTrackingParameters& params)
{
	if (m_funcEnableBT == NULL)
	{
		return sl::ERROR_CODE::FAILURE;
	}

	sl::ERROR_CODE e = (sl::ERROR_CODE)m_funcEnableBT(camera_id, params);

	return e;
}

void ZEDCamera::DisableBodyTracking(unsigned int instanceId, bool forceDisableAllInstances) {
	if (m_funcDisableBT == NULL) {
		return;
	}
	m_funcDisableBT(camera_id, instanceId, forceDisableAllInstances);
}

sl::POSITIONAL_TRACKING_STATE ZEDCamera::GetPosition(SL_PoseData& poseData, sl::REFERENCE_FRAME reference_frame)
{
	if (m_funcGetPosition == NULL)
	{
		return sl::POSITIONAL_TRACKING_STATE::OFF;
	}

	sl::POSITIONAL_TRACKING_STATE e = (sl::POSITIONAL_TRACKING_STATE)m_funcGetPosition(camera_id, poseData, reference_frame);

	return e;
}

int ZEDCamera::GetSerialNumber() {
	if (m_funcGetSN == NULL)
	{
		return -1;
	}
	return m_funcGetSN(camera_id);
}

sl::ERROR_CODE ZEDCamera::RetrieveBodies(SL_BodyTrackingRuntimeParameters& bt_params, SL_Bodies& bodies, unsigned int instance_id) {
	if (m_funcRetrieveBT == NULL)
	{
		return sl::ERROR_CODE::FAILURE;
	}
	sl::ERROR_CODE e = (sl::ERROR_CODE)m_funcRetrieveBT(camera_id, bt_params, bodies, instance_id);
	return e;
}

void ZEDCamera::SetSVOPosition(int frame_number) {
	if (m_funcSetSVOPosition == NULL)
	{
		return;
	}
	m_funcSetSVOPosition(camera_id, frame_number);
}

void ZEDCamera::StartPublishing(std::string json_config_filename) {
	if (m_funcStartPublishing == NULL)
	{
		return;
	}
	m_funcStartPublishing(camera_id, json_config_filename.c_str());
}

#endif