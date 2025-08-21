#include "CaptureHandler.h"

#include "imgui.h"
#include "Common/FilesystemUtils.h"
#include "Common/TravailleurUtils.h"
#include "Common/StringUtils.h"
#include "ImGuiWrappers/ImGuiTextOutput.h"
#include "TravailleurContext.h"

CaptureHandler::CaptureHandler(TravailleurContext* travailleurContext):
    travailleurContext(travailleurContext),
    availableDevicesCombo("Available devices"),
    escapiCaptureBuffer(new int[captureWidth() * captureHeight()]),
    imGuiImageRenderer(new ImGuiImageRenderer),
    frameOverlays(new Image(captureWidth(), captureHeight()))
{
    setFrameToBlackFrame();

    updateAvailableDevices();

    desktopFrameGrabber = std::make_unique<DesktopFrameGrabber>();

    autoconnectOnStartup = std::make_unique<ImGuiCheckbox>("Autoconnect on startup", true, TravailleurUtils::getConfigFilenameFor("autoconnect-on-startup", travailleurContext->getName()));

    grabDesktopInstead = std::make_unique<ImGuiCheckbox>("Grab desktop instead", true, TravailleurUtils::getConfigFilenameFor("grab-desktop-instead", travailleurContext->getName()));

    resizeFrameCheckbox = std::make_unique<ImGuiCheckbox>("Resize frame", true, TravailleurUtils::getConfigFilenameFor("resize-frame", travailleurContext->getName()));
    frameDisplayResizeWidthSlider = std::make_unique<ImGuiIntSlider>("width", captureWidth(), captureWidth() / 10, captureWidth(), 1.0, TravailleurUtils::getConfigFilenameFor("frame-display-resize-width", travailleurContext->getName()));
    frameDisplayResizeHeightSlider = std::make_unique<ImGuiIntSlider>("height", captureHeight(), captureHeight() / 10, captureHeight(), 1.0, TravailleurUtils::getConfigFilenameFor("frame-display-resize-height", travailleurContext->getName()));

    showFrameOverlays = std::make_unique<ImGuiCheckbox>("Show frame overlays", true, TravailleurUtils::getConfigFilenameFor("show-frame-overlays", travailleurContext->getName()));

    screenshotCaptureFilename = std::make_unique<ImGuiTextInput>("Screenshot capture filename", TravailleurUtils::getConfigFilenameFor("screenshot-capture-filename", travailleurContext->getName()));
    videoRecordingFilename = std::make_unique<ImGuiTextInput>("Video recording filename", TravailleurUtils::getConfigFilenameFor("video-recording-filename", travailleurContext->getName()));

    screenshotSelectionList = std::make_unique<ImGuiSingleSelectionList>("Screenshot selection");
    videoSelectionList = std::make_unique<ImGuiSingleSelectionList>("Video selection");

    videoPlayerStep = std::make_unique<ImGuiIntSlider>("step (ms)", 2000, 1, 1000000, 1.0, TravailleurUtils::getConfigFilenameFor("video-player-step", travailleurContext->getName()));

    videoFrameSelectionList = std::make_unique<ImGuiSingleSelectionList>("Frame selection");
    videoFrameSelectionList->setHowManyItemsToShow(6);
    descriptionOfVideoFrameToAdd = std::make_unique<ImGuiTextInput>("Description of video frame to add", TravailleurUtils::getConfigFilenameFor("description-of-video-frame-to-add", travailleurContext->getName()));

    updateScreenshotSelectionList();
    updateVideoSelectionList();

    load();

    if (autoconnectOnStartup->getValue()) {
        connectToDevice();
    }
}

CaptureHandler::~CaptureHandler()
{
    save();

    disconnectFromDevice();

	delete frame;
    delete imGuiImageRenderer;

    delete frameOverlays;
}

size_t CaptureHandler::captureWidth()
{
    return 1024;
}

size_t CaptureHandler::captureHeight()
{
    return 768;
}

Rectangle2D<int> CaptureHandler::captureRegion()
{
    return Rectangle2D<int>(Point2D<int>(0, 0), Point2D(static_cast<int>(captureWidth()), static_cast<int>(captureWidth())));
}

bool CaptureHandler::pointIsInsideCaptureRegion(const Point2D<int> point)
{
    return captureRegion().pointIsInside(point);
}

bool CaptureHandler::regionIsInsideCaptureRegion(const Rectangle2D<int> region)
{
    return region.isInsideOther(captureRegion());
}

