#ifndef __ZED__STRUCTS_H__
#define __ZED__STRUCTS_H__

#include <sl/Camera.hpp>

#define MAX_NUMBER_OBJECT 75


struct SL_Quaternion {
	float x;
	float y;
	float z;
	float w;

	SL_Quaternion() {
		x = 0;
		y = 0;
		z = 0;
		w = 1;
	}
};

struct SL_Vector2
{
	float x;
	float y;
};

struct SL_Vector3
{
	float x;
	float y;
	float z;

	SL_Vector3() {
		x = 0;
		y = 0;
		z = 0;
	}
};

struct SL_Vector4
{
	float x;
	float y;
	float z;
	float w;
};


struct SL_InitParameters
{
	sl::INPUT_TYPE input_type;
	sl::RESOLUTION resolution;
	int camera_fps;
	int camera_device_id;
	sl::FLIP_MODE camera_image_flip;
	bool camera_disable_self_calib;

	bool enable_right_side_measure;
	bool svo_real_time_mode;
	sl::DEPTH_MODE depth_mode;
	bool depth_stabilization;
	float depth_minimum_distance;
	float depth_maximum_distance;

	sl::UNIT coordinate_unit;
	sl::COORDINATE_SYSTEM coordinate_system;

	int sdk_gpu_id;
	int sdk_verbose;

	bool sensors_required;
	bool enable_image_enhancement;

	float open_timeout_sec;

	SL_InitParameters() {
		input_type = sl::INPUT_TYPE::USB;
		resolution = sl::RESOLUTION::HD720;
		camera_fps = 60;
		camera_device_id = 0;
		camera_image_flip = sl::FLIP_MODE::AUTO;
		camera_disable_self_calib = false;
		enable_right_side_measure = false;
		svo_real_time_mode = true;
		depth_mode = sl::DEPTH_MODE::PERFORMANCE;
		depth_stabilization = true;
		depth_minimum_distance = -1;
		depth_maximum_distance = -1;
		coordinate_unit = sl::UNIT::CENTIMETER;
		coordinate_system = sl::COORDINATE_SYSTEM::LEFT_HANDED_Z_UP;
		sdk_gpu_id = -1;
		sdk_verbose = false;
		sensors_required = false;
		enable_image_enhancement = true;
		open_timeout_sec = 5.0f;
	}
};

/**
* \brief Resolution
*/
struct SL_Resolution {
	long long width;
	long long height;
};

struct SL_CameraParameters {
	float fx; /**< Focal length in pixels along x axis. */
	float fy; /**< Focal length in pixels along y axis. */
	float cx; /**< Optical center along x axis, defined in pixels (usually close to width/2). */
	float cy; /**< Optical center along y axis, defined in pixels (usually close to height/2). */
	double disto[5]; /**< Distortion factor : [ k1, k2, p1, p2, k3 ]. Radial (k1,k2,k3) and Tangential (p1,p2) distortion.*/
	float v_fov; /**< Vertical field of view, in degrees. */
	float h_fov; /**< Horizontal field of view, in degrees.*/
	float d_fov; /**< Diagonal field of view, in degrees.*/
	SL_Resolution image_size; /** size in pixels of the images given by the camera.*/
};

/**
Holds calibration information about the current ZED's hardware, including per-sensor calibration and offsets between the two sensors.
*/
struct SL_CalibrationParameters
{
	/**
	Intrinsic parameters of the left camera
	*/
	struct SL_CameraParameters left_cam;
	/**
	Intrinsic parameters of the right camera
	*/
	struct SL_CameraParameters right_cam;
	/**
	Left to Right camera rotation, expressed in user coordinate system and unit (defined by InitParameters).
	*/
	SL_Vector4 rotation;
	/**
	Left to Right camera translation, expressed in user coordinate system and unit (defined by InitParameters).
	*/
	SL_Vector3 translation;
};

/*
Parameters that define the behavior of the grab.
*/
struct SL_RuntimeParameters
{
	sl::SENSING_MODE sensing_mode;
	sl::REFERENCE_FRAME reference_frame;
	bool enable_depth;
	int confidence_threshold;
	int texture_confidence_threshold;
	bool remove_saturated_areas;

