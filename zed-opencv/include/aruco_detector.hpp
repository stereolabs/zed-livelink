#ifndef __ARUCO_DETECTOR_H
#define __ARUCO_DETECTOR_H

#include "aruco.hpp"
#include <sl/Camera.hpp>


class ArucoDetector
{
private:
	ArucoDetector();

    ~ArucoDetector()
    {

    }

    static ArucoDetector* instance;

    cv::aruco::Dictionary dictionary;

    float actual_marker_size_meters_;
    std::vector<cv::Vec3d> rvecs, tvecs;
    std::vector<int> ids;
    std::vector<std::vector<cv::Point2f> > corners;
    cv::Matx33d camera_matrix;
    cv::Matx<float, 4, 1> dist_coeffs;

    cv::Mat image_ocv;
    cv::Mat image_ocv_rgb;
    sl::Transform pose;
public:

    static ArucoDetector* get()
    {
        if (!instance)   // Only allow one instance of class to be generated.
            instance = new ArucoDetector();

        return instance;
    }

    static bool isNotCreated()
    {
        return (instance == nullptr);
    }

    static void destroyInstance()
    {
        if (!instance)   // Only allow one instance of class to be generated.
            delete instance;
        instance = nullptr;

    }

    void init(float actual_marker_size_meters, cv::aruco::PREDEFINED_DICTIONARY_NAME dictionary_name, float fx, float fy, float cx, float cy);

    void detectMarkers(int width, int height, uchar* img_ptr);

    bool getPose(float& t_x, float& t_y, float& t_z, float& q_x, float& q_y, float& q_z, float& q_w);
};

#endif