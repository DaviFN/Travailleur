#include "ColorUtils.h"

#include <opencv2/opencv.hpp>

// inputs: r, g, b in range 0-255, outputs: h, s, v in ranges h 0-179, s, v 0-255
bool ColorUtils::RGBtoHSV(const int r, const int g, const int b, int& h, int& s, int& v)
{
    if (r < 0 || r > 255) return false;
    if (g < 0 || g > 255) return false;
    if (b < 0 || b > 255) return false;
    static cv::Mat mat(1, 1, CV_8UC3, cv::Scalar(0, 0, 0));
    mat.at<cv::Vec3b>(0, 0)[0] = b;
    mat.at<cv::Vec3b>(0, 0)[1] = g;
    mat.at<cv::Vec3b>(0, 0)[2] = r;
    cv::cvtColor(mat, mat, cv::COLOR_BGR2HSV);
    h = (int)mat.at<cv::Vec3b>(0, 0)[0];
    s = (int)mat.at<cv::Vec3b>(0, 0)[1];
    v = (int)mat.at<cv::Vec3b>(0, 0)[2];
    cv::cvtColor(mat, mat, cv::COLOR_HSV2BGR);
    return true;
}

// inputs: r, g, b in range 0-255, outputs: h, s, v in ranges h 0-179, s, v 0-255
bool ColorUtils::HSVtoRGB(const int h, const int s, const int v, int& r, int& g, int& b)
{
    if (h < 0 || h > 179) return false;
    if (s < 0 || s > 255) return false;
    if (v < 0 || v > 255) return false;
    static cv::Mat mat(1, 1, CV_8UC3, cv::Scalar(0, 0, 0));
    static bool firstTimeBeingCalled = true;
    if (firstTimeBeingCalled) {
        cv::cvtColor(mat, mat, cv::COLOR_BGR2HSV);
        firstTimeBeingCalled = false;
    }
    mat.at<cv::Vec3b>(0, 0)[0] = h;
    mat.at<cv::Vec3b>(0, 0)[1] = s;
    mat.at<cv::Vec3b>(0, 0)[2] = v;
    cv::cvtColor(mat, mat, cv::COLOR_HSV2BGR);
    b = (int)mat.at<cv::Vec3b>(0, 0)[0];
    g = (int)mat.at<cv::Vec3b>(0, 0)[1];
    r = (int)mat.at<cv::Vec3b>(0, 0)[2];
    cv::cvtColor(mat, mat, cv::COLOR_BGR2HSV);
    return true;
}