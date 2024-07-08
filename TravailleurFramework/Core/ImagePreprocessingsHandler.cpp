#include "ImagePreprocessingsHandler.h"

#include "imgui.h"

#include "Common/Chronometer.h"
#include "Common/Point2DPersistenceUtils.h"
#include "Common/Rectangle2DPersistenceUtils.h"
#include "Common/TravailleurUtils.h"
#include "ImagePreprocessingUtils.h"
#include "ImGuiWrappers/ImGuiSimpleColorSelector.h"
#include "ImGuiWrappers/ImGuiColorOutput.h"
#include "ImGuiWrappers/ImGuiSimpleIntRectangleSlider.h"
#include "ImGuiWrappers/ImGuiTextOutput.h"
#include "TravailleurContext.h"

ImagePreprocessingsHandler::ImagePreprocessingsHandler(TravailleurContext* travailleurContext):
	travailleurContext(travailleurContext)
{
	allowFrameOverlays = std::make_unique<ImGuiCheckbox>("Allow frame overlays", true, TravailleurUtils::getConfigFilenameFor("allow-image-preprocessings-handler-show-overlays", travailleurContext->getName()));

	imagePreprocessingsNames = std::make_unique<ImGuiSingleSelectionList>("image-preprocessings-names");

	showPreprocessingTestRegionCheckbox = std::make_unique<ImGuiCheckbox>("Show preprocessing test region", true, TravailleurUtils::getConfigFilenameFor("show-preprocessing-test-region", travailleurContext->getName()));

	originalTestImageRenderer = std::make_unique<ImGuiImageRenderer>();
	preprocessedTestImageRenderer = std::make_unique<ImGuiImageRenderer>();

	showOriginalTestImageCheckbox = std::make_unique<ImGuiCheckbox>("Show original test image", true, TravailleurUtils::getConfigFilenameFor("show-original-test-image-region", travailleurContext->getName()));

	showPreprocessedTestImageCheckbox = std::make_unique<ImGuiCheckbox>("Show preprocessed test image", true, TravailleurUtils::getConfigFilenameFor("show-preprocessed-test-image-region", travailleurContext->getName()));

	useRectangularRegionAsPreprocessingTestRegion = std::make_unique<ImGuiCheckbox>("Use rectangular region as preprocessing test region", false, TravailleurUtils::getConfigFilenameFor("use-rectangular-region-as-preprocessing-test-region", travailleurContext->getName()));
	rectangularRegionsNamesList_preprocessingTestRegion = std::make_unique<ImGuiSingleSelectionList>("rectangular-regions-names-list-preprocessing-test-region");

	load();
}

ImagePreprocessingsHandler::~ImagePreprocessingsHandler()
{
	save();
	saveSelectedImagePreprocessing();
	saveRectangularRegionsNamesListSelection_preprocessingTestRegion();

	for (auto& mapItem : mapFromNameToImagePreprocessing) {
		delete mapItem.second;
	}
}

void ImagePreprocessingsHandler::save() const
{
	const std::string preprocessingTestRegionFrameFilepath = TravailleurUtils::getConfigFilenameFor("preprocessing-test-region", travailleurContext->getName());
	Rectangle2DPersistenceUtils::saveIntRectangle2D(FilesystemUtils::filenameFromFilepath(preprocessingTestRegionFrameFilepath), preprocessingTestRegion, FilesystemUtils::parentDirectoryOfFilepath(preprocessingTestRegionFrameFilepath));
}

void ImagePreprocessingsHandler::load()
{
	const std::string preprocessingTestRegionFrameFilepath = TravailleurUtils::getConfigFilenameFor("preprocessing-test-region", travailleurContext->getName());
	Rectangle2DPersistenceUtils::loadIntRectangle2D(FilesystemUtils::filenameFromFilepath(preprocessingTestRegionFrameFilepath), preprocessingTestRegion, FilesystemUtils::parentDirectoryOfFilepath(preprocessingTestRegionFrameFilepath));
}

void ImagePreprocessingsHandler::saveSelectedImagePreprocessing() const
{
	ImagePreprocessing* selectedImagePreprocessing = getSelectedImagePreprocessing();
	if (selectedImagePreprocessing != nullptr) {
		const std::string selectedImagePreprocessingName = selectedImagePreprocessing->getName();
		SimplePersistenceUtils::saveString("selected-image-preprocessing-name", selectedImagePreprocessingName, TravailleurUtils::getConfigPath(travailleurContext->getName()));
	}
}

