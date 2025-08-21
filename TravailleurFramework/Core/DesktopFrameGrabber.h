#ifndef DesktopFrameGrabber_H
#define DesktopFrameGrabber_H

#include "opencv2/opencv.hpp"
#include "Common/Chronometer.h"
#include "Common/Image.h"
#include "ScreenCapture.h"

#include <mutex>

class DesktopFrameGrabber {
public:
    void grabDesktopFrame(Image& output);
    void update();
private:
    void initiateCapturing();
    void stopCapturing();
    void assureCapturing();

    bool capturing = false;
    Chronometer timeSinceLastGrab;
    std::atomic<bool> frameIsReady = false;

    SL::Screen_Capture::MonitorCallback monitorCallback;
    std::shared_ptr<SL::Screen_Capture::ICaptureConfiguration<SL::Screen_Capture::ScreenCaptureCallback>> captureConfig;

    std::mutex mutex;
    cv::Mat frameMat;
};

#endif