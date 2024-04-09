#include "SenderRunner.hpp"

SenderRunner::SenderRunner() : running(false) {
    init_params.coordinate_units = coord_unit;
    init_params.coordinate_system = coord_sys;

}

SenderRunner::~SenderRunner() {
    zed.close();
}

bool SenderRunner::open(sl::InputType input, sl::BODY_FORMAT body_format, ZEDFusionConfig zed_fusion_config) {
    // already running
    if (runner.joinable())
        return false;

    init_params.input = input;

    init_params.depth_mode = zed_fusion_config.depth_mode;
    init_params.camera_fps = zed_fusion_config.fps;
    init_params.camera_resolution = zed_fusion_config.resolution;
    init_params.grab_compute_capping_fps = zed_fusion_config.grab_compute_capping_fps;
    init_params.sdk_verbose = 0;
    init_params.svo_real_time_mode = true;

    auto state = zed.open(init_params);
    if (state != sl::ERROR_CODE::SUCCESS)
    {
        std::cout << "Error: " << state << std::endl;
        return false;
    }

    // in most cases in body tracking setup, the cameras are static
    sl::PositionalTrackingParameters positional_tracking_parameters;
    positional_tracking_parameters.set_as_static = true;
    state = zed.enablePositionalTracking(positional_tracking_parameters);
    if (state != sl::ERROR_CODE::SUCCESS)
    {
        std::cout << "Error: " << state << std::endl;
        return false;
    }

    // define the body tracking parameters, as the fusion can does the tracking and fitting you don't need to enable them here, unless you need it for your app
    sl::BodyTrackingParameters body_tracking_parameters;
    body_tracking_parameters.detection_model = zed_fusion_config.detection_model;
    body_tracking_parameters.body_format = zed_fusion_config.body_format;
    body_tracking_parameters.enable_body_fitting = false;
    body_tracking_parameters.enable_tracking = false;
    state = zed.enableBodyTracking(body_tracking_parameters);
    if (state != sl::ERROR_CODE::SUCCESS)
    {
        std::cout << "Error: " << state << std::endl;
        return false;
    }

    body_rt_params.detection_confidence_threshold = zed_fusion_config.detection_confidence;
    body_rt_params.minimum_keypoints_threshold = zed_fusion_config.minimum_keypoints_threshold;
    body_rt_params.skeleton_smoothing = 0;

    return true;
}


void SenderRunner::start()
{

    if (zed.isOpened()) {
        running = true;
        // the camera should stream its data so the fusion can subscibe to it to gather the detected body and others metadata needed for the process.
        zed.startPublishing();
        // the thread can start to process the camera grab in background
        runner = std::thread(&SenderRunner::work, this);
    }
}

void SenderRunner::stop() 
{
    running = false;
    if (runner.joinable())
        runner.join();
    zed.close();
}

void SenderRunner::work() 
{
    sl::Bodies bodies;

    // in this sample we use a dummy thread to process the ZED data.
    // you can replace it by your own application and use the ZED like you use to, retrieve its images, depth, sensors data and so on.
    // as long as you call the grab function and the retrieveBodies (wich run the detection) the camera will be able to seamlessly transmit the data to the fusion module.
    while (running)
    {
        auto err = zed.grab();
        if (err == sl::ERROR_CODE::SUCCESS)
        {
            // just be sure to run the bodies detection
            zed.retrieveBodies(bodies, body_rt_params);
        }
        else if (err == sl::ERROR_CODE::END_OF_SVOFILE_REACHED)
        {
            zed.setSVOPosition(0);
        }
    }
}
