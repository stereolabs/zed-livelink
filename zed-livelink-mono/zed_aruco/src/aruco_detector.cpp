#include "aruco_detector.hpp"
#include <opencv2/opencv.hpp>

ArucoDetector::ArucoDetector()
{

}

ArucoDetector* ArucoDetector::instance = nullptr;

void ArucoDetector::init(float actual_marker_size_meters, cv::aruco::PREDEFINED_DICTIONARY_NAME dictionary_name, float fx, float fy, float cx, float cy, bool display_image)
{
    actual_marker_size_meters_ = actual_marker_size_meters;
	dictionary = cv::aruco::getPredefinedDictionary(dictionary_name);
	display_image_ = display_image;

    camera_matrix = cv::Matx33d::eye();
    camera_matrix(0, 0) = fx;
    camera_matrix(1, 1) = fy;
    camera_matrix(0, 2) = cx;
    camera_matrix(1, 2) = cy;

    dist_coeffs = cv::Vec4f::zeros();
	init_ = true;
}


void ArucoDetector::detectMarkers(int width, int height, uchar* img_ptr)
{
	if (init_)
	{
		image_ocv = cv::Mat(height, width, CV_8UC4, img_ptr);
		image_ocv_rgb;
		cv::cvtColor(image_ocv, image_ocv_rgb, cv::COLOR_RGBA2RGB);

		cv::aruco::detectMarkers(image_ocv_rgb, dictionary, corners, ids);
	}
}

bool ArucoDetector::getPose(float& t_x, float& t_y, float& t_z, float& q_x, float& q_y, float& q_z, float& q_w)
{
	if (init_)
	{
		bool res = false;
		if (ids.size() > 0) {
			cv::aruco::estimatePoseSingleMarkers(corners, actual_marker_size_meters_, camera_matrix, dist_coeffs, rvecs, tvecs);
			pose.setTranslation(sl::float3(tvecs[0](0), tvecs[0](1), tvecs[0](2)));
			pose.setRotationVector(sl::float3(rvecs[0](0), rvecs[0](1), rvecs[0](2)));

			if (display_image_)
			{
				cv::aruco::drawDetectedMarkers(image_ocv_rgb, corners, ids);
				cv::aruco::drawAxis(image_ocv_rgb, camera_matrix, dist_coeffs, rvecs[0], tvecs[0], actual_marker_size_meters_ * 0.5f);
				cv::aruco::drawDetectedMarkers(image_ocv_rgb, corners, ids);
				cv::aruco::drawAxis(image_ocv_rgb, camera_matrix, dist_coeffs, rvecs[0], tvecs[0], actual_marker_size_meters_ * 0.5f);
				auto position_txt = "Aruco x: " + std::to_string(pose.tx) + "; y: " + std::to_string(pose.ty) + "; z: " + std::to_string(pose.tz);
				cv::putText(image_ocv_rgb, position_txt, cv::Point(10, 60), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(124, 252, 124));
			}


			sl::convertCoordinateSystem(pose, sl::COORDINATE_SYSTEM::IMAGE, sl::COORDINATE_SYSTEM::LEFT_HANDED_Z_UP);

			t_x = pose.getTranslation().x * 100; //meter to centimeter
			t_y = pose.getTranslation().y * 100; //meter to centimeter
			t_z = pose.getTranslation().z * 100; //meter to centimeter

			q_x = pose.getOrientation().x;
			q_y = pose.getOrientation().y;
			q_z = pose.getOrientation().z;
			q_w = pose.getOrientation().w;

			res = true;
		}
		else res = false;

		if (display_image_)
		{
			cv::imshow("zed img", image_ocv_rgb);
			cv::waitKey(10);
		}

		return res;
	}
	else return false;
}