void ImagePreprocessingsHandler::updateSelectedImagePreprocessingBasedOnSave()
{
	std::string selectedImagePreprocessingName;
	SimplePersistenceUtils::loadString("selected-image-preprocessing-name", selectedImagePreprocessingName, TravailleurUtils::getConfigPath(travailleurContext->getName()));
	ImagePreprocessing* savedSelectedImagePreprocessing = getImagePreprocessing(selectedImagePreprocessingName);
	if (savedSelectedImagePreprocessing != nullptr) {
		setSelectedImagePreprocessing(savedSelectedImagePreprocessing);
	}
}

void ImagePreprocessingsHandler::saveRectangularRegionsNamesListSelection_preprocessingTestRegion() const
{
	if (rectangularRegionsNamesList_preprocessingTestRegion->getSelectedIndex() != -1) {
		const std::string rectangularRegionsNamesListSelection = rectangularRegionsNamesList_preprocessingTestRegion->getSelectedOption();
		SimplePersistenceUtils::saveString("rectangular-regions-names-list-selection-preprocessing-test-region", rectangularRegionsNamesListSelection, TravailleurUtils::getConfigPath(travailleurContext->getName()));
	}
}

void ImagePreprocessingsHandler::loadRectangularRegionsNamesListSelection_preprocessingTestRegion()
{
	std::string rectangularRegionsNamesListSelection;
	SimplePersistenceUtils::loadString("rectangular-regions-names-list-selection-preprocessing-test-region", rectangularRegionsNamesListSelection, TravailleurUtils::getConfigPath(travailleurContext->getName()));
	rectangularRegionsNamesList_preprocessingTestRegion->setSelectedOption(rectangularRegionsNamesListSelection);
}

void ImagePreprocessingsHandler::updateImagePreprocessingsNames()
{
	std::vector<std::string> names;
	for (auto& mapItem : mapFromNameToImagePreprocessing) {
		names.push_back(mapItem.first);
	}
	imagePreprocessingsNames->setOptions(names);
	updateSelectedImagePreprocessingBasedOnSave();
}

void ImagePreprocessingsHandler::registerImagePreprocessing(const std::string& name)
{
	assert(getImagePreprocessing(name) == nullptr);
	assert(mapFromNameToImagePreprocessing.find(name) == mapFromNameToImagePreprocessing.end());

	auto imagePreprocessing = new ImagePreprocessing(name, travailleurContext);
	mapFromNameToImagePreprocessing[name] = imagePreprocessing;

	updateImagePreprocessingsNames();
}

void ImagePreprocessingsHandler::afterAppSetup()
{
	const std::vector<std::string> rectangularRegionsNames = travailleurContext->rectangularRegionsNames();
	rectangularRegionsNamesList_preprocessingTestRegion->setOptions(rectangularRegionsNames);

	loadRectangularRegionsNamesListSelection_preprocessingTestRegion();
}

ImagePreprocessing* ImagePreprocessingsHandler::getImagePreprocessing(const std::string& name) const
{
	auto it = mapFromNameToImagePreprocessing.find(name);
	if (it == mapFromNameToImagePreprocessing.end()) {
		return nullptr;
	}
	return it->second;
}

ImagePreprocessing* ImagePreprocessingsHandler::getSelectedImagePreprocessing() const
{
	if (!imagePreprocessingsNames->hasSelectedOption()) {
		return nullptr;
	}
	return getImagePreprocessing(imagePreprocessingsNames->getSelectedOption());
}

void ImagePreprocessingsHandler::setSelectedImagePreprocessing(ImagePreprocessing* imagePreprocessing)
{
	if (imagePreprocessing == nullptr) return;
	const std::string imagePreprocessingName = imagePreprocessing->getName();
	imagePreprocessingsNames->setSelectedOption(imagePreprocessingName);
}

void ImagePreprocessingsHandler::showFrameOverlays(Image* frame, Image* frameOverlays)
{
	if (!allowFrameOverlays->getValue()) {
		return;
	}

	if (shouldShowPreprocessingTestRegion()) {
		showPreprocessingTestRegion(frame, frameOverlays);
	}
}

bool ImagePreprocessingsHandler::shouldShowPreprocessingTestRegion() const
{
	return showPreprocessingTestRegionCheckbox->getValue();
}

void ImagePreprocessingsHandler::showPreprocessingTestRegion(Image* frame, Image* frameOverlays)
{
	const Rectangle2D<int> region = determinePreprocessingTestRegion(frame);
	frameOverlays->highlightRectangularRegion(region.topLeft, region.size, 2, Pixel(255, 0, 0));
}

