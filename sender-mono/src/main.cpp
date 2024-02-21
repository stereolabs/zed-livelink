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
#include "GLViewer.hpp"
#include "PracticalSocket.h"
#include "json.hpp"
#include "Util.h"
#include <sl/Camera.hpp>

using namespace sl;

nlohmann::json toJSON(int frame_id, int serial_number, sl::Timestamp timestamp, sl::Pose& cam_pose, sl::COORDINATE_SYSTEM coord_sys, sl::UNIT coord_unit);
nlohmann::json toJSON(int frame_id, sl::Timestamp timestamp, sl::Bodies& bodies, int id, sl::BODY_FORMAT body_format, sl::COORDINATE_SYSTEM coord_sys, sl::UNIT coord_unit);

void print(string msg_prefix, ERROR_CODE err_code = ERROR_CODE::SUCCESS, string msg_suffix = "");

// Type of data send 
enum class ZEDLiveLinkRole
{
    Transform = 0,
    Camera,
    Animation
};

// Defines the Coordinate system and unit used in this sample
static const sl::COORDINATE_SYSTEM coord_sys = sl::COORDINATE_SYSTEM::RIGHT_HANDED_Y_UP;
static const sl::UNIT coord_unit = sl::UNIT::MILLIMETER;

/// ----------------------------------------------------------------------------
/// ----------------------------------------------------------------------------
/// -------------------------------- MAIN LOOP ---------------------------------
/// ----------------------------------------------------------------------------
/// ----------------------------------------------------------------------------