void CaptureHandler::save() const
{
    SimplePersistenceUtils::saveString("selected-screenshot", screenshotSelectionList->getSelectedOption(), TravailleurUtils::getConfigPath(travailleurContext->getName()));

    SimplePersistenceUtils::saveString("selected-video", videoSelectionList->getSelectedOption(), TravailleurUtils::getConfigPath(travailleurContext->getName()));

    SimplePersistenceUtils::saveInt32("video-player-status", (int)videoPlayerStatus, TravailleurUtils::getConfigPath(travailleurContext->getName()));
    
    saveVideoMetadatas();
    for (auto& mapItem : mapFromVideoFilenameToVideoMetadata) {
        VideoMetadata* videoMetadata = mapItem.second;
        delete videoMetadata;
    }

    if (videoPlayerStatus != VideoPlayerStatus::NotInUse) {
        SimplePersistenceUtils::saveInt32("current-frame-position-in-video-being-played", currentFramePositionInVideoBeingPlayed, TravailleurUtils::getConfigPath(travailleurContext->getName()));
    }

    saveSelectedDeviceName(availableDevicesCombo.getSelectedOption());
}

void CaptureHandler::load()
{
    availableDevicesCombo.setSelectedOption(loadSelectedDeviceName());
    previousIndexOfSelectedDevice = availableDevicesCombo.getSelectedIndex();

    std::string savedSelectedScreenshot;
    SimplePersistenceUtils::loadString("selected-screenshot", savedSelectedScreenshot, TravailleurUtils::getConfigPath(travailleurContext->getName()));
    screenshotSelectionList->setSelectedOption(savedSelectedScreenshot);

    setFrameToSelectedScreenshot();

    std::string savedSelectedVideo;
    SimplePersistenceUtils::loadString("selected-video", savedSelectedVideo, TravailleurUtils::getConfigPath(travailleurContext->getName()));
    videoSelectionList->setSelectedOption(savedSelectedVideo);

    int videoPlayerStatusAsInt = 0;
    SimplePersistenceUtils::loadInt32("video-player-status", videoPlayerStatusAsInt, TravailleurUtils::getConfigPath(travailleurContext->getName()));
    videoPlayerStatus = static_cast<VideoPlayerStatus>(videoPlayerStatusAsInt);

    loadVideoMetadatas();

    restoreVideoPlayingIfNeeded();

    if (videoPlayerStatus != VideoPlayerStatus::NotInUse) {
        SimplePersistenceUtils::loadInt32("current-frame-position-in-video-being-played", currentFramePositionInVideoBeingPlayed, TravailleurUtils::getConfigPath(travailleurContext->getName()));
        setCurrentFramePositionOfVideoBeingPlayed(currentFramePositionInVideoBeingPlayed);
    }
}

void CaptureHandler::saveVideoMetadatas() const
{
    for (const auto& mapItem : mapFromVideoFilenameToVideoMetadata) {
        const std::string& videoFilename = mapItem.first;
        const VideoMetadata* metadata = mapItem.second;
        const std::string metadataFilename = videoFilename + ".metadata";
        const std::string videoRecordingsPath =TravailleurUtils::getVideoRecordingsPath(travailleurContext->getName());
        const std::string fullMetadataFilepath = videoRecordingsPath + "\\" + metadataFilename;
        
        FILE* f = fopen(fullMetadataFilepath.c_str(), "wb");
        if (f != nullptr) {
            metadata->saveToBinaryFile(f);
            fclose(f);
        }
    }
}

void CaptureHandler::loadVideoMetadatas()
{
    const std::vector<std::string> videoRecordingsFilenames = getVideoRecordingsFilenames();
    for (const std::string& videoFilename : videoRecordingsFilenames) {
        VideoMetadata* metadata = getOrCreateVideoMetadata(videoFilename);
        const std::string metadataFilename = videoFilename + ".metadata";
        const std::string videoRecordingsPath = TravailleurUtils::getVideoRecordingsPath(travailleurContext->getName());
        const std::string fullMetadataFilepath = videoRecordingsPath + "\\" + metadataFilename;

        FILE* f = fopen(fullMetadataFilepath.c_str(), "rb");
        if (f != nullptr) {
            metadata->loadFromBinaryFile(f);
            fclose(f);
        }

        mapFromVideoFilenameToVideoMetadata[videoFilename] = metadata;
    }
}

