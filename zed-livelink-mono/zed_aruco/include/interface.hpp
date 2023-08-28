#ifndef __INTERFACE_H
#define __INTERFACE_H

#include "aruco.hpp"

#ifdef _WIN32
#define INTERFACE_API __declspec(dllexport)
#else
#define INTERFACE_API
#endif


#ifdef __cplusplus
extern "C" {
#endif
    INTERFACE_API void init(float actual_marker_size_meters, cv::aruco::PREDEFINED_DICTIONARY_NAME dictionary_name, float fx, float fy, float cx, float cy, bool display_image);

    INTERFACE_API void detectMarkers(int width, int height, uchar* img_ptr);

    INTERFACE_API bool getPose(float& t_x, float& t_y, float& t_z, float& q_x, float& q_y, float& q_z, float& q_w);

#ifdef __cplusplus
}
#endif
#endif
