#ifndef  __SENDER_RUNNER_HDR__
#define __SENDER_RUNNER_HDR__

#include "Util.h"
#include <sl/Fusion.hpp>

#include <thread>

class SenderRunner {

public:
    SenderRunner();
    ~SenderRunner();

    bool open(sl::InputType, sl::BODY_FORMAT body_format, ZEDFusionConfig zed_fusion_config);
    void start();
    void stop();

private:
    sl::Camera zed;
    sl::InitParameters init_params;
    sl::BodyTrackingRuntimeParameters body_rt_params;
    void work();
    std::thread runner;
    bool running;
};

#endif // ! __SENDER_RUNNER_HDR__
