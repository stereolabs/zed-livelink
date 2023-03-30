#ifndef  __SENDER_RUNNER_HDR__
#define __SENDER_RUNNER_HDR__

#include <sl/Fusion.hpp>
#include "ZEDCamera.h"
#include "Utils/Util.h"

#include <thread>

class SenderRunner {

public:
    SenderRunner();
    ~SenderRunner();

    bool open(SL_FusionConfiguration zed_config, int cam_id, ZEDFusionConfig zed_fusion_config);
	void start();
    void stop();

private:
	ZEDCamera zed;
    SL_InitParameters init_params;
    void work();
    std::thread runner;
    bool running;
	SL_FusionConfiguration fusion_config;

	bool isOpened = false;
	float confidence_threshold;
};

#endif // ! __SENDER_RUNNER_HDR__