	SL_RuntimeParameters() {
		sensing_mode = sl::SENSING_MODE::STANDARD;
		reference_frame = sl::REFERENCE_FRAME::CAMERA;
		enable_depth = true;
		confidence_threshold = 100;
		texture_confidence_threshold = 100;
		remove_saturated_areas = true;
	}
};

struct SL_BatchParameters
{
	bool enable = false;
	float id_retention_time = 30.f;
	float latency = 2.0f;
};

struct SL_ObjectDetectionParameters
{
	bool image_sync; // data synchronized
	bool enable_tracking;
	bool enable_mask_output;
	sl::DETECTION_MODEL model;
	bool enable_body_fitting;
	sl::BODY_FORMAT body_format;
	float max_range;
	SL_BatchParameters batch_parameters;
	sl::OBJECT_FILTERING_MODE filtering_mode;

	SL_ObjectDetectionParameters() {
		image_sync = false;
		enable_tracking = true;
		enable_mask_output = false;
		enable_body_fitting = false;
		model = sl::DETECTION_MODEL::HUMAN_BODY_ACCURATE;
		max_range = -1;
		body_format = sl::BODY_FORMAT::POSE_34;
		filtering_mode = sl::OBJECT_FILTERING_MODE::NMS3D;
	}
};

/*
Sets the object detection runtime parameters.
*/
struct SL_ObjectDetectionRuntimeParameters
{
	float detection_confidence_threshold;
	int object_class_filter[(int)sl::OBJECT_CLASS::LAST];
	int object_confidence_threshold[(int)sl::OBJECT_CLASS::LAST];

};

struct SL_Pose {
	sl::float3 position;
	sl::float4 rotation;
};

struct SL_PoseData {
	bool valid;
	unsigned long long timestamp;
	SL_Quaternion rotation;
	SL_Vector3 translation;
	int pose_confidence;
	float pose_covariance[36];
	float twist[6];
	float twist_covariance[36];
};

struct SL_PositionalTrackingParameters
{
	SL_Quaternion initial_world_rotation = SL_Quaternion();
	SL_Vector3 initial_world_position = SL_Vector3();
	bool enable_area_memory = true;
	bool enable_pose_smoothing = false;
	bool set_floor_as_origin = false;
	bool set_as_static = false;
	bool enable_imu_fusion = true;
	sl::String area_file_path = "";
};

/*
Contains data of a detected object such as its bounding_box, label, id and its 3D position.
*/
struct SL_ObjectData
{
	int id;
	unsigned char unique_object_id[37];
	int raw_label;
	sl::OBJECT_CLASS label;
	sl::OBJECT_SUBCLASS sublabel;
	sl::OBJECT_TRACKING_STATE tracking_state;
	sl::OBJECT_ACTION_STATE action_state;
	float confidence;

	//Mask
	int* mask;

	//Image
	sl::float2 bounding_box_2d[4];

	sl::float3 position;
	sl::float3 head_position;
	sl::float3 velocity;
	sl::float3 dimensions;
	sl::float3 bounding_box[8];
	sl::float3 head_bounding_box[8];
	sl::float2 keypoint_2d[34];
	sl::float3 keypoint[34];

	float position_covariance[6];
	float keypoint_confidence[34];

	sl::float3 local_position_per_joint[34];
	sl::float4 local_orientation_per_joint[34];
	sl::float4 global_root_orientation;
};

/*
Contains the result of the object detection module.
*/
struct SL_Objects
{
	int nb_object;
	unsigned long long image_ts;
	int is_new;
	int is_tracked;
	sl::DETECTION_MODEL detection_model;
	SL_ObjectData object_list[MAX_NUMBER_OBJECT];
};

struct SL_InputType
{
	sl::INPUT_TYPE input_type = sl::INPUT_TYPE::USB;
	unsigned int serial_number = 0;
	unsigned int id = 0;
	sl::String svo_input_filename = "";
	sl::String stream_input_ip = "";
	unsigned short stream_input_port = 0;
};

#endif
