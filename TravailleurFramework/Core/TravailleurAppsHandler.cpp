#include "TravailleurAppsHandler.h"

#include "imgui.h"

#include "Common/TravailleurUtils.h"
#include "Common/SimplePersistenceUtils.h"
#include "ImGuiWrappers/ImGuiImageRenderer.h"
#include "ImGuiWrappers/ImGuiTextOutput.h"
#include "TravailleurApp.h"
#include "TravailleurContext.h"

TravailleurAppsHandler::TravailleurAppsHandler(TravailleurContext* travailleurContext, const std::vector<TravailleurApp*>& travailleurApps
) :
	travailleurContext(travailleurContext),
	travailleurApps(travailleurApps)
{
	for (TravailleurApp* travailleurApp : travailleurApps) {
		travailleurApp->travailleurContext = travailleurContext;
		travailleurApp->setup();
		travailleurApp->afterSetup();
	}

	travailleurAppsSelector = std::make_unique<ImGuiSingleSelectionList>("travailleur-apps-selector");
	initializeTravailleurAppsSelector();

	frameOverlays = new Image(captureWidth(), captureHeight());

	delayInMsBetweenFrameProcessings = std::make_unique<ImGuiIntSlider>("delayInMsBetweenFrameProcessings", 1, 1, 1000, 1.0, TravailleurUtils::getConfigFilenameFor("delay-in-ms-between-frame-processings", travailleurContext->getName()));

	displayCounterEvennessRepresentation = std::make_unique<ImGuiCheckbox>("display counter evenness representation", false);
	processedFrameCounterEvennessImageRenderer = std::make_unique<ImGuiImageRenderer>();
	constexpr int processedFrameCounterEvennessImageWidth = 10;
	constexpr int processedFrameCounterEvennessImageHeight = 10;
	evenProcessedFrameCounterImage = std::make_unique<Image>(processedFrameCounterEvennessImageWidth, processedFrameCounterEvennessImageHeight);
	evenProcessedFrameCounterImage->fillWithSamePixel(Pixel(235, 170, 50));
	oddProcessedFrameCounterImage = std::make_unique<Image>(processedFrameCounterEvennessImageWidth, processedFrameCounterEvennessImageHeight);
	oddProcessedFrameCounterImage->fillWithSamePixel(Pixel(0, 170, 50));

	showTravailleurAppPrerunGui = std::make_unique<ImGuiCheckbox>("show app's prerun gui", false, TravailleurUtils::getConfigFilenameFor("show-app-prerun-gui", travailleurContext->getName()));

	load();
}

TravailleurAppsHandler::~TravailleurAppsHandler()
{
	save();

	assureAppExecutionThreadIsNotRunning();

	for (TravailleurApp* travailleurApp : travailleurApps) {
		travailleurApp->baseSave();
	}

	delete frameBeingProcessed;
	delete frameOverlays;
}

void TravailleurAppsHandler::save() const
{
	saveSelectedTravailleurApp();
}

void TravailleurAppsHandler::saveSelectedTravailleurApp() const
{
	TravailleurApp* selectedTravailleurApp = getSelectedTravailleurApp();
	if (selectedTravailleurApp != nullptr) {
		const std::string selectedTravailleurAppName = selectedTravailleurApp->getName();
		SimplePersistenceUtils::saveString("selected-travailleur-app-name", selectedTravailleurAppName, TravailleurUtils::getConfigPath(travailleurContext->getName()));
	}
}

void TravailleurAppsHandler::load()
{
	loadSelectedTravailleurApp();
}

void TravailleurAppsHandler::loadSelectedTravailleurApp()
{
	std::string selectedTravailleurAppName;
	SimplePersistenceUtils::loadString("selected-travailleur-app-name", selectedTravailleurAppName, TravailleurUtils::getConfigPath(travailleurContext->getName()));
	TravailleurApp* savedSelectedTravailleurApp = getTravailleurAppByName(selectedTravailleurAppName);
	if (savedSelectedTravailleurApp != nullptr) {
		setSelectedTravailleurApp(savedSelectedTravailleurApp);
	}
}

