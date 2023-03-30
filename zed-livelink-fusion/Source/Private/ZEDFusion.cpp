#ifndef __ZED_FUSION_CPP__
#define __ZED_FUSION_CPP__

#include "ZEDFusion.h"

///////////////////////////////////////
//////////// IMPORT DLL ///////////////
///////////////////////////////////////

ZEDFusion::ZEDFusion() {
	FString name_dll;
	
#if PLATFORM_LINUX
	name_dll = "libsl_zed_c.so";
#elif PLATFORM_WINDOWS
	name_dll = "sl_zed_c.dll";
#endif 
	LoadDll(name_dll);
}

ZEDFusion::~ZEDFusion() {
	UnloadDll();
}

/////////////////////////////////////////////////////////////////////
///////////////////// Import Methods ////////////////////////////////
/////////////////////////////////////////////////////////////////////

bool ZEDFusion::ImportMethod_Init()
{
	if (v_dllHandle != NULL)
	{
		m_funcInit = NULL;
		FString procName = "sl_fusion_init";// Needs to be the exact name of the DLL method.
		m_funcInit = (__InitFusion)FPlatformProcess::GetDllExport(v_dllHandle, *procName);
		if (m_funcInit != NULL)
		{
			return true;
		}
	}
	return false;// Return an error.
}

bool ZEDFusion::ImportMethod_Process()
{
	if (v_dllHandle != NULL)
	{
		m_funcProcess = NULL;
		FString procName = "sl_fusion_process";// Needs to be the exact name of the DLL method.
		m_funcProcess = (__Process)FPlatformProcess::GetDllExport(v_dllHandle, *procName);
		if (m_funcProcess != NULL)
		{
			return true;
		}
	}
	return false;// Return an error.
}
bool ZEDFusion::ImportMethod_Subscribe()
{
	if (v_dllHandle != NULL)
	{
		m_funcSubscribe = NULL;
		FString procName = "sl_fusion_subscribe";// Needs to be the exact name of the DLL method.
		m_funcSubscribe = (__Subscribe)FPlatformProcess::GetDllExport(v_dllHandle, *procName);
		if (m_funcSubscribe != NULL)
		{
			return true;
		}
	}
	return false;// Return an error.
}


bool ZEDFusion::ImportMethod_Close()
{
	if (v_dllHandle != NULL)
	{
		m_funcClose = NULL;
		FString procName = "sl_fusion_close";// Needs to be the exact name of the DLL method.
		m_funcClose = (__CloseFusion)FPlatformProcess::GetDllExport(v_dllHandle, *procName);
		if (m_funcClose != NULL)
		{
			return true;
		}
	}
	return false;// Return an error.
}

bool ZEDFusion::ImportMethod_EnableBodyTracking()
{
	if (v_dllHandle != NULL)
	{
		m_funcEnableBodyTracking = NULL;
		FString procName = "sl_fusion_enable_body_tracking";// Needs to be the exact name of the DLL method.
		m_funcEnableBodyTracking = (__EnableBodyTrackingFusion)FPlatformProcess::GetDllExport(v_dllHandle, *procName);
		if (m_funcEnableBodyTracking != NULL)
		{
			return true;
		}
	}
	return false;// Return an error.
}

bool ZEDFusion::ImportMethod_DisableBodyTracking()
{
	if (v_dllHandle != NULL)
	{
		m_funcDisableBodyTracking = NULL;
		FString procName = "sl_fusion_disable_body_tracking";// Needs to be the exact name of the DLL method.
		m_funcDisableBodyTracking = (__DisableBodyTrackingFusion)FPlatformProcess::GetDllExport(v_dllHandle, *procName);
		if (m_funcDisableBodyTracking != NULL)
		{
			return true;
		}
	}
	return false;// Return an error.
}

bool ZEDFusion::ImportMethod_RetrieveBodies()
{
	if (v_dllHandle != NULL)
	{
		m_funcRetrieveBodies = NULL;
		FString procName = "sl_fusion_retrieve_bodies";// Needs to be the exact name of the DLL method.
		m_funcRetrieveBodies = (__RetrieveBodiesFusion)FPlatformProcess::GetDllExport(v_dllHandle, *procName);
		if (m_funcRetrieveBodies != NULL)
		{
			return true;
		}
	}
	return false;// Return an error.
}

bool ZEDFusion::ImportMethod_GetProcessMetrics()
{
	if (v_dllHandle != NULL)
	{
		m_funcGetProcessMetrics = NULL;
		FString procName = "sl_fusion_get_process_metrics";// Needs to be the exact name of the DLL method.
		m_funcGetProcessMetrics = (__GetProcessMetrics)FPlatformProcess::GetDllExport(v_dllHandle, *procName);
		if (m_funcGetProcessMetrics != NULL)
		{
			return true;
		}
	}
	return false;// Return an error.
}

