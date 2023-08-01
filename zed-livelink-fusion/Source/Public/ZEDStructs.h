#ifndef __ZED__STRUCTS_H__
#define __ZED__STRUCTS_H__

THIRD_PARTY_INCLUDES_START
#include <sl/Camera.hpp>
#include <sl/Fusion.hpp>
THIRD_PARTY_INCLUDES_END

#define MAX_NUMBER_OBJECT 75
#define MAX_FUSED_CAMERAS 20

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

	SL_Quaternion(float q_x, float q_y, float q_z, float q_w) {
		x = q_x;
		y = q_y;
		z = q_z;
		w = q_w;
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

	SL_Vector3(float t_x, float t_y, float t_z) {
		x = t_x;
		y = t_y;
		z = t_z;
	}
};

struct SL_Vector4
{
	float x;
	float y;
	float z;
	float w;
};

enum SL_INPUT_TYPE {
	SL_INPUT_TYPE_USB,
	SL_INPUT_TYPE_SVO,
	SL_INPUT_TYPE_STREAM,
	SL_INPUT_TYPE_GMSL
};


struct SL_InitParameters
{
	sl::INPUT_TYPE input_type;
	sl::RESOLUTION camera_resolution;
	int camera_fps;
	int camera_device_id;
	sl::FLIP_MODE camera_image_flip;
	bool camera_disable_self_calib;

	bool enable_right_side_measure;
	bool svo_real_time_mode;
	sl::DEPTH_MODE depth_mode;
	int depth_stabilization;
	float depth_minimum_distance;
	float depth_maximum_distance;

	sl::UNIT coordinate_units;
	sl::COORDINATE_SYSTEM coordinate_system;

	int sdk_gpu_id;
	int sdk_verbose;

	bool sensors_required;
	bool enable_image_enhancement;

	float open_timeout_sec;

	bool async_grab_camera_recovery;

	float grab_compute_capping_fps;