void ImagePreprocessingsHandler::performActionsThatDependOnFrame(Image* frame, Image* frameOverlays)
{
	if (shouldTestPreprocessing()) {
		testPreprocessing(frame);
	}
}

bool ImagePreprocessingsHandler::shouldTestPreprocessing() const
{
	return wantsToTestPreprocessing;
}

Rectangle2D<int> ImagePreprocessingsHandler::determinePreprocessingTestRegion(Image* frame) const
{
	if (useRectangularRegionAsPreprocessingTestRegion->getValue()) {
		RectangularRegion* rectangularRegionToUse = travailleurContext->getRectangularRegion(rectangularRegionsNamesList_preprocessingTestRegion->getSelectedOption());
		if (rectangularRegionToUse != nullptr) {
			const Point2D<int> topLeftConsideringParents = travailleurContext->getRectangularRegionsHandler()->determineTopLeftConsideringParents(frame, rectangularRegionToUse);
			return Rectangle2D<int>(topLeftConsideringParents, rectangularRegionToUse->getSize());
		}
	}
	return preprocessingTestRegion;
}

void ImagePreprocessingsHandler::testPreprocessing(Image* frame)
{
	const Rectangle2D<int> region = determinePreprocessingTestRegion(frame);
	ImagePreprocessing* selectedImagePreprocessing = getSelectedImagePreprocessing();
	if (selectedImagePreprocessing == nullptr) {
		timeTakenByTestInMs = "";
		return;
	}
	delete originalTestImage;
	originalTestImage = new Image(*frame);
	const bool wasAbleToCrop = originalTestImage->crop(region);
	if (wasAbleToCrop) {
		delete preprocessedTestImage;
		preprocessedTestImage = new Image(*originalTestImage);

		Chronometer chronometer;

		ImagePreprocessingUtils::applyImagePreprocessing(preprocessedTestImage, selectedImagePreprocessing);

		const double timeElapsedInMs = chronometer.TimeElapsed(Chronometer::Unit::Milliseconds);
		timeTakenByTestInMs = std::to_string(timeElapsedInMs);
	}
	else {
		delete originalTestImage;
		originalTestImage = nullptr;
		delete preprocessedTestImage;
		preprocessedTestImage = nullptr;
		timeTakenByTestInMs = "";
	}

	wantsToTestPreprocessing = false;
}

void ImagePreprocessingsHandler::showGui()
{
	ImGui::Begin("ImagePreprocessingsHandler");

	allowFrameOverlays->showGui();

	showOriginalTestImageCheckbox->showGui();
	if (showOriginalTestImageCheckbox->getValue()) {
		ImGuiTextOutput("originalTestImage:");
		if (originalTestImage != nullptr) {
			originalTestImageRenderer->renderImage(*originalTestImage);
		}
		else {
			ImGuiTextOutput("no original test image");
		}
	}

	showPreprocessedTestImageCheckbox->showGui();
	if (showPreprocessedTestImageCheckbox->getValue()) {
		ImGuiTextOutput("preprocessedTestImage:");
		if (preprocessedTestImage != nullptr) {
			preprocessedTestImageRenderer->renderImage(*preprocessedTestImage);
		}
		else {
			ImGuiTextOutput("no preprocessedTestImage test image");
		}
	}

	if (timeTakenByTestInMs != "") {
		ImGuiTextOutput("time taken to preprocess image: " + timeTakenByTestInMs + " ms");
	}

	imagePreprocessingsNames->showGui();
	ImagePreprocessing* selectedImagePreprocessing = getSelectedImagePreprocessing();

	useRectangularRegionAsPreprocessingTestRegion->showGui();
	if (useRectangularRegionAsPreprocessingTestRegion->getValue()) {
		rectangularRegionsNamesList_preprocessingTestRegion->showGui();
	}
	else {
		ImGuiSimpleIntRectangleSlider("preprocessingTestRegion", &preprocessingTestRegion, 1.0, -2000, 2000, 0, 2000);
	}

	if (selectedImagePreprocessing != nullptr) {
		if (ImGui::Button("Test preprocessing")) {
			wantsToTestPreprocessing = true;
		}
	}
	else {
		ImGuiTextOutput("no selected image preprocessing to be tested");
	}

	if (selectedImagePreprocessing != nullptr)
	{
		selectedImagePreprocessing->showGui();
	}
	else {
		ImGuiTextOutput("no selected image preprocessing");
	}

	ImGui::End();
}

std::vector<std::string> ImagePreprocessingsHandler::getImagePreprocessingsNames() const
{
	return imagePreprocessingsNames->getOptions();
}