void CaptureHandler::saveSelectedDeviceName(const std::string& captureDeviceName) const
{
    SimplePersistenceUtils::saveString(
        "selected-capture-device-name",
        captureDeviceName,
        TravailleurUtils::getConfigPath(travailleurContext->getName())
    );
}

std::string CaptureHandler::loadSelectedDeviceName() const
{
    std::string captureDeviceName;
    SimplePersistenceUtils::loadString(
        "selected-capture-device-name",
        captureDeviceName,
        TravailleurUtils::getConfigPath(travailleurContext->getName())
    );
    return captureDeviceName;
}

void CaptureHandler::clearFrameOverlays()
{
    frameOverlays->setToEmptyMaskForImage(*frame);
}

void CaptureHandler::setFrameToBlackFrame()
{
    delete frame;
    frame = new Image(captureWidth(), captureHeight());
    frame->fillWithSamePixel(Pixel(0, 0, 0));
    clearFrameOverlays();
}

void CaptureHandler::setFrameToDefaultFrame()
{
    setFrameToSelectedScreenshot();
}

Image* CaptureHandler::getFrame() const
{
	return frame;
}

Image* CaptureHandler::getFrameOverlays() const
{
    return frameOverlays;
}

int CaptureHandler::selectedDeviceIndex() const
{
    return availableDevicesCombo.getSelectedIndex();
}

size_t CaptureHandler::countCaptureDevices()
{
    constexpr size_t maxTested = 10;
    size_t count = 0;
    for (size_t i = 0; i < maxTested; ++i) {
        cv::VideoCapture cap(i, cv::CAP_ANY);
        if (cap.isOpened()) {
            ++count;
            cap.release();
        }
    }
    return count;
}

void CaptureHandler::updateAvailableDevices()
{
    if (isConnectedToDevice()) {
        disconnectFromDevice();
    }
    std::vector<std::string> availableDevicesOptions;
    const int nCaptureDevices = countCaptureDevices();
    for (int i = 0; i < nCaptureDevices; ++i) {
        availableDevicesOptions.push_back("Device " + std::to_string(i + 1));
    }
    availableDevicesCombo.setOptions(availableDevicesOptions);
}

void CaptureHandler::connectToDevice()
{
    videoCapture.open(selectedDeviceIndex());
    videoCapture.set(cv::CAP_PROP_FRAME_WIDTH, captureWidth());
    videoCapture.set(cv::CAP_PROP_FRAME_HEIGHT, captureHeight());
}

void CaptureHandler::disconnectFromDevice()
{
    videoCapture.release();
    setFrameToDefaultFrame();

    if (videoWriter.isOpened()) {
        stopVideoRecording();
    }
}

void CaptureHandler::update()
{
    desktopFrameGrabber->update();
    if (shouldGrabDesktop()) {
        grabDesktopFrame();
    }
    else if (isConnectedToDevice()) {
        videoCapture.read(*frame->getMat());
        cv::cvtColor(*frame->getMat(), *frame->getMat(), cv::COLOR_BGR2BGRA);
    }
    else if (videoFilePlayer.isOpened()) {
        updateFrameFromVideoBeingPlayed();
    }
    if (videoWriter.isOpened()) {
        videoWriter.write(*frame->getMat());
    }
    clearFrameOverlays();
}

bool CaptureHandler::isConnectedToDevice() const
{
    return videoCapture.isOpened();
}

void CaptureHandler::takeScreenshot()
{
    const std::string filename = screenshotCaptureFilename->getValue();
    if (filename != "") {
        const std::string screenshotsPath = TravailleurUtils::getScreenshotsPath(travailleurContext->getName());
        const std::string filepath = screenshotsPath + "\\" + filename + ".png";
        if (!FilesystemUtils::fileExists(filepath)) {
            frame->saveAsPng(filepath);
        }
        else {
            screenshotCaptureFilename->setValue("");
        }
    }
}

void CaptureHandler::setScreenshotNameToNextAvailableNumber()
{
    const std::string screenshotsPath = TravailleurUtils::getScreenshotsPath(travailleurContext->getName());
    const std::string nextFilename = FilesystemUtils::getNextNumberFilenameBasedOnExistingFiles(screenshotsPath, "png");
    screenshotCaptureFilename->setValue(nextFilename);
}