	SL_InitParameters() {
		input_type = sl::INPUT_TYPE::USB;
		camera_resolution = sl::RESOLUTION::HD720;
		camera_fps = 60;
		camera_device_id = 0;
		camera_image_flip = sl::FLIP_MODE::AUTO;
		camera_disable_self_calib = false;
		enable_right_side_measure = false;
		svo_real_time_mode = true;
		depth_mode = sl::DEPTH_MODE::PERFORMANCE;
		depth_stabilization = 1;
		depth_minimum_distance = -1;
		depth_maximum_distance = -1;
		coordinate_units = sl::UNIT::CENTIMETER;
		coordinate_system = sl::COORDINATE_SYSTEM::LEFT_HANDED_Z_UP;
		sdk_gpu_id = -1;
		sdk_verbose = false;
		sensors_required = false;
		enable_image_enhancement = true;
		open_timeout_sec = 5.0f;
		async_grab_camera_recovery = false;
		grab_compute_capping_fps = 0;
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
	sl::REFERENCE_FRAME reference_frame;
	bool enable_depth;
	bool enable_fill_mode;
	int confidence_threshold;
	int texture_confidence_threshold;
	bool remove_saturated_areas;

	SL_RuntimeParameters() {
		reference_frame = sl::REFERENCE_FRAME::CAMERA;
		enable_depth = true;
		enable_fill_mode = false;
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
	/**
	\brief Defines a module instance id. This is used to identify which object detection model instance is used.
	 * If the id is negative it will be auto incremented/generated
	 */
	unsigned int instance_module_id;
	/**
	\brief Defines if the object detection is synchronized to the image or runs in a separate thread.
	If set to true, the detection is run for every grab, otherwise, the thread runs at its own speed, which can lead to new detection once in a while.
	*/
	bool image_sync; // data synchronized
	/**
	\brief Defines if the object detection will track objects across images flow
	 */
	bool enable_tracking;
	/**
	\brief Defines if the mask object will be computed
	 */
	bool enable_segmentation;
	/**
	\brief Enable human pose estimation with skeleton keypoints output (SL_DETECTION_MODEL).
	 */
	sl::OBJECT_DETECTION_MODEL detection_model;
	/**
	\brief Defines a upper depth range for detections.
	  * \n Defined in \ref UNIT set at \ref SL_Camera::open.
	  * \n Default value is set to \ref SL_Initparameters::depth_maximum_distance (can not be higher).
	  */
	float max_range;
	/**
	 \brief Batching system parameters.
	 Batching system (introduced in 3.5) performs short-term re-identification with deep learning and trajectories filtering.
	 * \n BatchParameters::enable need to be true to use this feature (by default disabled)
	 */
	SL_BatchParameters batch_parameters;
	/**
	\brief Defines the filtering mode that should be applied to raw detections.
	*/
	sl::OBJECT_FILTERING_MODE filtering_mode;
	/**
	* @brief When an object is not detected anymore, the SDK will predict its positions during a short period of time before switching its state to SEARCHING.
	* \n It prevents the jittering of the object state when there is a short misdetection. The user can define its own prediction time duration.
	*
	* \note During this time, the object will have OK state even if it is not detected.
	* \note the duration is expressed in seconds
	* \warning the prediction_timeout_s will be clamped to 1 second as the prediction is getting worst with time.
	* \warning set this parameter to 0 to disable SDK predictions
	* defulat : 0.2f
	*/
	float prediction_timeout_s;
	/**
	\brief Allow inference to run at a lower precision to improve runtime and memory usage,
	 * it might increase the initial optimization time and could include downloading calibration data or calibration cache and slightly reduce the accuracy
	 * \note The fp16 is automatically enabled if the GPU is compatible and provides a speed up of almost x2 and reduce memory usage by almost half, no precision loss.
	 * \note This setting allow int8 precision which can speed up by another x2 factor (compared to fp16, or x4 compared to fp32) and half the fp16 memory usage, however some accuracy can be lost.
	 * The accuracy loss should not exceed 1-2% on the compatible models.
	 * The current compatible models are all HUMAN_BODY_XXXX
	 */
	bool allow_reduced_precision_inference;

	SL_ObjectDetectionParameters() {
		image_sync = false;
		enable_tracking = true;
		enable_segmentation = false;
		detection_model = sl::OBJECT_DETECTION_MODEL::MULTI_CLASS_BOX_ACCURATE;
		max_range = -1;
		filtering_mode = sl::OBJECT_FILTERING_MODE::NMS3D;
		prediction_timeout_s = 0.2f;
		allow_reduced_precision_inference = false;
	}
};

struct SL_BodyTrackingParameters
{
	/**
	\brief Defines a module instance id. This is used to identify which object detection model instance is used.
	 * If the id is negative it will be auto incremented/generated
	 */
	unsigned int instance_module_id;

	/**
	\brief Defines if the object detection is synchronized to the image or runs in a separate thread.
	If set to true, the detection is run for every grab, otherwise, the thread runs at its own speed, which can lead to new detection once in a while.
	 */
	bool image_sync;

	/**
	\brief Defines if the object detection will track objects across images flow
	 */
	bool enable_tracking;

	/**
	\brief Defines if the mask object will be computed
	 */
	bool enable_segmentation;

	/**
	\brief Enable human pose estimation with skeleton keypoints output
	 */
	sl::BODY_TRACKING_MODEL detection_model;

	/**
	\brief Defines if the body fitting will be applied
	 */
	bool enable_body_fitting;

	/**
	 * \brief Defines the body format outputed by the sdk when \ref retrieveBodies is called.
	 * \warning if BODY_FORMAT::POSE_34, the ZED SDK will automatically enable the fitting \ref enable_body_fitting
	 *
	 */
	sl::BODY_FORMAT body_format;

	/**
	 * \brief Defines the body selection outputted by the sdk when \ref retrieveBodies is called.
	 */
	sl::BODY_KEYPOINTS_SELECTION body_selection = sl::BODY_KEYPOINTS_SELECTION::FULL;

	/**
   \brief Defines a upper depth range for detections.
	 * \n Defined in \ref UNIT set at \ref sl::Camera::open.
	 * \n Default value is set to \ref sl::Initparameters::depth_maximum_distance (can not be higher).
	 */
	float max_range = -1.f;

#if 0
	/**
	 \brief Batching system parameters.
	 Batching system (introduced in 3.5) performs short-term re-identification with deep learning and trajectories filtering.
	 * \n BatchParameters::enable need to be true to use this feature (by default disabled)
	 */
	SL_BatchParameters batch_parameters;
#endif
	/**
	 * @brief When an object is not detected anymore, the SDK will predict its positions during a short period of time before its state switched to SEARCHING.
	 * \n It prevents the jittering of the object state when there is a short misdetection. The user can define its own prediction time duration.
	 *
	 * \note During this time, the object will have OK state even if it is not detected.
	 * \note the duration is expressed in seconds
	 * \warning the prediction_timeout_s will be clamped to 1 second as the prediction is getting worst with time.
	 * \warning set this parameter to 0 to disable SDK predictions
	 */
	float prediction_timeout_s;

	/**
	\brief Allow inference to run at a lower precision to improve runtime and memory usage,
	 * it might increase the initial optimization time and could include downloading calibration data or calibration cache and slightly reduce the accuracy
	 * \note The fp16 is automatically enabled if the GPU is compatible and provides a speed up of almost x2 and reduce memory usage by almost half, no precision loss.
	 * \note This setting allow int8 precision which can speed up by another x2 factor (compared to fp16, or x4 compared to fp32) and half the fp16 memory usage, however some accuracy can be lost.
	 * The accuracy loss should not exceed 1-2% on the compatible models.
	 * The current compatible models are all HUMAN_BODY_XXXX
	 */
	bool allow_reduced_precision_inference;

	SL_BodyTrackingParameters() {
		instance_module_id = 0;
		image_sync = true;
		enable_tracking = true;
		enable_segmentation = false;
		enable_body_fitting = true;
		detection_model = sl::BODY_TRACKING_MODEL::HUMAN_BODY_MEDIUM;
		max_range = -1;
		body_format = sl::BODY_FORMAT::BODY_38;
		body_selection = sl::BODY_KEYPOINTS_SELECTION::FULL;
		prediction_timeout_s = 0.2f;
		allow_reduced_precision_inference = false;
	}
};

/*
Sets the body tracking runtime parameters.
*/
struct SL_BodyTrackingRuntimeParameters
{
	float detection_confidence_threshold = 20;
	int minimum_keypoints_threshold = -1;
	float skeleton_smoothing = 0.2f;
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
	float depth_min_range = -1.0f;
	bool set_gravity_as_origin = true;
	sl::String area_file_path = "";
	sl::POSITIONAL_TRACKING_MODE mode = sl::POSITIONAL_TRACKING_MODE::STANDARD;
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
	sl::float3 position;
	float confidence;

	//Mask
	int* mask;

	//Image
	sl::float2 bounding_box_2d[4];

	sl::float3 head_position;
	sl::float3 velocity;
	sl::float3 dimensions;
	sl::float3 bounding_box[8];
	sl::float3 head_bounding_box[8];
	sl::float3 head_bounding_box_2d[4];
	float position_covariance[6];
};

/*
Contains data of a detected bodies such as its bounding_box, id and its 3D position.
*/
struct SL_BodyData
{
	int id;
	unsigned char unique_object_id[37];
	sl::OBJECT_TRACKING_STATE tracking_state;
	sl::OBJECT_ACTION_STATE action_state;
	sl::float3 position;
	sl::float3 velocity;
	float position_covariance[6];
	float confidence;
	int* mask;
	sl::float2 bounding_box_2d[4];
	sl::float3 head_position;
	sl::float3 dimensions;
	sl::float3 bounding_box[8];
	sl::float3 head_bounding_box[8];
	sl::float2 head_bounding_box_2d[4];
	sl::float2 keypoint_2d[38];
	sl::float3 keypoint[38];
	float keypoint_confidence[38];
	float keypoint_covariances[38][6];
	sl::float3 local_position_per_joint[38];
	sl::float4 local_orientation_per_joint[38];
	sl::float4 global_root_orientation;
};

/*
Contains the result of the object detection module.
*/
struct SL_Objects
{
	int nb_objects;
	unsigned long long timestamp;
	int is_new;
	int is_tracked;
	sl::OBJECT_DETECTION_MODEL detection_model;
	SL_ObjectData object_list[MAX_NUMBER_OBJECT];
};

struct SL_Bodies
{
	/**
	\brief Number of detected objects. Used to iterate through the body_list array.
	 */
	int nb_bodies;
	/**
	\brief Defines the timestamp corresponding to the frame acquisition.
	 * This value is especially useful for the async mode to synchronize the data.
	 */
	unsigned long long timestamp;
	/**
	\brief Defined if the object list has already been retrieved or not.
	 */
	int is_new;
	/**
	\brief Defined if both the object tracking and the world orientation has been setup.
	 */
	int is_tracked;
	/**
	\brief The list of detected objects
	 */
	SL_BodyData body_list[MAX_NUMBER_OBJECT];
};

struct SL_InputType
{
	sl::INPUT_TYPE input_type = sl::INPUT_TYPE::USB;
	unsigned int serial_number = 0;
	unsigned int id = 0;
	char svo_input_filename[256];
	char stream_input_ip[128];
	unsigned short stream_input_port = 0;
};


///////////////////////////////////////
///////////// Multi Cam ///////////////
///////////////////////////////////////

enum SL_COMM_TYPE
{
	SL_COMM_TYPE_LOCAL_NETWORK, /* the sender and receiver are on the samed local network and communicate by RTP, communication can be affected by the network load.*/
	SL_COMM_TYPE_INTRA_PROCESS /* both sender and receiver are declared by the same process, can be in different threads, this communication is optimized.*/
};

struct  SL_CommunicationParameters
{
	SL_COMM_TYPE communication_type;
	unsigned int ip_port;
	char ip_add[128];
};

struct SL_FusionConfiguration {
	int serial_number;
	struct SL_CommunicationParameters comm_param;
	SL_Vector3 position;
	SL_Quaternion rotation;
	SL_InputType input;
};

struct SL_InitFusionParameters
{
	/**
	This parameter allows you to select the unit to be used for all metric values of the SDK. (depth, point cloud, tracking, mesh, and others).
	\n default : \ref UNIT "UNIT::MILLIMETER"
	 */
	sl::UNIT coordinate_units = sl::UNIT::CENTIMETER;

	/**
	Positional tracking, point clouds and many other features require a given \ref COORDINATE_SYSTEM to be used as reference.
	This parameter allows you to select the \ref COORDINATE_SYSTEM used by the \ref Camera to return its measures.
	\n This defines the order and the direction of the axis of the coordinate system.
	\n default : \ref COORDINATE_SYSTEM "COORDINATE_SYSTEM::IMAGE"
	 */
	sl::COORDINATE_SYSTEM coordinate_system = sl::COORDINATE_SYSTEM::LEFT_HANDED_Z_UP;

	/**
	 * @brief it allows users to extract some stats of the Fusion API like drop frame of each camera, latency, etc
	 *
	 */
	bool output_performance_metrics = false;

	bool verbose = false;

	/**
	 * @brief If specified change the number of period necessary for a source to go in timeout without data. For example, if you set this to 5
	 * then, if any source do not receive data during 5 period, these sources will go to timeout and will be ignored.
	 *
	 */
	unsigned int timeout_period_number = 20;
};

struct SL_RuntimeMultiCameraParameters
{
	bool force_grab_call;
};

struct SL_CameraIdentifier {
	uint64_t sn;

	SL_CameraIdentifier(uint64_t serial_number = 0) : sn(serial_number) {}
};

struct SL_BodyTrackingFusionParameters
{
	//std::string reid_database_file;
	bool enable_tracking = true;
	bool enable_body_fitting = true;
};

struct SL_BodyTrackingFusionRuntimeParameters
{
	/**
	* @brief if the fused skeleton has less than skeleton_minimum_allowed_keypoints keypoints, it will be discarded. Default is -1.
	 *
	 */
	int skeleton_minimum_allowed_keypoints = -1;
	/**
	 * @brief if a skeleton was detected in less than skeleton_minimum_allowed_camera cameras, it will be discarded
	 *
	 */
	int skeleton_minimum_allowed_camera = -1;

	/**
	 * @brief this value controls the smoothing of the tracked or fitted fused skeleton.
	 * it is ranged from 0 (low smoothing) and 1 (high smoothing)
	 */
	float skeleton_smoothing = 0;
};

struct SL_CameraMetrics
{
	struct SL_CameraIdentifier uuid;

	// gives the fps of the received datas
	float received_fps;

	// gives the latency (in second) of the received datas
	float received_latency;

	// gives the latency (in second) after Fusion synchronization
	float synced_latency;

	// if no data present is set to false
	bool is_present;

	// percent of detection par image during the last second in %, a low values means few detections occurs lately
	float ratio_detection;

	// percent of detection par image during the last second in %, a low values means few detections occurs lately
	float delta_ts;
};

struct SL_FusionMetrics {

	// mean number of camera that provides data during the past second
	float mean_camera_fused;

	// mean number of camera that provides data during the past second
	float mean_stdev_between_camera;

	struct SL_CameraMetrics camera_individual_stats[MAX_FUSED_CAMERAS];
};

#endif
