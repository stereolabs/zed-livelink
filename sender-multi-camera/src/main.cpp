///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2024, STEREOLABS.
//
// All rights reserved.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////

// OpenGL Viewer(!=0) or no viewer (0)
#define DISPLAY_OGL 1

// ZED include
#include "SenderRunner.hpp"
#include "GLViewer.hpp"
#include "PracticalSocket.h"
#include <sl/Camera.hpp>
#include "Util.h"

nlohmann::json toJSON(int frame_id, int serial_number, sl::Timestamp timestamp, sl::Pose& cam_pose, sl::COORDINATE_SYSTEM coord_sys, sl::UNIT coord_unit);
nlohmann::json toJSON(int frame_id, sl::Timestamp timestamp, sl::Bodies& bodies, int id, sl::BODY_FORMAT body_format, sl::COORDINATE_SYSTEM coord_sys, sl::UNIT coord_unit);

nlohmann::json bodyDataToJson(sl::BodyData body);
void print(string msg_prefix, sl::ERROR_CODE err_code = sl::ERROR_CODE::SUCCESS, string msg_suffix = "");

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

/// ----------------------------------------------------------------------------
/// ----------------------------------------------------------------------------
/// -------------------------------- MAIN LOOP ---------------------------------
/// ----------------------------------------------------------------------------
/// ----------------------------------------------------------------------------

std::vector<sl::CameraIdentifier> cameras;

