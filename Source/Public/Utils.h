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
	"EAR_RIGHT",
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

const int
// JointType
JointType_Head = 0,
JointType_Neck = 1,
JointType_ShoulderRight = 2,
JointType_ElbowRight = 3,
JointType_WristRight = 4,
JointType_ShoulderLeft = 5,
JointType_ElbowLeft = 6,
JointType_WristLeft = 7,
JointType_HipRight = 8,
JointType_KneeRight = 9,
JointType_AnkleRight = 10,
JointType_HipLeft = 11,
JointType_KneeLeft = 12,
JointType_AnkleLeft = 13,
JointType_EyesRight = 14,
JointType_EyesLeft = 15,
JointType_HearRight = 16,
JointType_HearLeft = 17,
JointType_Pelvis = 18,
JointType_Spine_01 = 19,
JointType_Spine_02 = 20,
JointType_Spine_03 = 21,
JointType_ClavicleLeft = 22,
JointType_ClavicleRight = 23,
JointType_Nose = 24,
jointCount = 25;


const TArray<int> jointSegment = {
	JointType_Pelvis, JointType_Spine_01,			     // Pelvis
	JointType_Spine_01, JointType_Spine_02,				 // Spine_01
	JointType_Spine_02, JointType_Spine_03,				 // Spine_02
	JointType_Spine_03, JointType_Neck,					 // Spine_03
	JointType_Neck, JointType_Head,                      // Neck
	// left
	JointType_ClavicleLeft, JointType_ShoulderLeft,      // LeftClavicle
	JointType_ShoulderLeft, JointType_ElbowLeft,         // LeftUpperArm
	JointType_ElbowLeft, JointType_WristLeft,            // LeftLowerArm
	JointType_HipLeft, JointType_KneeLeft,               // LeftUpperLeg
	JointType_KneeLeft, JointType_AnkleLeft,             // LeftLowerLeg
	// right
	JointType_ClavicleRight, JointType_ShoulderRight,    // RightClavicle
	JointType_ShoulderRight, JointType_ElbowRight,       // RightUpperArm
	JointType_ElbowRight, JointType_WristRight,          // RightLowerArm
	JointType_HipRight, JointType_KneeRight,             // RightUpperLeg
	JointType_KneeRight, JointType_AnkleRight,           // RightLowerLeg
};


#endif
