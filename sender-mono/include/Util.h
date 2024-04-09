#ifndef __UTIL_H__
#define __UTIL_H__

#include "json.hpp"

#include <sl/Camera.hpp>

static bool exit_app = false;

// Handle the CTRL-C keyboard signal
#ifdef _WIN32
#include <Windows.h>

void CtrlHandler(DWORD fdwCtrlType) {
	exit_app = (fdwCtrlType == CTRL_C_EVENT);
}
#else
#include <signal.h>
void nix_exit_handler(int s) {
	exit_app = true;
}
#endif

// Set the function to handle the CTRL-C
void SetCtrlHandler() {
#ifdef _WIN32
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE);
#else // unix
	struct sigaction sigIntHandler;
	sigIntHandler.sa_handler = nix_exit_handler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;
	sigaction(SIGINT, &sigIntHandler, NULL);
#endif
}


enum CONNECTION_TYPE
{
	UNICAST,
	MULTICAST
};

struct ZEDConfig {
	sl::InputType input;
	unsigned int id = 0;
	unsigned int serial_number;
	std::string ip;
	int port;
	std::string svo_path;
	sl::RESOLUTION resolution;
	int fps;
	sl::DEPTH_MODE depth_mode;
	float grab_compute_capping_fps = 0;

	CONNECTION_TYPE connection_type;
	std::string udp_ip;
	unsigned long udp_port;
	bool send_bodies;
	bool send_camera_pose;

	bool enable_pose_smoothing;
	bool enable_area_memory;
	bool set_as_static;
	bool set_floor_as_origin;

	sl::BODY_TRACKING_MODEL detection_model;
	sl::BODY_FORMAT body_format;
	sl::BODY_KEYPOINTS_SELECTION body_selection;
	float detection_confidence;
	float max_range;
	int minimum_keypoints_threshold;
	float skeleton_smoothing;

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
			std::cout << "Error: Invalid Input type. Using default value  " << std::endl;
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
			std::cout << "Body 18 is not compatible, using body34 instead... " << std::endl;
			format = sl::BODY_FORMAT::BODY_34;
		}
		if (value == "BODY_34") {
			format = sl::BODY_FORMAT::BODY_34;
		}
		else if (value == "BODY_38") {
			format = sl::BODY_FORMAT::BODY_38;
		}
#if 0
		else if (value == "BODY_70") {
			format = sl::BODY_FORMAT::BODY_70;
		}
#endif
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
		/*else if (value == "HAND") {
			format = sl::BODY_KEYPOINTS_SELECTION::HAND;
		}*/
		else
		{
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

		processJsonField(injson, "PositionalTrackingParameters", "enable_pose_smoothing", enable_pose_smoothing);
		processJsonField(injson, "PositionalTrackingParameters", "set_as_static", set_as_static);
		processJsonField(injson, "PositionalTrackingParameters", "set_floor_as_origin", set_floor_as_origin);
		processJsonField(injson, "PositionalTrackingParameters", "enable_area_memory", enable_area_memory);

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
			//if (processJsonField(injson, "BodyTrackingParameters", "body_selection", value))
			//{
			//	body_selection = toBodySelection(value);
			//}

			processJsonField(injson, "BodyTrackingParameters", "confidence", detection_confidence);
			processJsonField(injson, "BodyTrackingParameters", "max_range", max_range);

			processJsonField(injson, "BodyTrackingParameters", "minimum_keypoints_threshold", minimum_keypoints_threshold);
			processJsonField(injson, "BodyTrackingParameters", "skeleton_smoothing", skeleton_smoothing);
		}

		sl::InputType::INPUT_TYPE i_type = toInputType(injson["InitParameters"]["input"]);

		switch (i_type) {
			case sl::InputType::INPUT_TYPE::USB_ID: { // INPUT USB id
			int path = injson["InitParameters"]["input_path"].get<int>();
			id = path;
			input.setFromCameraID(id, sl::BUS_TYPE::USB);
			serial_number = 0;
			std::cout << "Input : USB ID : " << id << std::endl;
			}
			break;
			case sl::InputType::INPUT_TYPE::USB_SERIAL: { // INPUT USB serial
				int path = injson["InitParameters"]["input_path"].get<int>();
				serial_number = path;
				input.setFromSerialNumber(serial_number, sl::BUS_TYPE::USB);
				std::cout << "Input : USB Serial" << std::endl;
			}
			case sl::InputType::INPUT_TYPE::GMSL_ID: { // INPUT GMSL id
				int path = injson["InitParameters"]["input_path"].get<int>();
				id = path;
				input.setFromCameraID(id, sl::BUS_TYPE::GMSL);
				serial_number = 0;
				std::cout << "Input : GMSL ID : " << id << std::endl;
			}
												  break;
			case sl::InputType::INPUT_TYPE::GMSL_SERIAL: { // INPUT GMSL serial
				int path = injson["InitParameters"]["input_path"].get<int>();
				serial_number = path;
				input.setFromSerialNumber(serial_number, sl::BUS_TYPE::GMSL);
				std::cout << "Input : GMSL Serial" << std::endl;
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
					input.setFromStream(sl::String(ip_add.c_str()), port);
				}
				else {
					ip = ip_add.c_str();
					input.setFromStream(sl::String(ip_add.c_str()));
				}
			}
			break;
			case sl::InputType::INPUT_TYPE::SVO_FILE: {// INPUT SVO
				std::string path = injson["InitParameters"]["input_path"].get<std::string>();
				svo_path = path.c_str();
				input.setFromSVOFile(sl::String(path.c_str()));
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


#endif