int main(int argc, char **argv) {

    ZEDFusionConfig zed_fusion_config;
    std::string zed_fusion_config_file("ZEDFusionLiveLinkConfig.json"); // Default name and location.
    if (argc == 2)
    {
        zed_fusion_config_file = argv[1];
        std::cout << "Loading " << zed_fusion_config_file << "fusion config file." << std::endl;
    }
    else if (argc > 2)
    {
        std::cout << "Usage: ./ZED_livelink_fusion.exe [path/to/ZEDFusionLiveLinkConfig.json (optional)]" << std::endl;
        std::cout << "Unexpected arguments, exiting..." << std::endl;
        return EXIT_FAILURE;
    }
    else {
        std::cout << "Trying to load default config file 'ZEDFusionLiveLinkConfig.json' next to executable..." << std::endl;
    }
    readZEDFusionConfig(zed_fusion_config_file, zed_fusion_config);

    std::string json_config_filename = zed_fusion_config.json_config_filename;
    auto configurations = sl::readFusionConfigurationFile(json_config_filename, coord_sys, coord_unit);

    // Check if the ZED camera should run within the same process or if they are running on the edge.
    std::vector<SenderRunner> clients(configurations.size());
    int id_ = 0;
    for (auto conf : configurations) {
        // if the ZED camera should run locally, then start a thread to handle it
        if (conf.communication_parameters.getType() == sl::CommunicationParameters::COMM_TYPE::INTRA_PROCESS) {
            std::cout << "Try to open ZED " << conf.serial_number << ".." << std::flush;
            auto state = clients[id_++].open(conf.input_type, zed_fusion_config.body_format, zed_fusion_config);
            if (state)
                std::cout << ". ready !" << std::endl;
        }
        else
            std::cout << "Fail to open ZED " << conf.serial_number << std::endl;
    }

    // start camera threads
    for (auto& it : clients)
        it.start();

    // Now that the ZED camera are running, we need to initialize the fusion module
    sl::InitFusionParameters init_params;
    init_params.coordinate_units = coord_unit;
    init_params.coordinate_system = coord_sys;
    init_params.verbose = zed_fusion_config.fusion_verbose;

    // create and initialize it
    sl::Fusion fusion;
    fusion.init(init_params);

    // subscribe to every cameras of the setup to internally gather their data
    for (auto& it : configurations) {
        sl::CameraIdentifier uuid(it.serial_number);
        // to subscribe to a camera you must give its serial number, the way to communicate with it (shared memory or local network), and its world pose in the setup.
        auto state = fusion.subscribe(uuid, it.communication_parameters, it.pose);
        if (state != sl::FUSION_ERROR_CODE::SUCCESS)
            std::cout << "Unable to subscribe to " << std::to_string(uuid.sn) << " . " << state << std::endl;
        else
            cameras.push_back(uuid);
    }

    // check that at least one camera is connected
    if (cameras.empty()) {
        std::cout << "no connections " << std::endl;
        return EXIT_FAILURE;
    }

    // as this sample shows how to fuse body detection from the multi camera setup
    // we enable the Body Tracking module with its options
    sl::BodyTrackingFusionParameters body_fusion_init_params;
    body_fusion_init_params.enable_tracking = true;
    body_fusion_init_params.enable_body_fitting = true;
    fusion.enableBodyTracking(body_fusion_init_params);

    // define fusion behavior 
    sl::BodyTrackingFusionRuntimeParameters body_tracking_runtime_parameters;
    // be sure that the detection skeleton is complete enough
    body_tracking_runtime_parameters.skeleton_minimum_allowed_keypoints = zed_fusion_config.fusion_skeleton_minimm_allowed_keypoints;
    body_tracking_runtime_parameters.skeleton_smoothing = zed_fusion_config.fusion_skeleton_smoothing;
    body_tracking_runtime_parameters.skeleton_minimum_allowed_camera = zed_fusion_config.fusion_skeleton_minimum_allowed_camera;

#if DISPLAY_OGL
    GLViewer viewer;
    viewer.init(argc, argv);
#endif

    // fusion outputs
    sl::Bodies fused_bodies;
    std::map<sl::CameraIdentifier, sl::Bodies> camera_raw_data;
    sl::FusionMetrics metrics;

    // ----------------------------------
    // UDP Sending ----------------------
    // ----------------------------------
    std::string servAddress;
    unsigned short servPort;
    UDPSocket sock;

    bool run = true;

    if (zed_fusion_config.connection_type == CONNECTION_TYPE::MULTICAST) sock.setMulticastTTL(1);

    servAddress = zed_fusion_config.udp_ip;
    servPort = zed_fusion_config.udp_port;
    int frame_id = 0;
    std::cout << "Sending fused data at " << servAddress << ":" << servPort << std::endl;

    SetCtrlHandler();
    // run the fusion as long as the viewer is available.
    while (!exit_app)
    {
        // run the fusion process (which gather data from all camera, sync them and process them)
        if (fusion.process() == sl::FUSION_ERROR_CODE::SUCCESS) {
            sl::Timestamp ts = fusion.getCurrentTimeStamp();
            if (zed_fusion_config.send_bodies)
            {
                // Retrieve fused body
                fusion.retrieveBodies(fused_bodies, body_tracking_runtime_parameters);
                // for debug, you can retrieve the data send by each camera
                for (auto& id : cameras)
                    fusion.retrieveBodies(camera_raw_data[id], body_tracking_runtime_parameters, id);
                // get metrics about the fusion process for monitoring purposes
                fusion.getProcessMetrics(metrics);
#if DISPLAY_OGL
                // update the 3D view
                viewer.updateBodies(fused_bodies, camera_raw_data, metrics);
#endif
                if (fused_bodies.is_new)
                {
                    try
                    {
                        // ----------------------------------
                        // UDP Sending ----------------------
                        // ----------------------------------

                        // send body data one at a time instead of as one single packet.
                        for (int i = 0; i < fused_bodies.body_list.size(); i++)
                        {
                            std::string data_to_send = toJSON(frame_id, ts, fused_bodies, i, zed_fusion_config.body_format, coord_sys, coord_unit).dump();
                            sock.sendTo(data_to_send.data(), data_to_send.size(), servAddress, servPort);
                        }
                    }
                    catch (SocketException& e)
                    {

                        cerr << e.what() << endl;
                    }
                }
            }
        }

#if DISPLAY_OGL
        run = viewer.isAvailable();
#endif
        sl::sleep_ms(10);
    }

#if DISPLAY_OGL
    viewer.exit();
#endif

    for (auto& it : clients)
        it.stop();

    fusion.close();

    return EXIT_SUCCESS;
}