bool ZEDFusion::ImportMethod_ReadFusionConfigFile()
{
	if (v_dllHandle != NULL)
	{
		m_funcReadFusionConfigFile = NULL;
		FString procName = "sl_fusion_read_configuration_file";// Needs to be the exact name of the DLL method.
		m_funcReadFusionConfigFile = (__ReadFusionConfigFile)FPlatformProcess::GetDllExport(v_dllHandle, *procName);
		if (m_funcReadFusionConfigFile != NULL)
		{
			return true;
		}
	}
	return false;// Return an error.
}



bool ZEDFusion::LoadDll(FString DLLName)
{
	if (FPaths::FileExists(DLLName))
	{
		v_dllHandle = FPlatformProcess::GetDllHandle(*DLLName); // Retrieve the DLL.
		if (v_dllHandle != NULL)
		{
			ImportMethod_Init();
			ImportMethod_Process();
			ImportMethod_Subscribe();
			ImportMethod_Close();
			ImportMethod_EnableBodyTracking();
			ImportMethod_DisableBodyTracking();
			ImportMethod_RetrieveBodies();
			ImportMethod_GetProcessMetrics();
			ImportMethod_ReadFusionConfigFile();
			return true;
		}
	}
	return false;// Return an error.
}

void ZEDFusion::UnloadDll() {
	if (v_dllHandle == NULL)
	{
		return;
	}
	// release dll handle
	FPlatformProcess::FreeDllHandle(v_dllHandle);
	v_dllHandle = NULL;
}

/////////////////////////////////////////////////////////////////////
//////////////////////////// Methods ////////////////////////////////
/////////////////////////////////////////////////////////////////////

sl::FUSION_ERROR_CODE ZEDFusion::Init(SL_InitFusionParameters& params) {
	if (m_funcInit == NULL)
	{
		return sl::FUSION_ERROR_CODE::FAILURE;
	}
	sl::FUSION_ERROR_CODE e = (sl::FUSION_ERROR_CODE)m_funcInit(params);
	return e;
}

sl::FUSION_ERROR_CODE ZEDFusion::Process() {
	if (m_funcProcess == NULL)
	{
		return sl::FUSION_ERROR_CODE::FAILURE;
	}
	sl::FUSION_ERROR_CODE e = (sl::FUSION_ERROR_CODE)m_funcProcess();
	return e;
}

sl::FUSION_ERROR_CODE ZEDFusion::Subscribe(SL_CameraIdentifier& uuid, SL_CommunicationParameters& params, SL_Vector3& pose_translation, SL_Quaternion& pose_rotation) {
	if (m_funcSubscribe == NULL)
	{
		return sl::FUSION_ERROR_CODE::FAILURE;
	}

	sl::FUSION_ERROR_CODE e = (sl::FUSION_ERROR_CODE)m_funcSubscribe(uuid, params, pose_translation, pose_rotation);
	return e;
}


void ZEDFusion::Close() {
	if (m_funcClose == NULL)
	{
		return;
	}
	m_funcClose();
}

sl::FUSION_ERROR_CODE ZEDFusion::EnableBodyTracking(SL_BodyTrackingFusionParameters& params)
{
	if (m_funcEnableBodyTracking == NULL)
	{
		return sl::FUSION_ERROR_CODE::FAILURE;
	}

	sl::FUSION_ERROR_CODE e = (sl::FUSION_ERROR_CODE)m_funcEnableBodyTracking(params);

	return e;
}

void ZEDFusion::DisableBodyTracking() {
	if (m_funcDisableBodyTracking == NULL) {
		return;
	}
	m_funcDisableBodyTracking();
}

sl::FUSION_ERROR_CODE ZEDFusion::RetrieveBodies(SL_Bodies& bodies, SL_BodyTrackingFusionRuntimeParameters& params, SL_CameraIdentifier uuid) {
	if (m_funcRetrieveBodies == NULL)
	{
		return sl::FUSION_ERROR_CODE::FAILURE;
	}
	sl::FUSION_ERROR_CODE e = (sl::FUSION_ERROR_CODE)m_funcRetrieveBodies(bodies, params, uuid);
	return e;
}

sl::FUSION_ERROR_CODE ZEDFusion::GetProcessMetrics(SL_CameraIdentifier& uuid, SL_FusionMetrics& metrics)
{
	if (m_funcGetProcessMetrics == NULL)
	{
		return sl::FUSION_ERROR_CODE::FAILURE;
	}
	sl::FUSION_ERROR_CODE e = (sl::FUSION_ERROR_CODE)m_funcGetProcessMetrics(uuid, metrics);
	return e;
}

void ZEDFusion::ReadFusionConfigFile(std::string jsonConfigFilename, sl::COORDINATE_SYSTEM coord_system, sl::UNIT unit, SL_FusionConfiguration* configs, int& nb_cameras)
{
	if (m_funcReadFusionConfigFile == NULL)
	{
		return;
	}
	char path_char_array[256];
	strcpy(path_char_array, jsonConfigFilename.c_str());
	
	return m_funcReadFusionConfigFile(path_char_array, coord_system, unit, configs, nb_cameras);
}

#endif