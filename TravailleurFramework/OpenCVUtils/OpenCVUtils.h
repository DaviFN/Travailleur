#ifndef OpenCVUtils_H
#define OpenCVUtils_H

#include <Windows.h>
#include "opencv2/opencv.hpp"
#include "Common/Image.h"

class OpenCVUtils {
	public:
		static void grabDesktopFrame(Image& output);
	private:
		static cv::Mat grabDesktopMat();
		static cv::Mat hwnd2mat(HWND hwnd);
};

#endif