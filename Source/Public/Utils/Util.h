#ifndef __UTILS_H__
#define __UTILS_H__

#include "ZEDStructs.h"
#include "Utils/json.hpp"

#include <sl/Camera.hpp>


sl::RESOLUTION toResolution(std::string value)
{
	sl::RESOLUTION res;
	if (value == "HD2K"){
		res = sl::RESOLUTION::HD2K;
	}
	else if (value == "HD1080") {
		res = sl::RESOLUTION::HD1080;
	}
	else if (value == "HD720") {
		res = sl::RESOLUTION::HD720;
	}
	else if (value == "VGA")
	{
		res = sl::RESOLUTION::VGA;
	}
	else {
		res = sl::RESOLUTION::HD720;
	}
return res;
}

sl::INPUT_TYPE toInputType(std::string value)
{
	sl::INPUT_TYPE input_type;
	if (value == "USB") {
		input_type = sl::INPUT_TYPE::USB;
	}
	else if (value == "SVO") {
		input_type = sl::INPUT_TYPE::SVO;
	}
	else if (value == "STREAM") {
		input_type = sl::INPUT_TYPE::STREAM;
	}
	else {
		input_type = sl::INPUT_TYPE::USB;
	}
	return input_type;
}

sl::DEPTH_MODE toDepthMode(std::string value)
{
	sl::DEPTH_MODE depth_mode;

	if (value == "PERFORMANCE") {
		depth_mode = sl::DEPTH_MODE::PERFORMANCE;
	}
	else if (value == "ULTRA") {
		depth_mode = sl::DEPTH_MODE::ULTRA;
	}
	else if (value == "QUALITY") {
		depth_mode = sl::DEPTH_MODE::QUALITY;
	}
	else if (value == "NEURAL") {
		depth_mode = sl::DEPTH_MODE::NEURAL;
	}
	else if (value == "NONE") {
		depth_mode = sl::DEPTH_MODE::NONE;
	}

	return depth_mode;
}

sl::DETECTION_MODEL toDetectionModel(std::string value)
{
	sl::DETECTION_MODEL model;

	if (value == "HUMAN_BODY_FAST") {
		model = sl::DETECTION_MODEL::HUMAN_BODY_FAST;
	}
	else if (value == "HUMAN_BODY_MEDIUM") {
		model = sl::DETECTION_MODEL::HUMAN_BODY_MEDIUM;
	}
	else if (value == "HUMAN_BODY_ACCURATE") {
		model = sl::DETECTION_MODEL::HUMAN_BODY_ACCURATE;
	}
	else
		model = sl::DETECTION_MODEL::HUMAN_BODY_ACCURATE;

	return model;
}

struct ZEDConfig {
	sl::INPUT_TYPE input_type;
	std::string ip;
	int port;
	std::string svo_path;
	sl::RESOLUTION resolution;
	int fps;
	sl::DEPTH_MODE depth_mode;
	bool enable_pose_smoothing;
	bool enable_area_memory;
	bool enable_object_detection_module;
	sl::DETECTION_MODEL detection_model;
	float confidence;
	float max_range;
	int minimum_keypoints_threshold;