/// ----------------------------------------------------------------------------
/// ----------------------------------------------------------------------------
/// ----------------------------- DATA FORMATTING ------------------------------
/// ----------------------------------------------------------------------------
/// ----------------------------------------------------------------------------

// Create the json sent to the clients
nlohmann::json toJSON(int frame_id, int serial_number, sl::Timestamp timestamp, sl::Pose& cam_pose, sl::COORDINATE_SYSTEM coord_sys, sl::UNIT coord_unit)
{
    nlohmann::json j;

    j["serial_number"] = serial_number;
    j["frame_id"] = frame_id;
    j["timestamp"] = timestamp.data_ns;
    j["role"] = ZEDLiveLinkRole::Camera;
    j["coordinate_system"] = coord_sys;
    j["coordinate_unit"] = coord_unit;

    j["camera_position"] = nlohmann::json::object();
    j["camera_position"]["x"] = isnan(cam_pose.getTranslation().x) ? 0 : cam_pose.getTranslation().x;
    j["camera_position"]["y"] = isnan(cam_pose.getTranslation().y) ? 0 : cam_pose.getTranslation().y;
    j["camera_position"]["z"] = isnan(cam_pose.getTranslation().z) ? 0 : cam_pose.getTranslation().z;

    j["camera_orientation"] = nlohmann::json::object();
    j["camera_orientation"]["x"] = isnan(cam_pose.getOrientation().x) ? 0 : cam_pose.getOrientation().x;
    j["camera_orientation"]["y"] = isnan(cam_pose.getOrientation().x) ? 0 : cam_pose.getOrientation().y;
    j["camera_orientation"]["z"] = isnan(cam_pose.getOrientation().x) ? 0 : cam_pose.getOrientation().z;
    j["camera_orientation"]["w"] = isnan(cam_pose.getOrientation().x) ? 0 : cam_pose.getOrientation().w;

    return j;
}


// send one skeleton at a time
nlohmann::json toJSON(int frame_id, sl::Timestamp timestamp, sl::Bodies& bodies, int id, sl::BODY_FORMAT body_format, sl::COORDINATE_SYSTEM coord_sys, sl::UNIT coord_unit)
{
    nlohmann::json j;

    j["frame_id"] = frame_id;
    j["timestamp"] = timestamp.data_ns;
    j["role"] = ZEDLiveLinkRole::Animation;
    j["body_format"] = body_format;
    j["is_new"] = (bool)bodies.is_new;
    j["coordinate_system"] = coord_sys;
    j["coordinate_unit"] = coord_unit;
    j["nb_bodies"] = bodies.body_list.size();

    if (id < bodies.body_list.size())
    {
        auto body = bodies.body_list[id];

        j["tracking_state"] = (int)body.tracking_state;
        j["action_state"] = (int)body.action_state;
        j["id"] = body.id;
        j["position"] = nlohmann::json::object();
        j["position"]["x"] = isnan(body.position.x) ? 0 : body.position.x;
        j["position"]["y"] = isnan(body.position.y) ? 0 : body.position.y;
        j["position"]["z"] = isnan(body.position.z) ? 0 : body.position.z;

        j["confidence"] = isnan(body.confidence) ? 0 : body.confidence;

        j["keypoint_confidence"] = nlohmann::json::array();
        for (auto& i : body.keypoint_confidence)
        {
            j["keypoint_confidence"].push_back(isnan(i) ? 0 : i);
        }

        j["keypoint"] = nlohmann::json::array();
        for (auto& i : body.keypoint)
        {
            nlohmann::json e;
            e["x"] = isnan(i.x) ? 0 : i.x;
            e["y"] = isnan(i.y) ? 0 : i.y;
            e["z"] = isnan(i.z) ? 0 : i.z;
            j["keypoint"].push_back(e);
        }
        j["local_position_per_joint"] = nlohmann::json::array();
        for (auto& i : body.local_position_per_joint)
        {
            nlohmann::json e;
            e["x"] = isnan(i.x) ? 0 : i.x;
            e["y"] = isnan(i.y) ? 0 : i.y;
            e["z"] = isnan(i.z) ? 0 : i.z;
            j["local_position_per_joint"].push_back(e);
        }
        j["local_orientation_per_joint"] = nlohmann::json::array();
        for (auto& i : body.local_orientation_per_joint)
        {
            nlohmann::json e;
            e["x"] = isnan(i.x) ? 0 : i.x;
            e["y"] = isnan(i.y) ? 0 : i.y;
            e["z"] = isnan(i.z) ? 0 : i.z;
            e["w"] = isnan(i.w) ? 0 : i.w;
            j["local_orientation_per_joint"].push_back(e);
        }

        j["global_root_posititon"] = nlohmann::json::object();
        j["global_root_posititon"]["x"] = isnan(body.keypoint[0].x) ? 0 : body.keypoint[0].x;
        j["global_root_posititon"]["y"] = isnan(body.keypoint[0].y) ? 0 : body.keypoint[0].y;
        j["global_root_posititon"]["z"] = isnan(body.keypoint[0].z) ? 0 : body.keypoint[0].z;

        j["global_root_orientation"] = nlohmann::json::object();
        j["global_root_orientation"]["x"] = isnan(body.global_root_orientation.x) ? 0 : body.global_root_orientation.x;
        j["global_root_orientation"]["y"] = isnan(body.global_root_orientation.y) ? 0 : body.global_root_orientation.y;
        j["global_root_orientation"]["z"] = isnan(body.global_root_orientation.z) ? 0 : body.global_root_orientation.z;
        j["global_root_orientation"]["w"] = isnan(body.global_root_orientation.w) ? 0 : body.global_root_orientation.w;
    }

    return j;
}
/// ----------------------------------------------------------------------------
/// ----------------------------------------------------------------------------
/// ----------------------------- MISC & MONO-CAM ------------------------------
/// ----------------------------------------------------------------------------
/// ----------------------------------------------------------------------------


