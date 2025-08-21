#include "DesktopFrameGrabber.h"
#include "Common/Image.h"

#include <opencv2/opencv.hpp>

namespace SC = SL::Screen_Capture;

void DesktopFrameGrabber::update()
{
    constexpr double maxTimeAllowedMs = 2000.0;
    if (capturing && timeSinceLastGrab.TimeElapsed(Chronometer::Unit::Milliseconds) > maxTimeAllowedMs) {
        stopCapturing();
    }
}

void DesktopFrameGrabber::initiateCapturing()
{
    monitorCallback = []() {
        auto monitors = SC::GetMonitors();
        if (monitors.empty())
            return std::vector<SC::Monitor>{};
        return std::vector<SC::Monitor>{ monitors[0] };
        };

    captureConfig = SC::CreateCaptureConfiguration(monitorCallback);
    captureConfig->onNewFrame([&](const SC::Image& img, const SC::Monitor&) {
        if (!frameIsReady) {
            mutex.lock();
            const int width = img.Bounds.right - img.Bounds.left;
            const int height = img.Bounds.bottom - img.Bounds.top;
            frameMat = cv::Mat(height, width, CV_8UC4, (void*)img.Data).clone();
            frameIsReady = true;
            mutex.unlock();
        }
    });
    frameIsReady = false;
    capturing = true;
    captureConfig->start_capturing();
}

void DesktopFrameGrabber::stopCapturing()
{
    frameIsReady = false;
    capturing = false;
    captureConfig = nullptr;
}

void DesktopFrameGrabber::assureCapturing()
{
    if (!capturing) {
        initiateCapturing();
    }
}

void DesktopFrameGrabber::grabDesktopFrame(Image& output)
{
    assureCapturing();
    if (frameIsReady) {
        mutex.lock();
        output.setMat(frameMat.clone());
        mutex.unlock();
    }
    frameIsReady = false;
    timeSinceLastGrab.restart();
}