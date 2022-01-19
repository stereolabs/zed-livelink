#ifndef __UTILS_H__
#define __UTILS_H__

#include "ZEDStructs.h"

#include <sl/Camera.hpp>

bool IsVectorNaN(FVector input)
{
	if (std::isnan(input.X) || std::isnan(input.Y) || std::isnan(input.Z))
		return true;
	else
		return false;
}

bool isNan(SL_ObjectData& objectData) {
	sl::float3 bodyPosition = objectData.keypoint[0]; //objectData.root_world_position;
	sl::float4 bodyRotation = objectData.global_root_orientation;

	if (!std::isfinite(bodyRotation.x) || !std::isfinite(bodyRotation.y) || !std::isfinite(bodyRotation.z) || !std::isfinite(bodyRotation.w)) {
		return true;
	}

	if (!std::isfinite(bodyPosition.x) || !std::isfinite(bodyPosition.y) || !std::isfinite(bodyPosition.z)) {
		return true;
	}

	return false;
}

const TArray<FString> targetBone = {
	"PELVIS",
	"SPINE_NAVAL",
	"SPINE_CHEST",
	"NECK",
	"CLAVICLE_LEFT",
	"SHOULDER_LEFT",
	"ELBOW_LEFT",
	"WRIST_LEFT",
	"HAND_LEFT",
	"HANDTIP_LEFT",
	"THUMB_LEFT",
	"CLAVICLE_RIGHT",
	"SHOULDER_RIGHT",
	"ELBOW_RIGHT",
	"WRIST_RIGHT",
	"HAND_RIGHT",
	"HANDTIP_RIGHT",
	"THUMB_RIGHT",
	"HIP_LEFT",
	"KNEE_LEFT",
	"ANKLE_LEFT",
	"FOOT_LEFT",
	"HIP_RIGHT",
	"KNEE_RIGHT",
	"ANKLE_RIGHT",
	"FOOT_RIGHT",
	"HEAD",
	"NOSE",
	"EYE_LEFT",
	"EAR_LEFT",
	"EYE_RIGHT",
	"EAR_RIGHT"
};

const TArray<int> parentsIdx = {
	-1,
	0,
	1,
	2,
	2,
	4,
	5,
	6,
	7,
	8,
	7,
	2,
	11,
	12,
	13,
	14,
	15,
	14,
	0,
	18,
	19,
	20,
	0,
	22,
	23,
	24,
	3,
	26,
	26,
	26,
	26,
	26
};

#endif