void CaptureHandler::startVideoRecording(const std::string& filepath)
{
    const std::string filename = videoRecordingFilename->getValue();
    if (filename != "") {
        constexpr double fps = 30.0;
        if (shouldGrabDesktop()) {
            const cv::Size frameSize(static_cast<int>(frame->getWidth()), static_cast<int>(frame->getHeight()));
            videoWriter.open(filepath, cv::VideoWriter::fourcc('h', '2', '6', '5'), fps, frameSize, true);
        }
        else if (!videoWriter.isOpened() && !FilesystemUtils::fileExists(filepath)) {
            const double width = videoCapture.get(cv::CAP_PROP_FRAME_WIDTH);
            const double height = videoCapture.get(cv::CAP_PROP_FRAME_HEIGHT);
            const cv::Size frameSize(static_cast<int>(width), static_cast<int>(height));
            videoWriter.open(filepath, cv::VideoWriter::fourcc('h', '2', '6', '5'), fps, frameSize, true);
        }
    }
}

void CaptureHandler::stopVideoRecording()
{
    if (videoWriter.isOpened()) {
        videoWriter.release();
    }
}

void CaptureHandler::setVideoRecordingNameToNextAvailableNumber()
{
    const std::string videoRecordingsPath = TravailleurUtils::getVideoRecordingsPath(travailleurContext->getName());
    const std::string nextFilename = FilesystemUtils::getNextNumberFilenameBasedOnExistingFiles(videoRecordingsPath, "mp4");
    videoRecordingFilename->setValue(nextFilename);
}

std::vector<std::string> CaptureHandler::getScreenshotsFilenames() const
{
    const std::string screenshotsPath = TravailleurUtils::getScreenshotsPath(travailleurContext->getName());
    std::vector<std::string> screenshotsFilenames;
    FilesystemUtils::globFiles(screenshotsFilenames, screenshotsPath, "png", false);
    for (std::string& item : screenshotsFilenames) {
        item = FilesystemUtils::filenameFromFilepath(item);
    }
    return screenshotsFilenames;
}

void CaptureHandler::updateScreenshotSelectionList()
{
    const std::vector<std::string> options = getScreenshotsFilenames();
    screenshotSelectionList->setOptions(options);
}

std::vector<std::string> CaptureHandler::getVideoRecordingsFilenames() const
{
    const std::string videoRecordingsPath = TravailleurUtils::getVideoRecordingsPath(travailleurContext->getName());
    std::vector<std::string> videoRecordingsFilenames;
    FilesystemUtils::globFiles(videoRecordingsFilenames, videoRecordingsPath, "mp4", false);
    for (std::string& item : videoRecordingsFilenames) {
        item = FilesystemUtils::filenameFromFilepath(item);
    }
    return videoRecordingsFilenames;
}

void CaptureHandler::updateVideoSelectionList()
{
    std::vector<std::string> options = getVideoRecordingsFilenames();
    videoSelectionList->setOptions(options);
}

void CaptureHandler::showSelectedScreenshot()
{
    stopVideoRecording();
    disconnectFromDevice();

    setFrameToSelectedScreenshot();
}

void CaptureHandler::setFrameToSelectedScreenshot()
{
    const std::string filepath = TravailleurUtils::getScreenshotsPath(travailleurContext->getName()) + "\\" + screenshotSelectionList->getSelectedOption();
    if (FilesystemUtils::fileExists(filepath)) {
        frame->loadFromFile(filepath);
        screenshotBeingShown = true;
    }
    else {
        setFrameToBlackFrame();
    }
}

void CaptureHandler::restoreVideoPlayingIfNeeded()
{
    if (videoPlayerStatus != VideoPlayerStatus::NotInUse)
    {
        const bool startPaused = videoPlayerStatus == VideoPlayerStatus::Paused;
        playSelectedVideo(startPaused);
    }
}

void CaptureHandler::playSelectedVideo(const bool startPaused)
{
    if (videoFilePlayer.isOpened()) return;
    stopVideoRecording();
    disconnectFromDevice();
    screenshotBeingShown = false;
    
    const std::string videoFilename = videoSelectionList->getSelectedOption();

    const std::string filepath = TravailleurUtils::getVideoRecordingsPath(travailleurContext->getName()) + "\\" + videoFilename;
    if (FilesystemUtils::fileExists(filepath)) {
        videoFilePlayer.open(filepath);
        if (!videoFilePlayer.isOpened()) {
            videoPlayerStatus = VideoPlayerStatus::NotInUse;
            return;
        }
        if (startPaused) {
            videoPlayerStatus = VideoPlayerStatus::Paused;
        }
        else {
            videoPlayerStatus = VideoPlayerStatus::Playing;
        }
        currentFramePositionInVideoBeingPlayed = 0;
        updateVideoFrameSelectionList(videoFilename);
    }
}