int main(int argc, char **argv) {

    ZEDConfig zed_config;
    std::string zed_config_file("ZEDLiveLinkConfig.json"); // Default name and location.
    if (argc == 2)
    {
        zed_config_file = argv[1];
        std::cout << "Loading " << zed_config_file << " config file.";
    }
    else if (argc > 2)
    {
        std::cout << "Unexecpected arguments, exiting..." << std::endl;
        return EXIT_FAILURE;
    }
    else {
        std::cout << "Trying to load default config file 'ZEDLiveLinkConfig.json' " << std::endl;
    }
    readZEDConfig(zed_config_file, zed_config);
    std::cout << "Starting LiveLink sender" << endl;

    Camera zed;
    InitParameters init_parameters;
    init_parameters.camera_resolution = zed_config.resolution;
    init_parameters.camera_fps = zed_config.fps;
    init_parameters.depth_mode = zed_config.depth_mode;
    init_parameters.coordinate_system = coord_sys;
    init_parameters.coordinate_units = coord_unit;
    init_parameters.grab_compute_capping_fps = zed_config.grab_compute_capping_fps;

    init_parameters.input = zed_config.input;
    init_parameters.svo_real_time_mode = true;

    // Open the camera
    auto returned_state = zed.open(init_parameters);
    if (returned_state != ERROR_CODE::SUCCESS) {
        print("Open Camera", returned_state, "\nExit program.");
        zed.close();
        return EXIT_FAILURE;
    }

    // Enable Positional tracking (mandatory for body tracking) -------------------------------------------------------
    PositionalTrackingParameters positional_tracking_parameters;
    positional_tracking_parameters.set_floor_as_origin = zed_config.set_floor_as_origin;
    // If the camera is static, uncomment the following line to have better performance.
    positional_tracking_parameters.set_as_static = zed_config.set_as_static;
    positional_tracking_parameters.enable_pose_smoothing = zed_config.enable_pose_smoothing;
    positional_tracking_parameters.enable_area_memory = zed_config.enable_area_memory;
   

    returned_state = zed.enablePositionalTracking(positional_tracking_parameters);
    if (returned_state != ERROR_CODE::SUCCESS) {
        print("enable Positional Tracking", returned_state, "\nExit program.");
        zed.close();
        return EXIT_FAILURE;
    }


    BodyTrackingParameters body_tracking_params;
    if (zed_config.send_bodies)
    {
        // Enable the Body tracking module -------------------------------------------------------------------------------------

        body_tracking_params.enable_tracking = true; // track people across grabs
        body_tracking_params.enable_body_fitting = true; // smooth skeletons moves
        body_tracking_params.body_format = zed_config.body_format;
        body_tracking_params.detection_model = zed_config.detection_model;
        body_tracking_params.max_range = zed_config.max_range;
        returned_state = zed.enableBodyTracking(body_tracking_params);
        if (returned_state != ERROR_CODE::SUCCESS) {
            print("enable Body Tracking", returned_state, "\nExit program.");
            zed.close();
            return EXIT_FAILURE;
        }

    }

#if DISPLAY_OGL
    GLViewer viewer;
    viewer.init(argc, argv);
#endif

    Pose cam_pose;
    cam_pose.pose_data.setIdentity();

    // Configure body tracking runtime parameters
    BodyTrackingRuntimeParameters body_tracking_parameters_rt;
    body_tracking_parameters_rt.detection_confidence_threshold = zed_config.detection_confidence;
    body_tracking_parameters_rt.minimum_keypoints_threshold = zed_config.minimum_keypoints_threshold;
    body_tracking_parameters_rt.skeleton_smoothing = zed_config.skeleton_smoothing;

    // Create ZED Bodies filled in the main loop
    Bodies bodies;

    bool run = true;

    // ----------------------------------
    // UDP ------------------------------
    // ----------------------------------
    std::string servAddress;
    unsigned short servPort;
    UDPSocket sock;

    if (zed_config.connection_type == CONNECTION_TYPE::MULTICAST) sock.setMulticastTTL(1);

    servAddress = zed_config.udp_ip;
    servPort = zed_config.udp_port;

    std::cout << "Sending data at " << servAddress << ":" << servPort << std::endl;

    // ----------------------------------
    // UDP ------------------------------
    // ----------------------------------

    RuntimeParameters rt_params = new RuntimeParameters();
    rt_params.measure3D_reference_frame = REFERENCE_FRAME::WORLD;
    int frame_id = 0;

    SetCtrlHandler();
    while (!exit_app)
    {
        auto start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        auto err = zed.grab(rt_params);
        //std::cout << "FPS : " << zed.getCurrentFPS() << std::endl;

        if (err == ERROR_CODE::SUCCESS)
        {
            sl::Timestamp ts = zed.getTimestamp(sl::TIME_REFERENCE::IMAGE);
            if (zed_config.send_bodies)
            {          
                // Retrieve Detected Human Bodies
                zed.retrieveBodies(bodies, body_tracking_parameters_rt);
#if DISPLAY_OGL
                //Update GL View
                viewer.updateData(bodies, cam_pose.pose_data);
#endif

                if (bodies.is_new) 
                {
                    try
                    {
                        // send body data one at a time instead of as one single packet.
                        for (int i = 0; i < bodies.body_list.size(); i++)
                        {
                            std::string data_to_send = toJSON(frame_id, ts, bodies, i, body_tracking_params.body_format, coord_sys, coord_unit).dump();
                            sock.sendTo(data_to_send.data(), data_to_send.size(), servAddress, servPort);
                        }
                    }
                    catch (SocketException& e)
                    {
                        cerr << e.what() << endl;
                        //exit(1);
                    }
                }
            }

            if (zed_config.send_camera_pose)
            {
                zed.getPosition(cam_pose);
                std::string data_to_send = toJSON(frame_id, zed.getCameraInformation().serial_number, ts, cam_pose, coord_sys, coord_unit).dump();
                sock.sendTo(data_to_send.data(), data_to_send.size(), servAddress, servPort);
            }

            frame_id++;
        }
        else if (err == sl::ERROR_CODE::END_OF_SVOFILE_REACHED)
        {
            frame_id = 0;
            zed.setSVOPosition(0);
        }
        else
        {
            print("error grab", returned_state, "\nExit program.");
        }

#if DISPLAY_OGL
        run = viewer.isAvailable();
#endif

       
        auto stop = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        //std::cout << stop - start << " ms" << std::endl;
    }

#if DISPLAY_OGL
    viewer.exit();
#endif

    // Release Bodies
    bodies.body_list.clear();

    // Disable modules
    zed.disableBodyTracking();
    zed.disablePositionalTracking();
    zed.close();

    return EXIT_SUCCESS;
}



/// ----------------------------------------------------------------------------
/// ----------------------------------------------------------------------------
/// ----------------------------- DATA FORMATTING ------------------------------
/// ----------------------------------------------------------------------------
/// ----------------------------------------------------------------------------


void print(string msg_prefix, ERROR_CODE err_code, string msg_suffix) {
    cout << "[Sample]";
    if (err_code != ERROR_CODE::SUCCESS)
        cout << "[Error]";
    cout << " " << msg_prefix << " ";
    if (err_code != ERROR_CODE::SUCCESS) {
        cout << " | " << toString(err_code) << " : ";
        cout << toVerbose(err_code);
    }
    if (!msg_suffix.empty())
        cout << " " << msg_suffix;
    cout << endl;
}

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
