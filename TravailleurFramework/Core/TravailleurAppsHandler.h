#ifndef TravailleurAppsHandler_H
#define TravailleurAppsHandler_H

#include <atomic>
#include <memory>
#include <thread>
#include <vector>

#include "Common/Chronometer.h"
#include "Common/Image.h"
#include "ImGuiWrappers/ImGuiCheckbox.h"
#include "ImGuiWrappers/ImGuiImageRenderer.h"
#include "ImGuiWrappers/ImGuiIntSlider.h"
#include "ImGuiWrappers/ImGuiSingleSelectionList.h"

class ImageResource;
class RectangularRegion;
class TravailleurApp;
class TravailleurContext;

class TravailleurAppsHandler
{
public:
    TravailleurAppsHandler(TravailleurContext* travailleurContext, const std::vector<TravailleurApp*>& travailleurApps);
    ~TravailleurAppsHandler();
    void showGui();

    void feedNewFrameIfNeeded(Image* frame);

    void update();

    TravailleurApp* getAppInExecution() const;

    bool isExecutingApp() const;

    Image* getFrameBeingProcessed() const;

    void showGuiForAppInExecution();

    void showAppFramesInMainMenu();

    Image* getCopyOfFrameOverlays() const;

    void insertFrameRectangularOverlay(RectangularRegion* rectangularRegion, const Pixel& color);

    void insertFrameImageOverlay(ImageResource* imageResource);

    bool currentThreadIsAppExecutionThread() const;

private:
    TravailleurContext* travailleurContext = nullptr;
    std::vector<TravailleurApp*> travailleurApps;
    TravailleurApp* appInExecution = nullptr;

    void save() const;
    void load();
    void saveSelectedTravailleurApp() const;
    void loadSelectedTravailleurApp();

    size_t captureWidth() const;
    size_t captureHeight() const;

    void clearFrameOverlays();

    void showProcessedFramesVisualizationGui();

    void initializeTravailleurAppsSelector();
    TravailleurApp* getTravailleurAppByName(const std::string& travailleurAppName) const;
    TravailleurApp* getSelectedTravailleurApp() const;
    void setSelectedTravailleurApp(TravailleurApp* travailleurApp);

    void beginAppExecution();
    bool userWantsToStopAppExecution = false;
    void appExecutionThread();

    void assureAppExecutionThreadIsNotRunning();

    std::unique_ptr<ImGuiSingleSelectionList> travailleurAppsSelector;
    std::unique_ptr<std::thread> appExecutionThreadObject;

    std::atomic<bool> needsNewFrame = false;
    Image* frameBeingProcessed = nullptr;
    Image* frameOverlays = nullptr;

    std::unique_ptr<ImGuiIntSlider> delayInMsBetweenFrameProcessings;

    mutable std::mutex mtx;
    bool appThreadInExecution = false;

    Chronometer appExecutionChronometer;
    size_t processedFrameCounter = 0;
    std::unique_ptr<ImGuiCheckbox> displayCounterEvennessRepresentation;
    std::unique_ptr<ImGuiImageRenderer> processedFrameCounterEvennessImageRenderer;
    std::unique_ptr<Image> evenProcessedFrameCounterImage;
    std::unique_ptr<Image> oddProcessedFrameCounterImage;

    std::unique_ptr<ImGuiCheckbox> showTravailleurAppPrerunGui;
};

#endif