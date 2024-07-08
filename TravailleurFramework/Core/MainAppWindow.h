#ifndef MainAppWindow_H
#define MainAppWindow_H

#include <memory>

#include "Common/Image.h"
#include "ImGuiWrappers/ImGuiCheckbox.h"
#include "ImGuiWrappers/ImGuiIntSlider.h"
#include "ImGuiWrappers/ImGuiImageRenderer.h"

class RectangularRegion;
class TravailleurContext;
class TravailleurApp;

class MainAppWindow
{
public:
    MainAppWindow(TravailleurContext* travailleurContext);
    ~MainAppWindow();

    void showGui();

private:
    size_t captureWidth() const;
    size_t captureHeight() const;

    Image* getFrameToDisplay() const;
    void displayFrame();

    TravailleurApp* getAppInExecution() const;

    std::unique_ptr<ImGuiCheckbox> showOnlyFramesBeingProcessed;
    std::unique_ptr<ImGuiIntSlider> frameDisplayResizeWidthSlider;
    std::unique_ptr<ImGuiIntSlider> frameDisplayResizeHeightSlider;
    std::unique_ptr<ImGuiCheckbox> resizeFrameCheckbox;
    std::unique_ptr<ImGuiCheckbox> showFrameOverlays;
    ImGuiImageRenderer* imGuiImageRenderer = nullptr;
   
    TravailleurContext* travailleurContext = nullptr;
};

#endif