int CaptureHandler::getFrameRateOfVideoBeingPlayed() const
{
    if (!videoFilePlayer.isOpened()) return 0;

    return videoFilePlayer.get(cv::CAP_PROP_FPS);
}

int CaptureHandler::getNumberOfFramesOfVideoBeingPlayed() const
{
    if (!videoFilePlayer.isOpened()) return 0;

    return videoFilePlayer.get(cv::CAP_PROP_FRAME_COUNT);
}

double CaptureHandler::getDurationInSecondsOfVideoBeingPlayed() const
{
    if (!videoFilePlayer.isOpened()) return 0;

    return static_cast<double>(getNumberOfFramesOfVideoBeingPlayed())/(getFrameRateOfVideoBeingPlayed());
}

double CaptureHandler::getCurrentTimestampInSecondsOfVideoBeingPlayed() const
{
    if (!videoFilePlayer.isOpened()) return 0;

    return static_cast<double>(currentFramePositionInVideoBeingPlayed) / getFrameRateOfVideoBeingPlayed();
}

int CaptureHandler::getCurrentFramePositionOfVideoBeingPlayed() const
{
    if (!videoFilePlayer.isOpened()) return 0;

    return videoFilePlayer.get(cv::CAP_PROP_POS_FRAMES);
}

void CaptureHandler::setCurrentFramePositionOfVideoBeingPlayed(const size_t framePosition)
{
    if (!videoFilePlayer.isOpened()) return;
    if (framePosition > getNumberOfFramesOfVideoBeingPlayed()) return;

    if (framePosition == 0) {
        videoFilePlayer.set(cv::CAP_PROP_POS_FRAMES, framePosition);
        setFrameToBlackFrame();
    }
    else {
        videoFilePlayer.set(cv::CAP_PROP_POS_FRAMES, framePosition - 1);
        readFrameFromVideoPlayer();
    }

    currentFramePositionInVideoBeingPlayed = framePosition;
    
}

void CaptureHandler::fastForwardVideoBeingPlayed(const size_t millisecondsToFastForward)
{
    if (!videoFilePlayer.isOpened()) return;

    const size_t currentFramePosition = getCurrentFramePositionOfVideoBeingPlayed();

    const double secondsToFastForward = static_cast<double>(millisecondsToFastForward) / 1000;

    currentFramePositionInVideoBeingPlayed += secondsToFastForward * getFrameRateOfVideoBeingPlayed();
    currentFramePositionInVideoBeingPlayed = std::min(currentFramePositionInVideoBeingPlayed, getNumberOfFramesOfVideoBeingPlayed());

    setCurrentFramePositionOfVideoBeingPlayed(currentFramePositionInVideoBeingPlayed);
}

void CaptureHandler::rewindVideoBeingPlayed(const size_t millisecondsToRewind)
{
    if (!videoFilePlayer.isOpened()) return;

    const size_t currentFramePosition = getCurrentFramePositionOfVideoBeingPlayed();
    const double secondsToFastForward = static_cast<double>(millisecondsToRewind) / 1000;

    currentFramePositionInVideoBeingPlayed -= secondsToFastForward * getFrameRateOfVideoBeingPlayed();
    currentFramePositionInVideoBeingPlayed = std::max(currentFramePositionInVideoBeingPlayed, 0);

    setCurrentFramePositionOfVideoBeingPlayed(currentFramePositionInVideoBeingPlayed);
}

void CaptureHandler::applyVideoReadCrashFix()
{
    // unsure exactly why this has to be done, but, if not, a crash happens
    if (!frame->getMat()->empty()) {
        cv::cvtColor(*frame->getMat(), *frame->getMat(), cv::COLOR_RGBA2BGRA);
        cv::cvtColor(*frame->getMat(), *frame->getMat(), cv::COLOR_BGRA2RGBA);
    }
    else {
        setFrameToBlackFrame();
    }
}

bool CaptureHandler::readFrameFromVideoPlayer()
{
    const bool wasAbleToReadFrame = videoFilePlayer.read(*frame->getMat());
    applyVideoReadCrashFix();
    return wasAbleToReadFrame;
}

void CaptureHandler::readAndAdvanceFrameFromVideoFilePlayerIfPossible()
{
    if (currentFramePositionInVideoBeingPlayed >= getNumberOfFramesOfVideoBeingPlayed()) return;

    const bool wasAbleToReadFrame = readFrameFromVideoPlayer();
    if (wasAbleToReadFrame) {
        ++currentFramePositionInVideoBeingPlayed;
    }
}

