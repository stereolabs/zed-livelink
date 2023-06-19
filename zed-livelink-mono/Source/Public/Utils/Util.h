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

sl::InputType::INPUT_TYPE toInputType(std::string value)
{
	sl::InputType::INPUT_TYPE input_type;
	if (value == "USB_ID") {
		input_type = sl::InputType::INPUT_TYPE::USB_ID;
	}
	else if (value == "USB_SERIAL") {
		input_type = sl::InputType::INPUT_TYPE::USB_SERIAL;
	}
	else if (value == "SVO") {
		input_type = sl::InputType::INPUT_TYPE::SVO_FILE;
	}
	else if (value == "STREAM") {
		input_type = sl::InputType::INPUT_TYPE::STREAM;
	}
	else if (value == "GMSL_ID") {
		input_type = sl::InputType::INPUT_TYPE::GMSL_ID;
	}
	else if (value == "GMSL_SERIAL") {
		input_type = sl::InputType::INPUT_TYPE::GMSL_SERIAL;
	}
	else {
		input_type = sl::InputType::INPUT_TYPE::USB_ID;
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
	//else if (value == "NEURAL_FAST") {
	//	depth_mode = sl::DEPTH_MODE::NEURAL_FAST;
	//}
	else if (value == "NEURAL") {
		depth_mode = sl::DEPTH_MODE::NEURAL;
	}
	else if (value == "NONE") {
		depth_mode = sl::DEPTH_MODE::NONE;
	}
	else {
		depth_mode = sl::DEPTH_MODE::NONE;
	}

	return depth_mode;
}

sl::BODY_TRACKING_MODEL toDetectionModel(std::string value)
{
	sl::BODY_TRACKING_MODEL model;

	if (value == "HUMAN_BODY_FAST") {
		model = sl::BODY_TRACKING_MODEL::HUMAN_BODY_FAST;
	}
	else if (value == "HUMAN_BODY_MEDIUM") {
		model = sl::BODY_TRACKING_MODEL::HUMAN_BODY_MEDIUM;
	}
	else if (value == "HUMAN_BODY_ACCURATE") {
		model = sl::BODY_TRACKING_MODEL::HUMAN_BODY_ACCURATE;
	}
	else
		model = sl::BODY_TRACKING_MODEL::HUMAN_BODY_MEDIUM;

	return model;
}

sl::BODY_FORMAT toBodyFormat(std::string value)
{
	sl::BODY_FORMAT format;

	if (value == "BODY_18") {
		std::cout << "Body 18 is not compatible, using body34 instead... " << std::endl;
		format = sl::BODY_FORMAT::BODY_34;
	}
	if (value == "BODY_34") {
		format = sl::BODY_FORMAT::BODY_34;
	}
	else if (value == "BODY_38") {
		format = sl::BODY_FORMAT::BODY_38;
	}
	/*else if (value == "BODY_70") {
		format = sl::BODY_FORMAT::BODY_70;
	}*/
	else
		format = sl::BODY_FORMAT::BODY_34;

	return format;
}

sl::BODY_KEYPOINTS_SELECTION toBodySelection(std::string value)
{
	sl::BODY_KEYPOINTS_SELECTION format;

	if (value == "FULL") {
		format = sl::BODY_KEYPOINTS_SELECTION::FULL;
	}
	else if (value == "UPPER_BODY") {
		format = sl::BODY_KEYPOINTS_SELECTION::UPPER_BODY;
	}
	/*else if (value == "HAND") {
		format = sl::BODY_KEYPOINTS_SELECTION::HAND;
	}*/
	else
		format = sl::BODY_KEYPOINTS_SELECTION::FULL;

	return format;
}

struct ZEDConfig {
	sl::INPUT_TYPE input_type;
	unsigned int id;
	unsigned int serial_number;
	std::string ip;
	int port;
	std::string svo_path;
	sl::RESOLUTION resolution;
	int fps;
	sl::DEPTH_MODE depth_mode;
	bool enable_pose_smoothing;
	bool enable_area_memory;
	bool enable_body_tracking_module;
	sl::BODY_TRACKING_MODEL detection_model;
	sl::BODY_FORMAT body_format;
	sl::BODY_KEYPOINTS_SELECTION body_selection;
	float detection_confidence;
	float max_range;
	int minimum_keypoints_threshold;
	int skeleton_smoothing;

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

		enable_body_tracking_module = injson["BodyTrackingParameters"]["enable_module"];
		std::cout << "enable_body_tracking_module : " << enable_body_tracking_module << std::endl;

		detection_model = toDetectionModel(injson["BodyTrackingParameters"]["detection_model"]);
		std::cout << "detection_model : " << detection_model << std::endl;

		body_format = toBodyFormat(injson["BodyTrackingParameters"]["body_format"]);
		std::cout << "body_format : " << body_format << std::endl;

		body_selection = toBodySelection(injson["BodyTrackingParameters"]["body_selection"]);
		std::cout << "body_selection : " << body_selection << std::endl;

		detection_confidence = injson["BodyTrackingParameters"]["confidence"];
		std::cout << "confidence : " << detection_confidence << std::endl;

		max_range = injson["BodyTrackingParameters"]["max_range"];
		std::cout << "max_range : " << max_range << std::endl;

		minimum_keypoints_threshold = injson["BodyTrackingParameters"]["minimum_keypoints_threshold"];
		std::cout << "minimum_keypoints_threshold : " << minimum_keypoints_threshold << std::endl;

		skeleton_smoothing = injson["BodyTrackingParameters"]["skeleton_smoothing"];
		std::cout << "skeleton_smoothing : " << skeleton_smoothing << std::endl;

		sl::InputType::INPUT_TYPE i_type = toInputType(injson["InitParameters"]["input"]);

		switch (i_type) {
			case sl::InputType::INPUT_TYPE::USB_ID: { // INPUT USB id
			input_type = sl::INPUT_TYPE::USB;
			int path = injson["InitParameters"]["input_path"].get<int>();
			id = path;
			serial_number = 0;
			std::cout << "Input : USB ID : " << id << std::endl;
			}
			break;
			case sl::InputType::INPUT_TYPE::USB_SERIAL: { // INPUT USB serial
				input_type = sl::INPUT_TYPE::USB;
				int path = injson["InitParameters"]["input_path"].get<int>();
				serial_number = path;
				std::cout << "Input : USB Serial" << std::endl;
			}
			break;
			case sl::InputType::INPUT_TYPE::STREAM: {// INPUT STREAM
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
			case sl::InputType::INPUT_TYPE::SVO_FILE: {// INPUT SVO
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

const TArray<FString> targetBone38 = {
	"PELVIS",
	"SPINE_1",
	"SPINE_2",
	"SPINE_3",
	"NECK",
	"NOSE",
	"LEFT_EYE",
	"RIGHT_EYE",
	"LEFT_EAR",
	"RIGHT_EAR",
	"LEFT_CLAVICLE",
	"RIGHT_CLAVICLE",
	"LEFT_SHOULDER",
	"RIGHT_SHOULDER",
	"LEFT_ELBOW",
	"RIGHT_ELBOW",
	"LEFT_WRIST",
	"RIGHT_WRIST",
	"LEFT_HIP",
	"RIGHT_HIP",
	"LEFT_KNEE",
	"RIGHT_KNEE",
	"LEFT_ANKLE",
	"RIGHT_ANKLE",
	"LEFT_BIG_TOE",
	"RIGHT_BIG_TOE",
	"LEFT_SMALL_TOE",
	"RIGHT_SMALL_TOE",
	"LEFT_HEEL",
	"RIGHT_HEEL",
	"LEFT_HAND_THUMB_4",
	"RIGHT_HAND_THUMB_4",
	"LEFT_HAND_INDEX_1",
	"RIGHT_HAND_INDEX_1",
	"LEFT_HAND_MIDDLE_4",
	"RIGHT_HAND_MIDDLE_4",
	"LEFT_HAND_PINKY_1",
	"RIGHT_HAND_PINKY_1",
	// Confidences
	"CONF_PELVIS",
	"CONF_SPINE_1",
	"CONF_SPINE_2",
	"CONF_SPINE_3",
	"CONF_NECK",
	"CONF_NOSE",
	"CONF_LEFT_EYE",
	"CONF_RIGHT_EYE",
	"CONF_LEFT_EAR",
	"CONF_RIGHT_EAR",
	"CONF_LEFT_CLAVICLE",
	"CONF_RIGHT_CLAVICLE",
	"CONF_LEFT_SHOULDER",
	"CONF_RIGHT_SHOULDER",
	"CONF_LEFT_ELBOW",
	"CONF_RIGHT_ELBOW",
	"CONF_LEFT_WRIST",
	"CONF_RIGHT_WRIST",
	"CONF_LEFT_HIP",
	"CONF_RIGHT_HIP",
	"CONF_LEFT_KNEE",
	"CONF_RIGHT_KNEE",
	"CONF_LEFT_ANKLE",
	"CONF_RIGHT_ANKLE",
	"CONF_LEFT_BIG_TOE",
	"CONF_RIGHT_BIG_TOE",
	"CONF_LEFT_SMALL_TOE",
	"CONF_RIGHT_SMALL_TOE",
	"CONF_LEFT_HEEL",
	"CONF_RIGHT_HEEL",
	"CONF_LEFT_HAND_THUMB_4",
	"CONF_RIGHT_HAND_THUMB_4",
	"CONF_LEFT_HAND_INDEX_1",
	"CONF_RIGHT_HAND_INDEX_1",
	"CONF_LEFT_HAND_MIDDLE_4",
	"CONF_RIGHT_HAND_MIDDLE_4",
	"CONF_LEFT_HAND_PINKY_1",
	"CONF_RIGHT_HAND_PINKY_1"
};

const TArray<int> parents38Idx = {
	-1,
	0,
	1,
	2,
	3,
	4,
	4,
	4,
	4,
	4,
	3,
	3,
	10,
	11,
	12,
	13,
	14,
	15,
	0,
	0,
	18,
	19,
	20,
	21,
	22,
	23,
	22,
	23,
	22,
	23,
	16,
	17,
	16,
	17,
	16,
	17,
	16,
	17,
	// confidences
	-1,
	0,
	1,
	2,
	3,
	4,
	4,
	4,
	4,
	4,
	3,
	3,
	10,
	11,
	12,
	13,
	14,
	15,
	0,
	0,
	18,
	19,
	20,
	21,
	22,
	23,
	22,
	23,
	22,
	23,
	16,
	17,
	16,
	17,
	16,
	17,
	16,
	17
};

const TArray<FString> targetBone70 = {
	"PELVIS",
	"SPINE_1",
	"SPINE_2",
	"SPINE_3",
	"NECK",
	"NOSE",
	"LEFT_EYE",
	"RIGHT_EYE",
	"LEFT_EAR",
	"RIGHT_EAR",
	"LEFT_CLAVICLE",
	"RIGHT_CLAVICLE",
	"LEFT_SHOULDER",
	"RIGHT_SHOULDER",
	"LEFT_ELBOW",
	"RIGHT_ELBOW",
	"LEFT_WRIST",
	"RIGHT_WRIST",
	"LEFT_HIP",
	"RIGHT_HIP",
	"LEFT_KNEE",
	"RIGHT_KNEE",
	"LEFT_ANKLE",
	"RIGHT_ANKLE",
	"LEFT_BIG_TOE",
	"RIGHT_BIG_TOE",
	"LEFT_SMALL_TOE",
	"RIGHT_SMALL_TOE",
	"LEFT_HEEL",
	"RIGHT_HEEL",
	"LEFT_HAND_THUMB_1",
	"LEFT_HAND_THUMB_2",
	"LEFT_HAND_THUMB_3",
	"LEFT_HAND_THUMB_4",
	"LEFT_HAND_INDEX_1",
	"LEFT_HAND_INDEX_2",
	"LEFT_HAND_INDEX_3",
	"LEFT_HAND_INDEX_4",
	"LEFT_HAND_MIDDLE_1",
	"LEFT_HAND_MIDDLE_2",
	"LEFT_HAND_MIDDLE_3",
	"LEFT_HAND_MIDDLE_4",
	"LEFT_HAND_RING_1",
	"LEFT_HAND_RING_2",
	"LEFT_HAND_RING_3",
	"LEFT_HAND_RING_4",
	"LEFT_HAND_PINKY_1",
	"LEFT_HAND_PINKY_2",
	"LEFT_HAND_PINKY_3",
	"LEFT_HAND_PINKY_4",
	"RIGHT_HAND_THUMB_1",
	"RIGHT_HAND_THUMB_2",
	"RIGHT_HAND_THUMB_3",
	"RIGHT_HAND_THUMB_4",
	"RIGHT_HAND_INDEX_1",
	"RIGHT_HAND_INDEX_2",
	"RIGHT_HAND_INDEX_3",
	"RIGHT_HAND_INDEX_4",
	"RIGHT_HAND_MIDDLE_1",
	"RIGHT_HAND_MIDDLE_2",
	"RIGHT_HAND_MIDDLE_3",
	"RIGHT_HAND_MIDDLE_4",
	"RIGHT_HAND_RING_1",
	"RIGHT_HAND_RING_2",
	"RIGHT_HAND_RING_3",
	"RIGHT_HAND_RING_4",
	"RIGHT_HAND_PINKY_1",
	"RIGHT_HAND_PINKY_2",
	"RIGHT_HAND_PINKY_3",
	"RIGHT_HAND_PINKY_4",
	"CONF_PELVIS",
	"CONF_SPINE_1",
	"CONF_SPINE_2",
	"CONF_SPINE_3",
	"CONF_NECK",
	"CONF_NOSE",
	"CONF_LEFT_EYE",
	"CONF_RIGHT_EYE",
	"CONF_LEFT_EAR",
	"CONF_RIGHT_EAR",
	"CONF_LEFT_CLAVICLE",
	"CONF_RIGHT_CLAVICLE",
	"CONF_LEFT_SHOULDER",
	"CONF_RIGHT_SHOULDER",
	"CONF_LEFT_ELBOW",
	"CONF_RIGHT_ELBOW",
	"CONF_LEFT_WRIST",
	"CONF_RIGHT_WRIST",
	"CONF_LEFT_HIP",
	"CONF_RIGHT_HIP",
	"CONF_LEFT_KNEE",
	"CONF_RIGHT_KNEE",
	"CONF_LEFT_ANKLE",
	"CONF_RIGHT_ANKLE",
	"CONF_LEFT_BIG_TOE",
	"CONF_RIGHT_BIG_TOE",
	"CONF_LEFT_SMALL_TOE",
	"CONF_RIGHT_SMALL_TOE",
	"CONF_LEFT_HEEL",
	"CONF_RIGHT_HEEL",
	"CONF_LEFT_HAND_THUMB_1",
	"CONF_LEFT_HAND_THUMB_2",
	"CONF_LEFT_HAND_THUMB_3",
	"CONF_LEFT_HAND_THUMB_4",
	"CONF_LEFT_HAND_INDEX_1",
	"CONF_LEFT_HAND_INDEX_2",
	"CONF_LEFT_HAND_INDEX_3",
	"CONF_LEFT_HAND_INDEX_4",
	"CONF_LEFT_HAND_MIDDLE_1",
	"CONF_LEFT_HAND_MIDDLE_2",
	"CONF_LEFT_HAND_MIDDLE_3",
	"CONF_LEFT_HAND_MIDDLE_4",
	"CONF_LEFT_HAND_RING_1",
	"CONF_LEFT_HAND_RING_2",
	"CONF_LEFT_HAND_RING_3",
	"CONF_LEFT_HAND_RING_4",
	"CONF_LEFT_HAND_PINKY_1",
	"CONF_LEFT_HAND_PINKY_2",
	"CONF_LEFT_HAND_PINKY_3",
	"CONF_LEFT_HAND_PINKY_4",
	"CONF_RIGHT_HAND_THUMB_1",
	"CONF_RIGHT_HAND_THUMB_2",
	"CONF_RIGHT_HAND_THUMB_3",
	"CONF_RIGHT_HAND_THUMB_4",
	"CONF_RIGHT_HAND_INDEX_1",
	"CONF_RIGHT_HAND_INDEX_2",
	"CONF_RIGHT_HAND_INDEX_3",
	"CONF_RIGHT_HAND_INDEX_4",
	"CONF_RIGHT_HAND_MIDDLE_1",
	"CONF_RIGHT_HAND_MIDDLE_2",
	"CONF_RIGHT_HAND_MIDDLE_3",
	"CONF_RIGHT_HAND_MIDDLE_4",
	"CONF_RIGHT_HAND_RING_1",
	"CONF_RIGHT_HAND_RING_2",
	"CONF_RIGHT_HAND_RING_3",
	"CONF_RIGHT_HAND_RING_4",
	"CONF_RIGHT_HAND_PINKY_1",
	"CONF_RIGHT_HAND_PINKY_2",
	"CONF_RIGHT_HAND_PINKY_3",
	"CONF_RIGHT_HAND_PINKY_4"
};

const TArray<int> parents70Idx = {
	-1,
	0,
	1,
	2,
	3,
	4,
	4,
	4,
	4,
	4,
	3,
	3,
	10,
	11,
	12,
	13,
	14,
	15,
	0,
	0,
	18,
	19,
	20,
	21,
	22,
	23,
	22,
	23,
	22,
	23,
	16,
	30,
	31,
	32,
	16,
	34,
	35,
	36,
	16,
	38,
	39,
	40,
	16,
	42,
	43,
	44,
	16,
	46,
	47,
	48,
	17,
	50,
	51,
	52,
	17,
	50,
	51,
	52,
	17,
	50,
	51,
	52,
	17,
	50,
	51,
	52,
	17,
	50,
	51,
	52,
	//confidences
	-1,
	0,
	1,
	2,
	3,
	4,
	4,
	4,
	4,
	4,
	3,
	3,
	10,
	11,
	12,
	13,
	14,
	15,
	0,
	0,
	18,
	19,
	20,
	21,
	22,
	23,
	22,
	23,
	22,
	23,
	16,
	30,
	31,
	32,
	16,
	34,
	35,
	36,
	16,
	38,
	39,
	40,
	16,
	42,
	43,
	44,
	16,
	46,
	47,
	48,
	17,
	50,
	51,
	52,
	17,
	50,
	51,
	52,
	17,
	50,
	51,
	52,
	17,
	50,
	51,
	52,
	17,
	50,
	51,
	52,
};

const TArray<FString> targetBone34 = {
	"PELVIS",
	"NAVAL_SPINE",
	"CHEST_SPINE",
	"NECK",
	"LEFT_CLAVICLE",
	"LEFT_SHOULDER",
	"LEFT_ELBOW",
	"LEFT_WRIST",
	"LEFT_HAND",
	"LEFT_HANDTIP",
	"LEFT_THUMB",
	"RIGHT_CLAVICLE",
	"RIGHT_SHOULDER",
	"RIGHT_ELBOW",
	"RIGHT_WRIST",
	"RIGHT_HAND",
	"RIGHT_HANDTIP",
	"RIGHT_THUMB",
	"LEFT_HIP",
	"LEFT_KNEE",
	"LEFT_ANKLE",
	"LEFT_FOOT",
	"RIGHT_HIP",
	"RIGHT_KNEE",
	"RIGHT_ANKLE",
	"RIGHT_FOOT",
	"HEAD",
	"NOSE",
	"LEFT_EYE",
	"LEFT_EAR",
	"RIGHT_EYE",
	"RIGHT_EAR",
	"LEFT_HEEL",
	"RIGHT_HEEL",
	"CONF_PELVIS",
	"CONF_NAVAL_SPINE",
	"CONF_CHEST_SPINE",
	"CONF_NECK",
	"CONF_LEFT_CLAVICLE",
	"CONF_LEFT_SHOULDER",
	"CONF_LEFT_ELBOW",
	"CONF_LEFT_WRIST",
	"CONF_LEFT_HAND",
	"CONF_LEFT_HANDTIP",
	"CONF_LEFT_THUMB",
	"CONF_RIGHT_CLAVICLE",
	"CONF_RIGHT_SHOULDER",
	"CONF_RIGHT_ELBOW",
	"CONF_RIGHT_WRIST",
	"CONF_RIGHT_HAND",
	"CONF_RIGHT_HANDTIP",
	"CONF_RIGHT_THUMB",
	"CONF_LEFT_HIP",
	"CONF_LEFT_KNEE",
	"CONF_LEFT_ANKLE",
	"CONF_LEFT_FOOT",
	"CONF_RIGHT_HIP",
	"CONF_RIGHT_KNEE",
	"CONF_RIGHT_ANKLE",
	"CONF_RIGHT_FOOT",
	"CONF_HEAD",
	"CONF_NOSE",
	"CONF_LEFT_EYE",
	"CONF_LEFT_EAR",
	"CONF_RIGHT_EYE",
	"CONF_RIGHT_EAR",
	"CONF_LEFT_HEEL",
	"CONF_RIGHT_HEEL"
};

static TArray<int> parents34Idx = TArray<int>{
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
		//confidences
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
