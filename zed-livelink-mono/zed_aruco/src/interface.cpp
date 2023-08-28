#include "interface.hpp"
#include "aruco_detector.hpp"

#ifdef __cplusplus
extern "C" {
#endif
    INTERFACE_API void init(float actual_marker_size_meters, cv::aruco::PREDEFINED_DICTIONARY_NAME dictionary_name, float fx , float fy, float cx, float cy, bool display_image)
    {
        if (ArucoDetector::get())
        {
            ArucoDetector::get()->init(actual_marker_size_meters, dictionary_name, fx, fy, cx, fy, display_image);
        }
    }

    INTERFACE_API void detectMarkers(int width, int height, uchar* img_ptr)
    {
        if (ArucoDetector::get())
        {
            ArucoDetector::get()->detectMarkers(width, height, img_ptr);
        }
    }

    INTERFACE_API bool getPose(float& t_x, float& t_y, float& t_z, float& q_x, float& q_y, float& q_z, float& q_w)
    {
        if (ArucoDetector::get())
        {
            return ArucoDetector::get()->getPose(t_x, t_y, t_z, q_x, q_y, q_z, q_w);
        }
        else return false;
    }

#ifdef __cplusplus
}
#endif