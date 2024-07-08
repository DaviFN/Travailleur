#ifndef ImageUtils_H
#define ImageUtils_H

#include <opencv2/opencv.hpp>

namespace ImageUtils {

	bool matsAreEqual(const cv::Mat& mat1, const cv::Mat& mat2);

	//bool RGBtoHSV(const int r, const int g, const int b, int& h, int& s, int& v)
	//void RGBtoHSV_innerfunction(float& fR, float& fG, float fB, float& fH, float& fS, float& fV);
	//
	//void HSVtoRGB(const int h, const int s, const int v, int& r, int& g, int& b);
	//void HSVtoRGB_innerfunction(float& fR, float& fG, float& fB, float& fH, float& fS, float& fV);

	//RawImage* scaleRawImage(RawImage* sourceImage, const double scaleFactorX, const double scaleFactorY);
	//
	//RawImage* resizeRawImage(RawImage* sourceImage, const double newWidth, const double newHeight);

}

#endif