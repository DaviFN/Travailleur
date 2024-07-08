#include "MainAppWindow.h"

#include "imgui.h"

#include "Common/TravailleurUtils.h"
#include "ImGuiWrappers/ImGuiTextOutput.h"
#include "TravailleurContext.h"

MainAppWindow::MainAppWindow(TravailleurContext* travailleurContext):
    travailleurContext(travailleurContext),
    imGuiImageRenderer(new ImGuiImageRenderer())
{
    showOnlyFramesBeingProcessed = std::make_unique<ImGuiCheckbox>("Show only frames being processed", true, TravailleurUtils::getConfigFilenameFor("show-only-frames-being-processed", travailleurContext->getName()));

    resizeFrameCheckbox = std::make_unique<ImGuiCheckbox>("Resize frame", true, TravailleurUtils::getConfigFilenameFor("resize-frame-mainappwindow", travailleurContext->getName()));
    frameDisplayResizeWidthSlider = std::make_unique<ImGuiIntSlider>("width", captureWidth(), captureWidth() / 10, captureWidth(), 1.0, TravailleurUtils::getConfigFilenameFor("frame-display-resize-width-mainappwindow", travailleurContext->getName()));
    frameDisplayResizeHeightSlider = std::make_unique<ImGuiIntSlider>("height", captureHeight(), captureHeight() / 10, captureHeight(), 1.0, TravailleurUtils::getConfigFilenameFor("frame-display-resize-height-mainappwindow", travailleurContext->getName()));

    showFrameOverlays = std::make_unique<ImGuiCheckbox>("Show frame overlays", true, TravailleurUtils::getConfigFilenameFor("show-frame-overlays-mainappwindow", travailleurContext->getName()));
}

MainAppWindow::~MainAppWindow()
{
    delete imGuiImageRenderer;
}

size_t MainAppWindow::captureWidth() const
{
    return CaptureHandler::captureWidth();
}

size_t MainAppWindow::captureHeight() const
{
    return CaptureHandler::captureHeight();
}

Image* MainAppWindow::getFrameToDisplay() const
{
    TravailleurAppsHandler* travailleurAppsHandler = travailleurContext->getTravailleurAppsHandler();
    if (travailleurAppsHandler->isExecutingApp()) {
        if (showOnlyFramesBeingProcessed->getValue()) {
            return travailleurAppsHandler->getFrameBeingProcessed();
        }
        return travailleurContext->getCaptureHandler()->getFrame();
    }
    return nullptr;
}

void MainAppWindow::displayFrame()
{
    Image* frameToDisplay = getFrameToDisplay();

    if (frameToDisplay != nullptr) {
        std::unique_ptr<Image> frameToRender = std::make_unique<Image>(*frameToDisplay);

        if (showFrameOverlays->getValue()) {
            Image* copyOfFrameOverlays = travailleurContext->getTravailleurAppsHandler()->getCopyOfFrameOverlays();
            frameToRender->drawMask(*copyOfFrameOverlays);
            delete copyOfFrameOverlays;
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

TravailleurApp* MainAppWindow::getAppInExecution() const
{
    return travailleurContext->getTravailleurAppsHandler()->getAppInExecution();
}

void MainAppWindow::showGui()
{
    ImGui::Begin("MainAppWindow");

    TravailleurApp* appInExecution = getAppInExecution();
    const std::string appInExecutionLabel = appInExecution != nullptr ? appInExecution->getName() : std::string("none");
    ImGuiTextOutput("app in execution: " + appInExecutionLabel);

    displayFrame();

    showOnlyFramesBeingProcessed->showGui();

    resizeFrameCheckbox->showGui();
    if (resizeFrameCheckbox->getValue()) {
        frameDisplayResizeWidthSlider->showGui();
        frameDisplayResizeHeightSlider->showGui();
    }

    showFrameOverlays->showGui();


    ImGui::End();
}