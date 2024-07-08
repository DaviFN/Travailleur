#include "TravailleurApp.h"

#include <future>

#include "ImGuiWrappers/include_all_headers.h"

#include "Common/TravailleurUtils.h"
#include "TravailleurContext.h"

TravailleurApp::TravailleurApp()
{
}

TravailleurApp::~TravailleurApp()
{
}

void TravailleurApp::assureDirectoriesExist() const
{
	FilesystemUtils::assureDirectoryExists(getFramesConfigPath());
}

void TravailleurApp::afterSetup()
{
	baseLoad();
}

void TravailleurApp::baseLoad()
{
	for (auto& mapItem : appFrameEnabledMap) {
		SimplePersistenceUtils::loadBool(mapItem.first, mapItem.second, getFramesConfigPath());
	}
	load();
}

void TravailleurApp::baseSave() const
{
	assureDirectoriesExist();
	for (auto& mapItem : appFrameEnabledMap) {
		SimplePersistenceUtils::saveBool(mapItem.first, mapItem.second, getFramesConfigPath());
	}
	save();
}

void TravailleurApp::load()
{
}

void TravailleurApp::save() const
{
}

void TravailleurApp::beforeLaunch()
{
}

void TravailleurApp::afterExecutionStopped()
{
}

void TravailleurApp::beforeProcessFrame()
{
	asyncVerificationDatas.clear();
}

void TravailleurApp::registerPointInput(const std::string& name)
{
	travailleurContext->registerPointInput(name);
}

void TravailleurApp::registerRectangularRegion(const std::string& name)
{
	travailleurContext->registerRectangularRegion(name);
}

void TravailleurApp::registerImagePattern(const std::string& name)
{
	travailleurContext->registerImagePattern(name);
}

void TravailleurApp::registerImagePreprocessing(const std::string& name)
{
	travailleurContext->registerImagePreprocessing(name);
}

void TravailleurApp::registerImageOCR(const std::string& name)
{
	travailleurContext->registerImageOCR(name);
}

void TravailleurApp::registerImageResource(const std::string& name)
{
	travailleurContext->registerImageResource(name);
}

void TravailleurApp::insertFrameRectangularOverlay(RectangularRegion* rectangularRegion, const Pixel& color)
{
	travailleurContext->getTravailleurAppsHandler()->insertFrameRectangularOverlay(rectangularRegion, color);
}

void TravailleurApp::insertFrameImageOverlay(ImageResource* imageResource)
{
	travailleurContext->getTravailleurAppsHandler()->insertFrameImageOverlay(imageResource);
}

bool TravailleurApp::hasImagePattern(const std::string& imagePatternName, const std::string& rectangularRegionName)
{
	RectangularRegion* rectangularRegion = travailleurContext->getRectangularRegion(rectangularRegionName);
	assert(rectangularRegion != nullptr);
	ImagePattern* imagePattern = travailleurContext->getImagePattern(imagePatternName);
	assert(imagePattern != nullptr);

	insertFrameRectangularOverlay(rectangularRegion, ImagePatternsHandler::characteristicColor());

	RectangularRegionsHandler* rectangularRegionsHandler = travailleurContext->getRectangularRegionsHandler();
	const Point2D<int> searchRegionTopLeft = rectangularRegionsHandler->determineTopLeftConsideringParents(getFrameBeingProcessed(), rectangularRegion);
	const Rectangle2D<int> searchRegion(searchRegionTopLeft, rectangularRegion->getSize());

	return imagePattern->occursIn(getFrameBeingProcessed(), searchRegion);
}

std::string TravailleurApp::performImageOcr(const std::string& imageOcrName, const std::string& rectangularRegionName)
{
	RectangularRegion* rectangularRegion = travailleurContext->getRectangularRegion(rectangularRegionName);
	assert(rectangularRegion != nullptr);
	ImageOCR* imageOcr = travailleurContext->getImageOCR(imageOcrName);
	assert(imageOcr != nullptr);

	insertFrameRectangularOverlay(rectangularRegion, ImageOCRsHandler::characteristicColor());

	RectangularRegionsHandler* rectangularRegionsHandler = travailleurContext->getRectangularRegionsHandler();
	const Point2D<int> searchRegionTopLeft = rectangularRegionsHandler->determineTopLeftConsideringParents(getFrameBeingProcessed(), rectangularRegion);
	const Rectangle2D<int> searchRegion(searchRegionTopLeft, rectangularRegion->getSize());

	return imageOcr->performOCROnRegion(getFrameBeingProcessed(), searchRegion);
}

void TravailleurApp::pushAsyncHasImagePattern(bool* result, const std::string& imagePatternName, const std::string& rectangularRegionName)
{
	assert(result != nullptr);

	AsyncVerificationData data;
	data.type = VerificationType::Type_HasImagePattern;
	data.result_hasImagePattern = result;
	data.result_performImageOcr = nullptr;
	data.imagePatternName = imagePatternName;
	data.rectangularRegionName = rectangularRegionName;

	asyncVerificationDatas.push_back(data);
}

void TravailleurApp::pushAsyncPerformImageOcr(std::string* result, const std::string& imageOcrName, const std::string& rectangularRegionName)
{
	assert(result != nullptr);

	AsyncVerificationData data;
	data.type = VerificationType::Type_PerformImageOcr;
	data.result_hasImagePattern = nullptr;
	data.result_performImageOcr = result;
	data.imageOcrName = imageOcrName;
	data.rectangularRegionName = rectangularRegionName;

	asyncVerificationDatas.push_back(data);
}

