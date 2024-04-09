#ifndef __UTIL_H__
#define __UTIL_H__

#include "json.hpp"
#include <sl/Camera.hpp>

// Defines the Coordinate system and unit used in this sample
static const sl::COORDINATE_SYSTEM coord_sys = sl::COORDINATE_SYSTEM::RIGHT_HANDED_Y_UP;
static const sl::UNIT coord_unit = sl::UNIT::METER;

// Type of data send 
enum class ZEDLiveLinkRole
{
	Transform = 0,
	Camera,
	Animation
};

enum CONNECTION_TYPE
{
	UNICAST,
	MULTICAST
};

struct ZEDFusionConfig {
	sl::RESOLUTION resolution;
	int fps;
	sl::DEPTH_MODE depth_mode;
	float grab_compute_capping_fps = 0;

	CONNECTION_TYPE connection_type;
	std::string udp_ip;
	unsigned long udp_port;
	bool send_bodies;
	bool send_camera_pose;

	sl::BODY_TRACKING_MODEL detection_model;
	sl::BODY_FORMAT body_format;
	sl::BODY_KEYPOINTS_SELECTION body_selection;
	float detection_confidence;
	float max_range;
	int minimum_keypoints_threshold;

	// fusion parameters
	std::string json_config_filename;
	bool fusion_verbose = false;
	float fusion_skeleton_smoothing = 0;
	int fusion_skeleton_minimm_allowed_keypoints = -1;
	int fusion_skeleton_minimum_allowed_camera = -1;

	CONNECTION_TYPE toConnectionType(std::string value)
	{
		CONNECTION_TYPE res;

		if (value == "UNICAST")
		{
			res = CONNECTION_TYPE::UNICAST;
		}
		else if (value == "MULTICAST")
		{
			res = CONNECTION_TYPE::MULTICAST;
		}
		else
		{
			std::cout << "Error: Invalid Connection Type. Using default value  " << std::endl;
			res = CONNECTION_TYPE::UNICAST;
		}
		return res;
	}