	void read(nlohmann::json& injson) {

		std::cout << "Loading config file ... " << std::endl;
		resolution = toResolution(injson["InitParameters"]["resolution"]);
		std::cout << "resolution : " << resolution << std::endl;
		 
		fps = injson["InitParameters"]["fps"];
		std::cout << "fps : " << fps << std::endl;

		depth_mode = toDepthMode(injson["InitParameters"]["depth_mode"]);
		std::cout << "depth_mode : " << depth_mode << std::endl;

		enable_pose_smoothing = injson["PositionalTrackingParameters"]["enable_pose_smoothing"];
		std::cout << "enable_pose_smoothing : " << enable_pose_smoothing << std::endl;

		enable_area_memory = injson["PositionalTrackingParameters"]["enable_area_memory"];
		std::cout << "enable_area_memory : " << enable_area_memory << std::endl;

		enable_object_detection_module = injson["ObjectDetectionParameters"]["enable_module"];
		std::cout << "enable_object_detection_module : " << enable_object_detection_module << std::endl;

		detection_model = toDetectionModel(injson["ObjectDetectionParameters"]["detection_model"]);
		std::cout << "detection_model : " << detection_model << std::endl;

		confidence = injson["ObjectDetectionParameters"]["confidence"];
		std::cout << "confidence : " << confidence << std::endl;

		max_range = injson["ObjectDetectionParameters"]["max_range"];
		std::cout << "max_range : " << max_range << std::endl;

		minimum_keypoints_threshold = injson["ObjectDetectionParameters"]["minimum_keypoints_threshold"];
		std::cout << "minimum_keypoints_threshold : " << minimum_keypoints_threshold << std::endl;

		sl::INPUT_TYPE i_type = toInputType(injson["InitParameters"]["input"]);

		switch (i_type) {
		case sl::INPUT_TYPE::USB: { // INPUT USB
			input_type = sl::INPUT_TYPE::USB;
			std::cout << "Input : USB" << std::endl;
		}
		break;
		case sl::INPUT_TYPE::STREAM: {// INPUT STREAM
			std::string ip_add = injson["InitParameters"]["input_path"].get<std::string>();
			std::size_t found_port = ip_add.find_last_of(":");
			std::cout << "Input : Stream "<< ip_add << std::endl;

			if (found_port != std::string::npos) {
				std::string IP_add = ip_add.substr(0, found_port);
				unsigned short p = std::atoi(ip_add.substr(found_port + 1).c_str());
				port = p;
			}
			else {
				input_type = sl::INPUT_TYPE::STREAM;
				ip = ip_add.c_str();
			}
		}
		break;
		case sl::INPUT_TYPE::SVO: {// INPUT SVO
			std::string path = injson["InitParameters"]["input_path"].get<std::string>();
			input_type = sl::INPUT_TYPE::SVO;
			svo_path = path.c_str();
			std::cout << "Input : SVO " << svo_path << std::endl;

		}
		break;
		}
	}
};

inline void readZEDConfig(std::string file, ZEDConfig& zed_config) {
	nlohmann::json json_file;
	std::ifstream in(file);
	if (in.is_open()) {
		in >> json_file;
		in.close();
		zed_config.read(json_file);
	}
	else {
		std::cout << "Unable to read json config file." << std::endl;
	}
}

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
	"HEEL_LEFT",
	"HEEL_RIGHT",
	"CONF_PELVIS",
	"CONF_SPINE_NAVAL",
	"CONF_CONF_SPINE_CHEST",
	"CONF_NECK",
	"CONF_CLAVICLE_LEFT",
	"CONF_SHOULDER_LEFT",
	"CONF_ELBOW_LEFT",
	"CONF_WRIST_LEFT",
	"CONF_HAND_LEFT",
	"CONF_HANDTIP_LEFT",
	"CONF_THUMB_LEFT",
	"CONF_CLAVICLE_RIGHT",
	"CONF_SHOULDER_RIGHT",
	"CONF_ELBOW_RIGHT",
	"CONF_WRIST_RIGHT",
	"CONF_HAND_RIGHT",
	"CONF_HANDTIP_RIGHT",
	"CONF_THUMB_RIGHT",
	"CONF_HIP_LEFT",
	"CONF_KNEE_LEFT",
	"CONF_ANKLE_LEFT",
	"CONF_FOOT_LEFT",
	"CONF_HIP_RIGHT",
	"CONF_KNEE_RIGHT",
	"CONF_ANKLE_RIGHT",
	"CONF_FOOT_RIGHT",
	"CONF_HEAD",
	"CONF_NOSE",
	"CONF_EYE_LEFT",
	"CONF_EAR_LEFT",
	"CONF_EYE_RIGHT",
	"CONF_EAR_RIGHT",
	"CONF_HEEL_LEFT",
	"CONF_HEEL_RIGHT"
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
	26,
	20,
	24,
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
	26,
	20,
	24
};

#endif