size_t TravailleurAppsHandler::captureWidth() const
{
	return CaptureHandler::captureWidth();
}

size_t TravailleurAppsHandler::captureHeight() const
{
	return CaptureHandler::captureHeight();
}

void TravailleurAppsHandler::clearFrameOverlays()
{
	if (frameBeingProcessed != nullptr) {
		frameOverlays->setToEmptyMaskForImage(*frameBeingProcessed);
	}
}

void TravailleurAppsHandler::initializeTravailleurAppsSelector()
{
	std::vector<std::string> travailleurAppsNames;
	for (TravailleurApp* travailleurApp : travailleurApps) {
		travailleurAppsNames.push_back(travailleurApp->getName());
	}
	travailleurAppsSelector->setOptions(travailleurAppsNames);
}

TravailleurApp* TravailleurAppsHandler::getTravailleurAppByName(const std::string& travailleurAppName) const
{
	for (TravailleurApp* travailleurApp : travailleurApps) {
		if (travailleurApp->getName() == travailleurAppName) {
			return travailleurApp;
		}
	}
	return nullptr;
}

TravailleurApp* TravailleurAppsHandler::getSelectedTravailleurApp() const
{
	if (!travailleurAppsSelector->hasSelectedOption()) {
		return nullptr;
	}
	return getTravailleurAppByName(travailleurAppsSelector->getSelectedOption());
}

void TravailleurAppsHandler::setSelectedTravailleurApp(TravailleurApp* travailleurApp)
{
	if (travailleurApp == nullptr) return;
	const std::string travailleurAppName = travailleurApp->getName();
	travailleurAppsSelector->setSelectedOption(travailleurAppName);
}

void TravailleurAppsHandler::showGui()
{
	ImGui::Begin("TravailleurAppsHandler");

	if (!isExecutingApp()) {
		TravailleurApp* selectedTravailleurApp = getSelectedTravailleurApp();

		travailleurAppsSelector->showGui();

		if (selectedTravailleurApp != nullptr) {
			if (selectedTravailleurApp->canBeLaunched()) {
				if (ImGui::Button("Execute app")) {
					beginAppExecution();
				}
			}
			else {
				ImGuiTextOutput("this app cannot be currently launched");
			}
		}
		else {
			ImGuiTextOutput("no app selected");
		}

		showTravailleurAppPrerunGui->showGui();
		if(showTravailleurAppPrerunGui->getValue()) {
			if (selectedTravailleurApp != nullptr) {
				selectedTravailleurApp->showPreRunGui();
			}
		}
	}
	else {
		ImGuiTextOutput("executing app: " + getAppInExecution()->getName());

		if (userWantsToStopAppExecution) {
			ImGuiTextOutput("stopping app's execution...");
		}
		else {
			if (ImGui::Button("Stop execution")) {
				userWantsToStopAppExecution = true;
			}
			ImGuiTextOutput("executing for: " + appExecutionChronometer.TimeElapsedAsString());
			showProcessedFramesVisualizationGui();
		}
	}

	delayInMsBetweenFrameProcessings->showGui();

	ImGui::End();
}

void TravailleurAppsHandler::showProcessedFramesVisualizationGui()
{
	ImGuiTextOutput("processedFrameCounter: " + std::to_string(processedFrameCounter));

	displayCounterEvennessRepresentation->showGui();
	if (displayCounterEvennessRepresentation->getValue()) {
		if (processedFrameCounter % 2 == 0) {
			processedFrameCounterEvennessImageRenderer->renderImage(*evenProcessedFrameCounterImage);
		}
		else {
			processedFrameCounterEvennessImageRenderer->renderImage(*oddProcessedFrameCounterImage);
		}
	}
}

TravailleurApp* TravailleurAppsHandler::getAppInExecution() const
{
	return appInExecution;
}

bool TravailleurAppsHandler::isExecutingApp() const
{
	return getAppInExecution() != nullptr;
}