void CaptureHandler::updateFrameFromVideoBeingPlayed()
{
    if (!videoFilePlayer.isOpened()) return;

    if (videoPlayerStatus != VideoPlayerStatus::Paused) {

        if (videoPlayerStatus == VideoPlayerStatus::Playing) {
            readAndAdvanceFrameFromVideoFilePlayerIfPossible();
        }
    }

}

void CaptureHandler::stopPlayingSelectedVideo()
{
    if (!videoFilePlayer.isOpened()) return;

    videoFilePlayer.release();
    setFrameToDefaultFrame();

    videoPlayerStatus = VideoPlayerStatus::NotInUse;
    currentFramePositionInVideoBeingPlayed = 0;
}

void CaptureHandler::showGui()
{
    showMainGui();
    showSecondaryGui();
}

void CaptureHandler::displayFrame()
{
    if (frame != nullptr) {
        std::unique_ptr<Image> frameToRender = std::make_unique<Image>(*frame);

        if (showFrameOverlays->getValue()) {
            frameToRender->drawMask(*frameOverlays);
        }
        if (resizeFrameCheckbox->getValue()) {
            frameToRender->resize(frameDisplayResizeWidthSlider->getValue(), frameDisplayResizeHeightSlider->getValue());
        }

        imGuiImageRenderer->renderImage(*frameToRender);
    }
    else {
        ImGuiTextOutput("no frame to display");
    }
}

void CaptureHandler::showMainGui()
{
    ImGui::Begin("CaptureHandler - Main");

    displayFrame();

    if (selectedDeviceIndex() != previousIndexOfSelectedDevice) {
        if (isConnectedToDevice()) {
            disconnectFromDevice();
        }
        previousIndexOfSelectedDevice = availableDevicesCombo.getSelectedIndex();

        saveSelectedDeviceName(availableDevicesCombo.getSelectedOption());
    }
    availableDevicesCombo.showGui();

    if (ImGui::Button("Update available devices")) {
        updateAvailableDevices();
    }

    if (availableDevicesCombo.getOptions().size() > 0) {
        if (!isConnectedToDevice()) {
            if (ImGui::Button("Connect")) {
                connectToDevice();
            }
        }
        else {
            if (ImGui::Button("Disconnect")) {
                disconnectFromDevice();
            }
        }
    }

    ImGuiTextOutput("Capture resolution: " + std::to_string(captureWidth()) + "x" + std::to_string(captureHeight()));

    autoconnectOnStartup->showGui();

    grabDesktopInstead->showGui();

    resizeFrameCheckbox->showGui();
    if (resizeFrameCheckbox->getValue()) {
        frameDisplayResizeWidthSlider->showGui();
        frameDisplayResizeHeightSlider->showGui();
    }

    showFrameOverlays->showGui();

    ImGui::End();
}

void CaptureHandler::showSecondaryGui()
{
    ImGui::Begin("CaptureHandler - Secondary");

    if (!videoWriter.isOpened()) {
        if (ImGui::Button("Record video")) {
            const std::string recordingFilename = TravailleurUtils::getVideoRecordingsPath(travailleurContext->getName()) + "\\" + videoRecordingFilename->getValue() + ".mp4";
            if (!FilesystemUtils::fileExists(recordingFilename)) {
                startVideoRecording(recordingFilename);
            }
            else {
                setVideoRecordingNameToNextAvailableNumber();
            }
        }
    }
    else {
        if (ImGui::Button("Stop recording")) {
            stopVideoRecording();
            setVideoRecordingNameToNextAvailableNumber();
            updateVideoSelectionList();
        }
    }

    videoRecordingFilename->showGui();

    if (ImGui::Button("Save screenshot (CTRL + SHIFT + Q)") || screenshotHotkeyPressed()) {
        takeScreenshot();
        setScreenshotNameToNextAvailableNumber();
        updateScreenshotSelectionList();
    }

    screenshotCaptureFilename->showGui();

    if (ImGui::Button("Open work folder in explorer")) {
        TravailleurUtils::openWorkFolderInExplorer();
    }

    if (videoPlayerStatus == VideoPlayerStatus::NotInUse) {
        screenshotSelectionList->showGui();
        if (screenshotSelectionList->changedButNotMarkedAsDealtWith()) {
            if (screenshotBeingShown) {
                showSelectedScreenshot();
            }
            screenshotSelectionList->markAsDealtWithChange();
        }
        if (ImGui::Button("Update screenshot selection list")) {
            updateScreenshotSelectionList();
        }

        if (ImGui::Button("Show selected screenshot on capture handler")) {
            showSelectedScreenshot();
        }
    }

    if (videoPlayerStatus == VideoPlayerStatus::NotInUse) {
        videoSelectionList->showGui();
        if (ImGui::Button("Update video selection list")) {
            updateVideoSelectionList();
        }
    }

    showVideoPlayerGui();

    ImGui::End();
}