	sl::RESOLUTION toResolution(std::string value)
	{
		sl::RESOLUTION res;
		if (value == "HD2K") {
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
			std::cout << "Error: Invalid Resolution. Using default value  " << std::endl;
			res = sl::RESOLUTION::HD720;
		}
		return res;
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
		else if (value == "NEURAL") {
			depth_mode = sl::DEPTH_MODE::NEURAL;
		}
		else if (value == "NONE") {
			depth_mode = sl::DEPTH_MODE::NONE;
		}
		else {
			std::cout << "Error: Invalid Depth mode. Using default value  " << std::endl;
			depth_mode = sl::DEPTH_MODE::ULTRA;
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
		{
			std::cout << "Error: Invalid Body model. Using default value  " << std::endl;
			model = sl::BODY_TRACKING_MODEL::HUMAN_BODY_MEDIUM;
		}

		return model;
	}

	sl::BODY_FORMAT toBodyFormat(std::string value)
	{
		sl::BODY_FORMAT format;

		if (value == "BODY_18") {
			format = sl::BODY_FORMAT::BODY_18;
		}
		if (value == "BODY_34") {
			format = sl::BODY_FORMAT::BODY_34;
		}
		else if (value == "BODY_38") {
			format = sl::BODY_FORMAT::BODY_38;
		}
		else
		{
			std::cout << "Error: Invalid Body Format. Using default value  " << std::endl;
			format = sl::BODY_FORMAT::BODY_34;
		}

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
		else{
			format = sl::BODY_KEYPOINTS_SELECTION::FULL;
		}

		return format;
	}

	/*PREDEFINED_DICTIONARY_NAME toArucoDictionary(std::string value)
{
	PREDEFINED_DICTIONARY_NAME dictionary;

	if (value == "DICT_4X4_50")   dictionary = PREDEFINED_DICTIONARY_NAME::DICT_4X4_50;
	else if (value == "DICT_4X4_100")  dictionary = PREDEFINED_DICTIONARY_NAME::DICT_4X4_100;
	else if (value == "DICT_4X4_250")  dictionary = PREDEFINED_DICTIONARY_NAME::DICT_4X4_250;
	else if (value == "DICT_4X4_1000") dictionary = PREDEFINED_DICTIONARY_NAME::DICT_4X4_1000;
	else if (value == "DICT_5X5_50")   dictionary = PREDEFINED_DICTIONARY_NAME::DICT_5X5_50;
	else if (value == "DICT_5X5_100")  dictionary = PREDEFINED_DICTIONARY_NAME::DICT_5X5_100;
	else if (value == "DICT_5X5_250")  dictionary = PREDEFINED_DICTIONARY_NAME::DICT_5X5_250;
	else if (value == "DICT_5X5_1000")  dictionary = PREDEFINED_DICTIONARY_NAME::DICT_5X5_1000;
	else if (value == "DICT_6X6_50")  dictionary = PREDEFINED_DICTIONARY_NAME::DICT_6X6_50;
	else if (value == "DICT_6X6_100")  dictionary = PREDEFINED_DICTIONARY_NAME::DICT_6X6_100;
	else if (value == "DICT_6X6_250")  dictionary = PREDEFINED_DICTIONARY_NAME::DICT_6X6_250;
	else if (value == "DICT_6X6_1000")  dictionary = PREDEFINED_DICTIONARY_NAME::DICT_6X6_1000;
	else if (value == "DICT_7X7_50")  dictionary = PREDEFINED_DICTIONARY_NAME::DICT_7X7_50;
	else if (value == "DICT_7X7_100")  dictionary = PREDEFINED_DICTIONARY_NAME::DICT_7X7_100;
	else if (value == "DICT_7X7_250")  dictionary = PREDEFINED_DICTIONARY_NAME::DICT_7X7_250;
	else if (value == "DICT_7X7_1000")  dictionary = PREDEFINED_DICTIONARY_NAME::DICT_7X7_1000;
	else if (value == "DICT_ARUCO_ORIGINAL")  dictionary = PREDEFINED_DICTIONARY_NAME::DICT_ARUCO_ORIGINAL;
	else dictionary = PREDEFINED_DICTIONARY_NAME::DICT_6X6_100;

	return dictionary;
}*/

	template <typename T>
	bool processJsonField(const nlohmann::json& injson, const std::string& fieldSectionName, const std::string& fieldName, T& fieldValue)
	{
		if (!injson[fieldSectionName][fieldName].is_null())
		{
			fieldValue = injson[fieldSectionName][fieldName].get<T>();
			std::cout << fieldName << " : " << fieldValue << std::endl;
			return true;
		}
		else
		{
			std::cout << "Error :[" << fieldSectionName << "]" << "][" << fieldName << "] is not found in the config file" << std::endl;
			return false;
		}
	}

	void read(nlohmann::json& injson) {

		std::string value;
		if (processJsonField(injson, "StreamingParameters", "connection_type", value))
		{
			connection_type = toConnectionType(value);
		}

		processJsonField(injson, "StreamingParameters", "ip", udp_ip);
		processJsonField(injson, "StreamingParameters", "port", udp_port);
		processJsonField(injson, "StreamingParameters", "send_camera_pose", send_camera_pose);
		processJsonField(injson, "StreamingParameters", "send_bodies", send_bodies);


		if (processJsonField(injson, "InitParameters", "resolution", value))
		{
			resolution = toResolution(value);
		}

		processJsonField(injson, "InitParameters", "fps", fps);

		if (processJsonField(injson, "InitParameters", "depth_mode", value))
		{
			depth_mode = toDepthMode(value);
		}

		processJsonField(injson, "InitParameters", "grab_compute_capping_fps", grab_compute_capping_fps);

		if (send_bodies)
		{
			if (processJsonField(injson, "BodyTrackingParameters", "detection_model", value))
			{
				detection_model = toDetectionModel(value);
			}
			if (processJsonField(injson, "BodyTrackingParameters", "body_format", value))
			{
				body_format = toBodyFormat(value);
			}

			processJsonField(injson, "BodyTrackingParameters", "confidence", detection_confidence);
			processJsonField(injson, "BodyTrackingParameters", "max_range", max_range);

			processJsonField(injson, "BodyTrackingParameters", "minimum_keypoints_threshold", minimum_keypoints_threshold);
		}

		// fusion 
		processJsonField(injson, "InitFusionParameters", "json_config_filename", json_config_filename);
		processJsonField(injson, "InitFusionParameters", "verbose", fusion_verbose);

		processJsonField(injson, "BodyTrackingFusionParameters", "skeleton_minimm_allowed_keypoints", fusion_skeleton_minimm_allowed_keypoints);
		processJsonField(injson, "BodyTrackingFusionParameters", "skeleton_minimum_allowed_camera", fusion_skeleton_minimum_allowed_camera);
		processJsonField(injson, "BodyTrackingFusionParameters", "skeleton_smoothing", fusion_skeleton_smoothing);
	}
};

inline void readZEDFusionConfig(std::string file, ZEDFusionConfig& zed_config) {
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

#endif