void TravailleurAppsHandler::beginAppExecution()
{
	appExecutionChronometer.restart();
	processedFrameCounter = 0;

	appInExecution = getSelectedTravailleurApp();
	if (appInExecution != nullptr) {
		appInExecution->beforeLaunch();

		if (appExecutionThreadObject != nullptr) {
			appExecutionThreadObject->join();
		}
		appExecutionThreadObject = std::make_unique<std::thread>(&TravailleurAppsHandler::appExecutionThread, this);
	}
}

void TravailleurAppsHandler::feedNewFrameIfNeeded(Image* frame)
{
	if (needsNewFrame) {
		delete frameBeingProcessed;
		frameBeingProcessed = new Image(*frame);
		++processedFrameCounter;
		clearFrameOverlays();

		needsNewFrame = false;
		needsNewFrame.notify_one();
	}
}

void TravailleurAppsHandler::appExecutionThread()
{
	appThreadInExecution = true;

	needsNewFrame = true;
	needsNewFrame.wait(true);

	while (!userWantsToStopAppExecution) {
		getAppInExecution()->beforeProcessFrame();
		getAppInExecution()->processFrame();

		needsNewFrame = true;
		needsNewFrame.wait(true);

		std::this_thread::sleep_for(std::chrono::milliseconds(delayInMsBetweenFrameProcessings->getValue()));
	}

	getAppInExecution()->afterExecutionStopped();

	userWantsToStopAppExecution = false;

	mtx.lock();
	appInExecution = nullptr;
	mtx.unlock();

	delete frameBeingProcessed;
	frameBeingProcessed = nullptr;
	clearFrameOverlays();

	appThreadInExecution = false;
}

void TravailleurAppsHandler::update()
{
	mtx.lock();
	mtx.unlock();
}

Image* TravailleurAppsHandler::getFrameBeingProcessed() const
{
	return frameBeingProcessed;
}

void TravailleurAppsHandler::showGuiForAppInExecution()
{
	mtx.lock();

	if (appInExecution != nullptr) {
		appInExecution->showRunningGui();
	}

	mtx.unlock();
}

// executes in main thread
void TravailleurAppsHandler::showAppFramesInMainMenu()
{
	if (ImGui::BeginMenu("App frames"))
	{
		mtx.lock();
		if (appInExecution != nullptr) {
			appInExecution->showAppFramesInMainMenu();
		}
		mtx.unlock();

		ImGui::EndMenu();
	}
}

// executes in main thread
Image* TravailleurAppsHandler::getCopyOfFrameOverlays() const
{
	mtx.lock();

	Image* copyOfFrameOverlays = new Image(*frameOverlays);
	
	mtx.unlock();

	return copyOfFrameOverlays;
}

// executes in app thread
void TravailleurAppsHandler::insertFrameRectangularOverlay(RectangularRegion* rectangularRegion, const Pixel& color)
{
	mtx.lock();
	
	const Point2D<int> topLeft = travailleurContext->getRectangularRegionsHandler()->determineTopLeftConsideringParents(frameBeingProcessed, rectangularRegion);
	const Point2D<int> size = rectangularRegion->getSize();
	frameOverlays->highlightRectangularRegion(topLeft, size, 2, color);
	
	mtx.unlock();
}

// executes in app thread
void TravailleurAppsHandler::insertFrameImageOverlay(ImageResource* imageResource)
{
	mtx.lock();

	assert(currentThreadIsAppExecutionThread());

	imageResource->renderImage(frameBeingProcessed, frameOverlays);

	mtx.unlock();
}

// executes in main thread
void TravailleurAppsHandler::assureAppExecutionThreadIsNotRunning()
{
	userWantsToStopAppExecution = true;

	while (appThreadInExecution) {
		needsNewFrame = false;
		needsNewFrame.notify_one();
	}

	if (appExecutionThreadObject != nullptr) {
		appExecutionThreadObject->join();
	}
}

bool TravailleurAppsHandler::currentThreadIsAppExecutionThread() const
{
	return std::this_thread::get_id() == appExecutionThreadObject->get_id();

}