void CaptureHandler::showVideoPlayerGui()
{
    if (!videoFilePlayer.isOpened()) {
        if (ImGui::Button("Play selected video on capture handler")) {
            playSelectedVideo();
        }
    }
    else {
        const std::string videoFilename = videoSelectionList->getSelectedOption();
        ImGuiTextOutput("video: " + videoFilename);

        ImGuiTextOutput("frame rate: " + std::to_string(getFrameRateOfVideoBeingPlayed()) + " fps");
        ImGuiTextOutput("# frames: " + std::to_string(getNumberOfFramesOfVideoBeingPlayed()));
        ImGuiTextOutput("current frame position: " + std::to_string(currentFramePositionInVideoBeingPlayed));
        ImGuiTextOutput("duration (s): " + std::to_string(getDurationInSecondsOfVideoBeingPlayed()));
        ImGuiTextOutput("curent timestamp (s): " + std::to_string(getCurrentTimestampInSecondsOfVideoBeingPlayed()));

        if (ImGui::Button("Stop playing video")) {
            stopPlayingSelectedVideo();
        }

        if (videoPlayerStatus == VideoPlayerStatus::Playing) {
            if (ImGui::Button("Pause video")) {
                videoPlayerStatus = VideoPlayerStatus::Paused;
            }
        }
        else {
            if (ImGui::Button("Play video")) {
                videoPlayerStatus = VideoPlayerStatus::Playing;
            }
        }
        

        static bool previousFastForwardButtonState = false;
        if (ImGui::Button("Fast forward")) {
            if (!previousFastForwardButtonState) {
                fastForwardVideoBeingPlayed(videoPlayerStep->getValue());
            }
            previousFastForwardButtonState = true;
        }
        else {
            previousFastForwardButtonState = false;
        }

        static bool previousRewindButtonState = false;
        if (ImGui::Button("Rewind")) {
            if (!previousRewindButtonState) {
                rewindVideoBeingPlayed(videoPlayerStep->getValue());
            }
            previousRewindButtonState = true;
        }
        else {
            previousRewindButtonState = false;
        }

        videoPlayerStep->showGui();

        showVideoFrameSelectionGui();
    }
}

void CaptureHandler::showVideoFrameSelectionGui()
{
    descriptionOfVideoFrameToAdd->showGui();
    if (ImGui::Button("Add current video frame")) {
        addCurrentVideoFrame(videoSelectionList->getSelectedOption(), descriptionOfVideoFrameToAdd->getValue());
    }
    if (videoFrameSelectionList->hasSelectedOption()) {
        if (ImGui::Button("Remove selected video frame")) {
            removeVideoFrame(videoSelectionList->getSelectedOption(), videoFrameSelectionList->getSelectedIndex());
        }
    }
    videoFrameSelectionList->showGui();
    if (videoFrameSelectionList->changedButNotMarkedAsDealtWith()) {
        goToSelectedVideoFrame();
        videoFrameSelectionList->markAsDealtWithChange();
    }
}

void CaptureHandler::goToSelectedVideoFrame()
{
    if (!videoSelectionList->hasSelectedOption()) return;
    if (!videoSelectionList->hasSelectedOption()) return;
    const std::string videoFilename = videoSelectionList->getSelectedOption();
    const int selectedIndex = videoFrameSelectionList->getSelectedIndex();
    VideoMetadata* videoMetadata = getOrCreateVideoMetadata(videoFilename);
    const VideoMetadata::FrameInfo& frameInfoToGoTo = videoMetadata->frameInfos[selectedIndex];
    setCurrentFramePositionOfVideoBeingPlayed(frameInfoToGoTo.framePosition);
}

void CaptureHandler::addCurrentVideoFrame(const std::string& videoFilename, const std::string& description)
{
    VideoMetadata* videoMetadata = getOrCreateVideoMetadata(videoFilename);
    videoMetadata->addVideoFrameIfUnexisting(videoFilename, currentFramePositionInVideoBeingPlayed, description);

    updateVideoFrameSelectionList(videoFilename);
}

