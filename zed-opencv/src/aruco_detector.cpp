#include "aruco_detector.hpp"
#include <opencv2/opencv.hpp>

ArucoDetector::ArucoDetector()
{

}

ArucoDetector* ArucoDetector::instance = nullptr;

void ArucoDetector::init(float actual_marker_size_meters, cv::aruco::PREDEFINED_DICTIONARY_NAME dictionary_name, float fx, float fy, float cx, float cy)
{
    actual_marker_size_meters_ = actual_marker_size_meters;
	dictionary = cv::aruco::getPredefinedDictionary(dictionary_name);

    camera_matrix = cv::Matx33d::eye();
    camera_matrix(0, 0) = fx;
    camera_matrix(1, 1) = fy;
    camera_matrix(0, 2) = cx;
    camera_matrix(1, 2) = cy;

    dist_coeffs = cv::Vec4f::zeros();
}


void ArucoDetector::detectMarkers(int width, int height, uchar* img_ptr)
{
    image_ocv = cv::Mat(height, width, CV_8UC4, img_ptr);
    image_ocv_rgb;
    cv::cvtColor(image_ocv, image_ocv_rgb, cv::COLOR_RGBA2RGB);

    cv::aruco::detectMarkers(image_ocv_rgb, dictionary, corners, ids);
}

bool ArucoDetector::getPose(float& t_x, float& t_y, float& t_z, float& q_x, float& q_y, float& q_z, float& q_w)
{
    bool res = false;
    if (ids.size() > 0) {
        cv::aruco::estimatePoseSingleMarkers(corners, actual_marker_size_meters_, camera_matrix, dist_coeffs, rvecs, tvecs);

        pose.setTranslation(sl::float3(tvecs[0](0), tvecs[0](1), tvecs[0](2)));
        pose.setRotationVector(sl::float3(rvecs[0](0), rvecs[0](1), rvecs[0](2)));
        pose.inverse();

        cv::aruco::drawDetectedMarkers(image_ocv_rgb, corners, ids);
        cv::aruco::drawAxis(image_ocv_rgb, camera_matrix, dist_coeffs, rvecs[0], tvecs[0], actual_marker_size_meters_ * 0.5f);
        cv::imshow("test", image_ocv_rgb);
        cv::waitKey(10);

        sl::convertCoordinateSystem(pose, sl::COORDINATE_SYSTEM::IMAGE, sl::COORDINATE_SYSTEM::LEFT_HANDED_Z_UP);

        t_x = pose.getTranslation().x * 100;
        t_y = pose.getTranslation().y * 100;
        t_z = pose.getTranslation().z * 100;

        q_x = pose.getOrientation().x;
        q_y = pose.getOrientation().y;
        q_z = pose.getOrientation().z;
        q_w = pose.getOrientation().w;

        res =  true;
    }
    else res = false;

    cv::imshow("test", image_ocv_rgb);
    cv::waitKey(10);
    return res;
}