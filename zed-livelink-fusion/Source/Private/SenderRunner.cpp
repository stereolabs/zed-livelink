#include "SenderRunner.hpp"
#include "Utils/Util.h"

SenderRunner::SenderRunner() : running(false) {
    init_params.coordinate_units = UNIT_SYS;
    init_params.coordinate_system = COORD_SYS;

}

SenderRunner::~SenderRunner() {
	zed.Close();
}

bool SenderRunner::open(SL_FusionConfiguration z_input, int cam_id, ZEDFusionConfig zed_fusion_config) {
    // already running
    if (runner.joinable()) return false;

	fusion_config = z_input;

	init_params.depth_mode = zed_fusion_config.sender_depth_mode;
	init_params.camera_fps = zed_fusion_config.sender_fps;;
	init_params.camera_resolution = zed_fusion_config.sender_resolution;
	init_params.svo_real_time_mode = true;
    init_params.input_type = fusion_config.input.input_type;
	std::cout << init_params.input_type << std::endl;
	if (fusion_config.input.input_type == sl::INPUT_TYPE::USB)
		init_params.camera_device_id = cam_id;

	if (!zed.CreateCamera(cam_id))
	{
		std::cout << "Error Create camera" << std::endl;
		return false;
	}

	auto state = zed.Open(init_params, fusion_config.input.serial_number, &fusion_config.input.svo_input_filename[0], &fusion_config.input.stream_input_ip[0], fusion_config.input.stream_input_port);
    if (state != sl::ERROR_CODE::SUCCESS) {
        std::cout << "Error Open Camera " << state << std::endl;
        return false;
    }
	isOpened = true;

    SL_PositionalTrackingParameters ptp;
    ptp.set_as_static = true;
    state = zed.EnablePositionalTracking(ptp);

	if (state != sl::ERROR_CODE::SUCCESS) {
		std::cout << "Error Enable tracking " << state << std::endl;
		return false;
	}

    SL_BodyTrackingParameters btp;
    btp.detection_model = zed_fusion_config.sender_detection_model;
	btp.body_selection = zed_fusion_config.sender_body_selection;
	btp.body_format = zed_fusion_config.sender_body_format;
    btp.enable_body_fitting = false;
    btp.enable_tracking = false;
	btp.image_sync = true;

	confidence_threshold = zed_fusion_config.sender_confidence;
    state = zed.EnableBodyTracking(btp);

    if (state != sl::ERROR_CODE::SUCCESS) {
        std::cout << "Error Enable Body Tracking " << state << std::endl;
        return false;
    }
	
    zed.StartPublishing(fusion_config.comm_param);

    return true;
}

void SenderRunner::start()
{
	if (isOpened)
	{
		runner = std::thread(&SenderRunner::work, this);
	}
}

void SenderRunner::stop() {
    running = false;
    if (runner.joinable()) runner.join();
}

void SenderRunner::work() {
    running = true;

    SL_Bodies local_bodies;
	SL_RuntimeParameters rt_params;
	SL_BodyTrackingRuntimeParameters bt_rt_params;
	bt_rt_params.detection_confidence_threshold = 40;
	bt_rt_params.detection_confidence_threshold = confidence_threshold;
    while (running) {
		auto err = zed.Grab(rt_params);
        if (err == sl::ERROR_CODE::SUCCESS) {
            // run Detection
			zed.RetrieveBodies(bt_rt_params, local_bodies);
        }
		else
		{
			std::cout << "Sender Grab not success : " << err << std::endl;
		}
    }
}