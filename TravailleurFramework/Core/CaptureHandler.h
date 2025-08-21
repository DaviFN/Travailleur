#ifndef CaptureHandler_H
#define CaptureHandler_H

#include <opencv2/opencv.hpp>

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "Common/Image.h"
#include "Common/Rectangle2D.h"
#include "Common/SimpleTimer.h"
#include "Core/DesktopFrameGrabber.h"
#include "ImGuiWrappers/ImGuiCombo.h"
#include "ImGuiWrappers/ImGuiCheckbox.h"
#include "ImGuiWrappers/ImGuiIntSlider.h"
#include "ImGuiWrappers/ImGuiImageRenderer.h"
#include "ImGuiWrappers/ImGuiSingleSelectionList.h"
#include "ImGuiWrappers/ImGuiTextInput.h"

class TravailleurContext;

class CaptureHandler
{
public:
    CaptureHandler(TravailleurContext* travailleurContext);
    ~CaptureHandler();

    static size_t captureWidth();
    static size_t captureHeight();
    static Rectangle2D<int> captureRegion();
    static bool pointIsInsideCaptureRegion(const Point2D<int> point);
    static bool regionIsInsideCaptureRegion(const Rectangle2D<int> region);

    Image* getFrame() const;
    void showGui();

    void update();

    Image* getFrameOverlays() const;

private:
    class VideoMetadata {
    public:
        struct FrameInfo {
            int framePosition;
            std::string description;
        };
        std::vector<FrameInfo> frameInfos;

        void addVideoFrameIfUnexisting(const std::string& videoFilename, const int framePosition, const std::string& description);
        void removeVideoFrame(const size_t index);
        void saveToBinaryFile(FILE* f) const;
        void loadFromBinaryFile(FILE* f);
    private:
        bool hasFrameInfoWithFramePosition(const int framePosition) const;
    };

    void save() const;
    void load();
    void showMainGui();
    void showSecondaryGui();
    void showVideoPlayerGui();
    void showVideoFrameSelectionGui();
    void goToSelectedVideoFrame();
    void addCurrentVideoFrame(const std::string& videoFilename, const std::string& description);
    void removeVideoFrame(const std::string& videoFilename, const size_t index);
    VideoMetadata* getOrCreateVideoMetadata(const std::string& videoFilename);
    void updateVideoFrameSelectionList(const std::string& videoFilename);
    void saveVideoMetadatas() const;
    void loadVideoMetadatas();
    void saveSelectedDeviceName(const std::string& captureDeviceName) const;
    std::string loadSelectedDeviceName() const;
    void clearFrameOverlays();
    void setFrameToBlackFrame();
    void setFrameToDefaultFrame();
    static size_t countCaptureDevices();
    void updateAvailableDevices();
    bool isConnectedToDevice() const;
    void connectToDevice();
    void disconnectFromDevice();

    int selectedDeviceIndex() const;

    void takeScreenshot();
    void startVideoRecording(const std::string& filepath);
    void stopVideoRecording();

    void setScreenshotNameToNextAvailableNumber();
    void setVideoRecordingNameToNextAvailableNumber();

    std::vector<std::string> getScreenshotsFilenames() const;
    void updateScreenshotSelectionList();
    std::vector<std::string> getVideoRecordingsFilenames() const;
    void updateVideoSelectionList();

    void displayFrame();
    void showSelectedScreenshot();
    void setFrameToSelectedScreenshot();

    void restoreVideoPlayingIfNeeded();
    void playSelectedVideo(const bool startPaused = false);
    void applyVideoReadCrashFix();
    bool readFrameFromVideoPlayer();
    void readAndAdvanceFrameFromVideoFilePlayerIfPossible();
    void updateFrameFromVideoBeingPlayed();
    void stopPlayingSelectedVideo();
    int getFrameRateOfVideoBeingPlayed() const;
    int getNumberOfFramesOfVideoBeingPlayed() const;
    int getCurrentFramePositionOfVideoBeingPlayed() const;
    double getDurationInSecondsOfVideoBeingPlayed() const;
    double getCurrentTimestampInSecondsOfVideoBeingPlayed() const;
    void setCurrentFramePositionOfVideoBeingPlayed(const size_t framePosition);
    void fastForwardVideoBeingPlayed(const size_t millisecondsToFastForward);
    void rewindVideoBeingPlayed(const size_t millisecondsToRewind);

    bool shouldGrabDesktop() const;
    void grabDesktopFrame();

    bool screenshotHotkeyPressed() const;

    cv::VideoCapture videoCapture;
    cv::VideoCapture videoFilePlayer;
    cv::VideoWriter videoWriter;
    std::unique_ptr<DesktopFrameGrabber> desktopFrameGrabber;
    ImGuiCombo availableDevicesCombo;
    Image* frame = nullptr;
    ImGuiImageRenderer* imGuiImageRenderer = nullptr;
    SimpleTimer timer;
    int previousIndexOfSelectedDevice = -1;
    bool captureHasBeenRequested = false;
    int* escapiCaptureBuffer = nullptr;
    std::unique_ptr<ImGuiCheckbox> autoconnectOnStartup;
    std::unique_ptr<ImGuiCheckbox> grabDesktopInstead;
    std::unique_ptr<ImGuiCheckbox> resizeFrameCheckbox;
    std::unique_ptr<ImGuiIntSlider> frameDisplayResizeWidthSlider;
    std::unique_ptr<ImGuiIntSlider> frameDisplayResizeHeightSlider;
    Image* frameOverlays = nullptr;
    std::unique_ptr<ImGuiCheckbox> showFrameOverlays;
    std::unique_ptr<ImGuiTextInput> screenshotCaptureFilename;
    std::unique_ptr<ImGuiTextInput> videoRecordingFilename;
    std::unique_ptr<ImGuiSingleSelectionList> screenshotSelectionList;
    std::unique_ptr<ImGuiSingleSelectionList> videoSelectionList;
    std::unique_ptr<ImGuiIntSlider> videoPlayerStep;

    bool screenshotBeingShown = false;

    enum class VideoPlayerStatus {
        NotInUse,
        Playing,
        Paused
    };
    VideoPlayerStatus videoPlayerStatus = VideoPlayerStatus::NotInUse;
    int currentFramePositionInVideoBeingPlayed = 0;
    std::map<std::string, VideoMetadata*> mapFromVideoFilenameToVideoMetadata;
    std::unique_ptr<ImGuiSingleSelectionList> videoFrameSelectionList;
    std::unique_ptr<ImGuiTextInput> descriptionOfVideoFrameToAdd;

    TravailleurContext* travailleurContext = nullptr;
};

#endif