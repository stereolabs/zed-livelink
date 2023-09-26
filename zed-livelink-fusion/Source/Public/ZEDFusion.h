#ifndef __ZED_FUSION_H__
#define __ZED_FUSION_H__

#include "Roles/LiveLinkCameraRole.h"
#include "Roles/LiveLinkCameraTypes.h"
#include "LiveLinkProvider.h"

#include "RequiredProgramMainCPPInclude.h"
#include "Modules/ModuleManager.h"

#include "LiveLinkRefSkeleton.h"
#include "Roles/LiveLinkAnimationRole.h"
#include "Roles/LiveLinkAnimationTypes.h"
#include "ZEDStructs.h"


typedef int(*__InitFusion)(SL_InitFusionParameters& params);
typedef int(*__RetrieveBodiesFusion)(SL_Bodies& bodies, SL_BodyTrackingFusionRuntimeParameters& params, SL_CameraIdentifier uuid);
typedef int(*__EnableBodyTrackingFusion)(SL_BodyTrackingFusionParameters& params);
typedef void(*__DisableBodyTrackingFusion)();
typedef void(*__CloseFusion)();
typedef int(*__Process)();
typedef int(*__Subscribe)(SL_CameraIdentifier& uuid, SL_CommunicationParameters& params, SL_Vector3& pose_translation, SL_Quaternion& pose_rotation);
typedef int(*__GetProcessMetrics)(SL_CameraIdentifier& uuid, SL_FusionMetrics& metrics);
typedef void(*__ReadFusionConfigFile)(const char* json_config_filename, sl::COORDINATE_SYSTEM coord_system, sl::UNIT unit, SL_FusionConfiguration* configs, int& nb_cameras);

class ZEDFusion {
public:
	ZEDFusion();
	~ZEDFusion();

	sl::FUSION_ERROR_CODE Init(SL_InitFusionParameters& params);
	void Close();
	sl::FUSION_ERROR_CODE EnableBodyTracking(SL_BodyTrackingFusionParameters& params);
	void DisableBodyTracking();
	sl::FUSION_ERROR_CODE RetrieveBodies(SL_Bodies& bodies, SL_BodyTrackingFusionRuntimeParameters& params, SL_CameraIdentifier uuid = SL_CameraIdentifier());
	sl::FUSION_ERROR_CODE Process();
	sl::FUSION_ERROR_CODE Subscribe(SL_CameraIdentifier& uuid, SL_CommunicationParameters& params, SL_Vector3& pose_translation, SL_Quaternion& pose_rotation);
	sl::FUSION_ERROR_CODE GetProcessMetrics(SL_CameraIdentifier& uuid, SL_FusionMetrics& stats);
	void ReadFusionConfigFile(std::string jsonConfigFilename, sl::COORDINATE_SYSTEM coord_system, sl::UNIT unit, SL_FusionConfiguration* configs, int& nb_cameras);
	void ReadFusionConfig(std::string fusionConfiguration, sl::COORDINATE_SYSTEM coord_system, sl::UNIT unit, SL_FusionConfiguration* configs, int& nb_cameras);

private:
	void *v_dllHandle;
	__InitFusion m_funcInit;
	__CloseFusion m_funcClose;
	__EnableBodyTrackingFusion m_funcEnableBodyTracking;
	__RetrieveBodiesFusion m_funcRetrieveBodies;
	__DisableBodyTrackingFusion m_funcDisableBodyTracking;
	__Process m_funcProcess;
	__Subscribe m_funcSubscribe;
	__GetProcessMetrics m_funcGetProcessMetrics;
	__ReadFusionConfigFile m_funcReadFusionConfigFile;
	__ReadFusionConfigFile m_funcReadFusionConfig;

	bool ImportMethod_Init();
	bool ImportMethod_Process();
	bool ImportMethod_Subscribe();
	bool ImportMethod_Close();
	bool ImportMethod_EnableBodyTracking();
	bool ImportMethod_DisableBodyTracking();
	bool ImportMethod_RetrieveBodies();
	bool ImportMethod_GetProcessMetrics();
	bool ImportMethod_ReadFusionConfigFile();
	bool ImportMethod_ReadFusionConfig();

	bool LoadDll(FString DLLName);
	void UnloadDll();

};
#endif