void print(string msg_prefix, sl::ERROR_CODE err_code, string msg_suffix) {
    cout << "[Sample]";
    if (err_code != sl::ERROR_CODE::SUCCESS)
        cout << "[Error]";
    cout << " " << msg_prefix << " ";
    if (err_code != sl::ERROR_CODE::SUCCESS) {
        cout << " | " << toString(err_code) << " : ";
        cout << toVerbose(err_code);
    }
    if (!msg_suffix.empty())
        cout << " " << msg_suffix;
    cout << endl;
}

// If the sender encounter NaN values, it sends 0 instead.
nlohmann::json bodyDataToJson(sl::BodyData body)
{
    nlohmann::json res;

    res["id"] = body.id;
    //res["unique_object_id"] = body.unique_object_id.get();
    res["tracking_state"] = body.tracking_state;
    res["action_state"] = body.action_state;
    res["position"] = nlohmann::json::object();
    res["position"]["x"] = isnan(body.position.x) ? 0 : body.position.x / 1000;
    res["position"]["y"] = isnan(body.position.y) ? 0 : body.position.y / 1000;
    res["position"]["z"] = isnan(body.position.z) ? 0 : body.position.z / 1000;
    res["velocity"] = nlohmann::json::object();
    res["velocity"]["x"] = isnan(body.velocity.x) ? 0 : body.velocity.x / 1000;
    res["velocity"]["y"] = isnan(body.velocity.y) ? 0 : body.velocity.y / 1000;
    res["velocity"]["z"] = isnan(body.velocity.z) ? 0 : body.velocity.z / 1000;
    //res["position_covariance"] = nlohmann::json::array();
    //for (auto& i : body.position_covariance) 
    // {
    //    res["position_covariance"].push_back(i);
    //}
    //res["bounding_box_2d"] = nlohmann::json::array();
    //for (auto& i : body.bounding_box_2d) 
    // {
    //    nlohmann::json e;
    //    e["x"] = i.x;
    //    e["y"] = i.y;
    //    res["bounding_box_2d"].push_back(e);
    //}
    res["confidence"] = isnan(body.confidence) ? 0 : body.confidence;
    res["bounding_box"] = nlohmann::json::array();
    for (auto& i : body.bounding_box) {
        nlohmann::json e;
        e["x"] = isnan(i.x) ? 0 : i.x / 1000;
        e["y"] = isnan(i.y) ? 0 : i.y / 1000;
        e["z"] = isnan(i.z) ? 0 : i.z / 1000;
        res["bounding_box"].push_back(e);
    }
    res["dimensions"] = nlohmann::json::object();
    res["dimensions"]["x"] = isnan(body.dimensions.x) ? 0 : body.dimensions.x / 1000;
    res["dimensions"]["y"] = isnan(body.dimensions.y) ? 0 : body.dimensions.y / 1000;
    res["dimensions"]["z"] = isnan(body.dimensions.z) ? 0 : body.dimensions.z / 1000;
    //res["keypoint_2d"] = nlohmann::json::array();
    //for (auto& i : body.keypoint_2d) 
    // {
    //    nlohmann::json e;
    //    e["x"] = i.x;
    //    e["y"] = i.y;
    //    res["keypoint_2d"].push_back(e);
    //}
    res["keypoint"] = nlohmann::json::array();
    for (auto& i : body.keypoint) {
        nlohmann::json e;
        e["x"] = isnan(i.x) ? 0 : i.x / 1000;
        e["y"] = isnan(i.y) ? 0 : i.y / 1000;
        e["z"] = isnan(i.z) ? 0 : i.z / 1000;
        res["keypoint"].push_back(e);
    }
    std::cout << "id [" << body.id << "] hips: " << body.keypoint[0] << std::endl;
    //res["head_bounding_box_2d"] = nlohmann::json::array();
    //for (auto& i : body.head_bounding_box_2d) 
    // {
    //    nlohmann::json e;
    //    e["x"] = i.x;
    //    e["y"] = i.y;
    //    res["head_bounding_box_2d"].push_back(e);
    //}
    //res["head_bounding_box"] = nlohmann::json::array();
    //for (auto& i : body.head_bounding_box)
    //  {
    //    nlohmann::json e;
    //    e["x"] = i.x;
    //    e["y"] = i.y;
    //    e["z"] = i.z;
    //    res["head_bounding_box"].push_back(e);
    //}
    //res["head_position"] = nlohmann::json::object();
    //res["head_position"]["x"] = body.head_position.x;
    //res["head_position"]["y"] = body.head_position.y;
    //res["head_position"]["z"] = body.head_position.z;
    res["keypoint_confidence"] = nlohmann::json::array();
    for (auto& i : body.keypoint_confidence)
    {
        res["keypoint_confidence"].push_back(isnan(i) ? 0 : i);
    }
    res["local_position_per_joint"] = nlohmann::json::array();
    for (auto& i : body.local_position_per_joint)
    {
        nlohmann::json e;
        e["x"] = isnan(i.x) ? 0 : i.x / 1000;
        e["y"] = isnan(i.y) ? 0 : i.y / 1000;
        e["z"] = isnan(i.z) ? 0 : i.z / 1000;
        res["local_position_per_joint"].push_back(e);
    }
    res["local_orientation_per_joint"] = nlohmann::json::array();
    for (auto& i : body.local_orientation_per_joint)
    {
        nlohmann::json e;
        e["x"] = isnan(i.x) ? 42 : i.x;
        e["y"] = isnan(i.y) ? 42 : i.y;
        e["z"] = isnan(i.z) ? 42 : i.z;
        e["w"] = isnan(i.w) ? 42 : i.w;
        res["local_orientation_per_joint"].push_back(e);
    }
    res["global_root_orientation"] = nlohmann::json::object();
    res["global_root_orientation"]["x"] = isnan(body.global_root_orientation.x) ? 0 : body.global_root_orientation.x;
    res["global_root_orientation"]["y"] = isnan(body.global_root_orientation.y) ? 0 : body.global_root_orientation.y;
    res["global_root_orientation"]["z"] = isnan(body.global_root_orientation.z) ? 0 : body.global_root_orientation.z;
    res["global_root_orientation"]["w"] = isnan(body.global_root_orientation.w) ? 0 : body.global_root_orientation.w;
    return res;
}