void TravailleurApp::runAsyncReadings()
{
	if (asyncVerificationDatas.empty()) return;

	std::vector<std::future<void>> futures(asyncVerificationDatas.size());

	for (size_t i = 0; i < asyncVerificationDatas.size(); ++i) {
		futures[i] = std::async(&TravailleurApp::runAsyncReading, this, asyncVerificationDatas[i]);
	}

	for (auto& future : futures) {
		future.wait();
	}

	asyncVerificationDatas.clear();
}

void TravailleurApp::runAsyncReading(const AsyncVerificationData& data)
{
	if (data.type == VerificationType::Type_HasImagePattern) {
		*data.result_hasImagePattern = hasImagePattern(data.imagePatternName, data.rectangularRegionName);
	}
	else if (data.type == VerificationType::Type_PerformImageOcr) {
		*data.result_performImageOcr = performImageOcr(data.imageOcrName, data.rectangularRegionName);
	}
}

bool TravailleurApp::rectangularRegionIsValidOnFrame(const std::string& rectangularRegionName) const
{
	assert(travailleurContext->getTravailleurAppsHandler()->currentThreadIsAppExecutionThread());

	RectangularRegion* rectangularRegion = travailleurContext->getRectangularRegion(rectangularRegionName);
	assert(rectangularRegion != nullptr);

	return travailleurContext->getRectangularRegionsHandler()->rectangularRegionIsValidOnFrame(getFrameBeingProcessed(), rectangularRegion);
}

void TravailleurApp::showImageResource(const std::string& imageResourceName)
{
	assert(travailleurContext->getTravailleurAppsHandler()->currentThreadIsAppExecutionThread());

	ImageResource* imageResource = travailleurContext->getImageResource(imageResourceName);
	assert(imageResource != nullptr);

	insertFrameImageOverlay(imageResource);
}

void TravailleurApp::showNormalMessage(const std::string& message)
{
	assert(travailleurContext->getTravailleurAppsHandler()->currentThreadIsAppExecutionThread());
	travailleurContext->getTravailleurConsole()->addNormalMessage(message);
}

void TravailleurApp::showErrorMessage(const std::string& message)
{
	assert(travailleurContext->getTravailleurAppsHandler()->currentThreadIsAppExecutionThread());
	travailleurContext->getTravailleurConsole()->addErrorMessage(message);
}

void TravailleurApp::showWarningMessage(const std::string& message)
{
	assert(travailleurContext->getTravailleurAppsHandler()->currentThreadIsAppExecutionThread());
	travailleurContext->getTravailleurConsole()->addWarningMessage(message);
}

void TravailleurApp::showInfoMessage(const std::string& message)
{
	assert(travailleurContext->getTravailleurAppsHandler()->currentThreadIsAppExecutionThread());
	travailleurContext->getTravailleurConsole()->addInfoMessage(message);
}

void TravailleurApp::showSuccessMessage(const std::string& message)
{
	assert(travailleurContext->getTravailleurAppsHandler()->currentThreadIsAppExecutionThread());
	travailleurContext->getTravailleurConsole()->addSuccessMessage(message);
}

void TravailleurApp::showDebugMessage(const std::string& message)
{
	assert(travailleurContext->getTravailleurAppsHandler()->currentThreadIsAppExecutionThread());
	travailleurContext->getTravailleurConsole()->addDebugMessage(message);
}

std::string TravailleurApp::getAppPath() const
{
	return TravailleurUtils::getAppsPath(travailleurContext->getName()) + "\\" + getName();
}

std::string TravailleurApp::getFramesConfigPath() const
{
	return getAppPath() + "\\frames-config";
}

TravailleurContext* TravailleurApp::getContext() const
{
	return travailleurContext;
}

void TravailleurApp::addAppFramesDisableOption(const std::string& frameName)
{
	appFrameEnabledMap[frameName] = false;
}

bool TravailleurApp::frameIsEnabledByUser(const std::string& frameName) const
{
	auto it = appFrameEnabledMap.find(frameName);
	if (it == appFrameEnabledMap.end()) {
		return false;
	}
	return it->second;
}

void TravailleurApp::showAppFramesInMainMenu()
{
	for (auto& mapItem : appFrameEnabledMap) {
		ImGui::Checkbox(mapItem.first.c_str(), &mapItem.second);
	}
}

Image* TravailleurApp::getFrameBeingProcessed() const
{
	return travailleurContext->getTravailleurAppsHandler()->getFrameBeingProcessed();
}

void TravailleurApp::pressKey(const KeyboardAndMouseDefs::Key key, const int pressTimeInMs)
{
	travailleurContext->pressKey(key, pressTimeInMs);
}

void TravailleurApp::pressLeftMouseButton(const int pressTimeInMs)
{
	travailleurContext->pressLeftMouseButton(pressTimeInMs);
}

void TravailleurApp::pressRightMouseButton(const int pressTimeInMs)
{
	travailleurContext->pressRightMouseButton(pressTimeInMs);
}

void TravailleurApp::moveMouse(const int x, const int y)
{
	travailleurContext->moveMouse(Point2D<int>(x, y));
}

void TravailleurApp::moveMouse(const std::string& pointInputName)
{
	travailleurContext->moveMouse(pointInputName);
}