void CaptureHandler::removeVideoFrame(const std::string& videoFilename, const size_t index)
{
    VideoMetadata* videoMetadata = getOrCreateVideoMetadata(videoFilename);
    videoMetadata->removeVideoFrame(index);
    updateVideoFrameSelectionList(videoFilename);
}

void CaptureHandler::updateVideoFrameSelectionList(const std::string& videoFilename)
{
    std::vector<std::string> options;

    const VideoMetadata* videoMetadata = getOrCreateVideoMetadata(videoFilename);
    for (const VideoMetadata::FrameInfo& frameInfo: videoMetadata->frameInfos) {
        const std::string framePositionString = std::to_string(frameInfo.framePosition);
        std::string option = framePositionString;
        if (!frameInfo.description.empty()) {
            option += " - " + frameInfo.description;
        }
        options.push_back(option);
    }

    videoFrameSelectionList->setOptions(options);
    videoFrameSelectionList->setSelectionToNone();
}

CaptureHandler::VideoMetadata* CaptureHandler::getOrCreateVideoMetadata(const std::string& videoFilename)
{
    auto it = mapFromVideoFilenameToVideoMetadata.find(videoFilename);
    if (it != mapFromVideoFilenameToVideoMetadata.end()) {
        return it->second;
    }
    VideoMetadata* newVideoMetadata = new VideoMetadata;
    mapFromVideoFilenameToVideoMetadata[videoFilename] = newVideoMetadata;
    return newVideoMetadata;
}

void CaptureHandler::VideoMetadata::addVideoFrameIfUnexisting(const std::string& videoFilename, const int framePosition, const std::string& description)
{
    if (hasFrameInfoWithFramePosition(framePosition)) return;

    VideoMetadata::FrameInfo frameInfo;
    frameInfo.description = description;
    frameInfo.framePosition = framePosition;

    frameInfos.push_back(frameInfo);

    // sorting in ascending order (based on frame position)
    std::sort(frameInfos.begin(), frameInfos.end(), [](const VideoMetadata::FrameInfo& a, const VideoMetadata::FrameInfo& b) -> bool {
        return a.framePosition < b.framePosition;
    });
}

void CaptureHandler::VideoMetadata::removeVideoFrame(const size_t index)
{
    if (index >= frameInfos.size()) return;
    frameInfos.erase(frameInfos.begin() + index);
}

bool CaptureHandler::VideoMetadata::hasFrameInfoWithFramePosition(const int framePosition) const
{
    for (const FrameInfo& frameInfo : frameInfos) {
        if (frameInfo.framePosition == framePosition) return true;
    }
    return false;
}

void CaptureHandler::VideoMetadata::saveToBinaryFile(FILE* f) const
{
    const size_t numberOfFrameInfos = frameInfos.size();
    fwrite(&numberOfFrameInfos, sizeof(int), 1, f);
    for (const FrameInfo& frameInfo : frameInfos) {
        StringUtils::saveToBinaryFile(frameInfo.description, f);
        fwrite(&frameInfo.framePosition, sizeof(int), 1, f);
    }
}

void CaptureHandler::VideoMetadata::loadFromBinaryFile(FILE* f)
{
    size_t numberOfFrameInfos = 0;
    fread(&numberOfFrameInfos, sizeof(int), 1, f);
    for (size_t i = 0; i < numberOfFrameInfos; ++i) {
        frameInfos.push_back(FrameInfo());
        FrameInfo& frameInfo = frameInfos.back();

        frameInfo.description = StringUtils::loadFromBinaryFile(f);
        fread(&frameInfo.framePosition, sizeof(int), 1, f);
    }
}

bool CaptureHandler::shouldGrabDesktop() const
{
    return grabDesktopInstead->getValue();
}

void CaptureHandler::grabDesktopFrame()
{
    desktopFrameGrabber->grabDesktopFrame(*frame);

    if (!frame->hasValidDimensions()) {
        setFrameToBlackFrame();
    }
}

bool CaptureHandler::screenshotHotkeyPressed() const
{
    std::set<KeyboardAndMouseDefs::Key> hotkey;
    hotkey.insert(KeyboardAndMouseDefs::Key::Ctrl);
    hotkey.insert(KeyboardAndMouseDefs::Key::Shift);
    hotkey.insert(KeyboardAndMouseDefs::Key::Q);

    return travailleurContext->getHotkeyHandler()->hotkeyPressed(hotkey);
}