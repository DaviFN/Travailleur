#include "ImageOCRsHandler.h"

#include "assert.h"
#include "imgui.h"

#include "Common/Chronometer.h"
#include "Common/Rectangle2DPersistenceUtils.h"
#include "Common/TravailleurUtils.h"
#include "ImGuiWrappers/ImGuiSimpleIntRectangleSlider.h"
#include "ImGuiWrappers/ImGuiTextOutput.h"
#include "RectangularRegionsHandler.h"
#include "RectangularRegion.h"
#include "TravailleurContext.h"

ImageOCRsHandler::ImageOCRsHandler(TravailleurContext* travailleurContext):
	travailleurContext(travailleurContext)
{
	allowFrameOverlays = std::make_unique<ImGuiCheckbox>("Allow frame overlays", true, TravailleurUtils::getConfigFilenameFor("allow-image-ocrs-handler-show-overlays", travailleurContext->getName()));

	imageOCRsNames = std::make_unique<ImGuiSingleSelectionList>("image-ocrs-names");

	showTestImageOCROptions = std::make_unique<ImGuiCheckbox>("Show test image ocr options", true, TravailleurUtils::getConfigFilenameFor("show-test-image-ocr-options", travailleurContext->getName()));

	useCustomTestRegion = std::make_unique<ImGuiCheckbox>("Use custom test region", true, TravailleurUtils::getConfigFilenameFor("image-ocr-use-custom-test-region", travailleurContext->getName()));

	testImagePreviewRenderer = std::make_unique<ImGuiImageRenderer>();

	skipImagePreprocessingOnTestPreview = std::make_unique<ImGuiCheckbox>("Skip image preprocessing on test preview", false, TravailleurUtils::getConfigFilenameFor("skip-image-preprocessing-on-test-preview", travailleurContext->getName()));

	load();
}

ImageOCRsHandler::~ImageOCRsHandler()
{
	save();
	saveSelectedImageOCR();

	for (auto& mapItem : mapFromNameToImageOCR) {
		delete mapItem.second;
	}
}

void ImageOCRsHandler::save() const
{
	const std::string testRegionFrameFilepath = TravailleurUtils::getConfigFilenameFor("ocr-image-test-region", travailleurContext->getName());
	Rectangle2DPersistenceUtils::saveIntRectangle2D(FilesystemUtils::filenameFromFilepath(testRegionFrameFilepath), testRegion, FilesystemUtils::parentDirectoryOfFilepath(testRegionFrameFilepath));
}

void ImageOCRsHandler::load()
{
	const std::string testRegionFrameFilepath = TravailleurUtils::getConfigFilenameFor("ocr-image-test-region", travailleurContext->getName());
	Rectangle2DPersistenceUtils::loadIntRectangle2D(FilesystemUtils::filenameFromFilepath(testRegionFrameFilepath), testRegion, FilesystemUtils::parentDirectoryOfFilepath(testRegionFrameFilepath));
}

void ImageOCRsHandler::saveSelectedImageOCR() const
{
	ImageOCR* selectedImageOCR = getSelectedImageOCR();
	if (selectedImageOCR != nullptr) {
		const std::string selectedImageOCRName = selectedImageOCR->getName();
		SimplePersistenceUtils::saveString("selected-image-ocr-name", selectedImageOCRName, TravailleurUtils::getConfigPath(travailleurContext->getName()));
	}
}

void ImageOCRsHandler::updateSelectedImageOCRBasedOnSave()
{
	std::string selectedImageOCRName;
	SimplePersistenceUtils::loadString("selected-image-ocr-name", selectedImageOCRName, TravailleurUtils::getConfigPath(travailleurContext->getName()));
	ImageOCR* savedSelectedImageOCR = getImageOCR(selectedImageOCRName);
	if (savedSelectedImageOCR != nullptr) {
		setSelectedImageOCR(savedSelectedImageOCR);
	}
}

std::string ImageOCRsHandler::getImageOCRsFolder() const
{
	return TravailleurUtils::getImageOCRsPath(travailleurContext->getName());
}

std::string ImageOCRsHandler::filepathOfImageOCR(const std::string& name) const
{
	return getImageOCRsFolder() + "\\" + name;
}

void ImageOCRsHandler::updateImageOCRsNames()
{
	std::vector<std::string> names;
	for (auto& mapItem : mapFromNameToImageOCR) {
		names.push_back(mapItem.first);
	}
	imageOCRsNames->setOptions(names);
}

void ImageOCRsHandler::registerImageOCR(const std::string& name)
{
	assert(getImageOCR(name) == nullptr);
	assert(mapFromNameToImageOCR.find(name) == mapFromNameToImageOCR.end());

	auto imageOCR = new ImageOCR(name, travailleurContext);
	mapFromNameToImageOCR[name] = imageOCR;

	updateImageOCRsNames();
	updateSelectedImageOCRBasedOnSave();
}

ImageOCR* ImageOCRsHandler::getImageOCR(const std::string& name) const
{
	auto it = mapFromNameToImageOCR.find(name);
	if (it == mapFromNameToImageOCR.end()) {
		return nullptr;
	}
	return it->second;
}

ImageOCR* ImageOCRsHandler::getSelectedImageOCR() const
{
	if (!imageOCRsNames->hasSelectedOption()) {
		return nullptr;
	}
	return getImageOCR(imageOCRsNames->getSelectedOption());
}

void ImageOCRsHandler::setSelectedImageOCR(ImageOCR* imageOCR)
{
	if (imageOCR == nullptr) return;
	const std::string imageOCRName = imageOCR->getName();
	imageOCRsNames->setSelectedOption(imageOCRName);
}

void ImageOCRsHandler::showGui()
{
	ImGui::Begin("ImageOCRsHandler");

	allowFrameOverlays->showGui();

	imageOCRsNames->showGui();

	ImageOCR* selectedImageOCR = getSelectedImageOCR();
	if (selectedImageOCR != nullptr)
	{
		selectedImageOCR->showGui();
	}
	else {
		ImGuiTextOutput("no selected image ocr");
	}

	showTestImageOCROptions->showGui();
	if (showTestImageOCROptions->getValue()) {

		useCustomTestRegion->showGui();
		if (useCustomTestRegion->getValue()) {
			ImGuiSimpleIntRectangleSlider("testRegion", &testRegion, 1.0, -2000, 2000, 0, 2000);
		}

		if (ImGui::Button("Perform test OCR")) {
			wantsToPerformTestOCR = true;
		}

		if (ImGui::Button("Clear test OCR result")) {
			ocrTestResult = "";
			timeTakenByOcrTestInMs = "";
		}

		ImGuiTextOutput("Current test OCR result:");
		ImGuiTextOutput(ocrTestResult);
		if (timeTakenByOcrTestInMs != "") {
			ImGuiTextOutput("took " + timeTakenByOcrTestInMs + " ms to perform");
		}


		if (ImGui::Button("Update test image preview")) {
			wantsToUpdateTestImagePreview = true;
		}
		if (ImGui::Button("Clear test image preview")) {
			delete testImagePreview;
			testImagePreview = nullptr;
		}
		skipImagePreprocessingOnTestPreview->showGui();

		ImGuiTextOutput("Test image preview:");
		if (testImagePreview != nullptr) {
			testImagePreviewRenderer->renderImage(*testImagePreview);
		}
		else {
			ImGuiTextOutput("No test image preview");
		}

	}

	ImGui::End();
}

void ImageOCRsHandler::showFrameOverlays(Image* frame, Image* frameOverlays)
{
	if (!allowFrameOverlays->getValue()) {
		return;
	}

	ImageOCR* selectedImageOCR = getSelectedImageOCR();
	if (selectedImageOCR != nullptr) {
		RectangularRegion* rectangularRegion = selectedImageOCR->getRectangularRegion();

		if (rectangularRegion != nullptr) {
			const Point2D<int> topLeft = travailleurContext->getRectangularRegionsHandler()->determineTopLeftConsideringParents(frame, rectangularRegion);
			const Point2D<int> size = rectangularRegion->getSize();
			frameOverlays->highlightRectangularRegion(topLeft, size, 2, characteristicColor());
		}
	}

	if (shouldShowTestRegion()) {
		showTestRegion(frame, frameOverlays);
	}
}

bool ImageOCRsHandler::shouldShowTestRegion() const
{
	return showTestImageOCROptions->getValue();
}

void ImageOCRsHandler::showTestRegion(Image* frame, Image* frameOverlays)
{
	const Rectangle2D<int> region = determineTestRegion(frame);
	frameOverlays->highlightRectangularRegion(region.topLeft, region.size, 2, Pixel(153, 102, 255));
}

Rectangle2D<int> ImageOCRsHandler::determineTestRegion(Image* frame)
{
	if (!useCustomTestRegion->getValue()) {
		ImageOCR* selectedImageOCR = getSelectedImageOCR();
		if (selectedImageOCR != nullptr) {
			RectangularRegion* rectangularRegion = selectedImageOCR->getRectangularRegion();
			if (rectangularRegion != nullptr) {
				const Point2D<int> topLeftConsideringParents = travailleurContext->getRectangularRegionsHandler()->determineTopLeftConsideringParents(frame, rectangularRegion);
				return Rectangle2D<int>(topLeftConsideringParents, rectangularRegion->getSize());
			}
		}
	}
	return testRegion;
}

void ImageOCRsHandler::performActionsThatDependOnFrame(Image* frame, Image* frameOverlays)
{
	if (wantsToPerformTestOCR) {

		performTestOcr(frame);

		wantsToPerformTestOCR = false;
	}

	if (wantsToUpdateTestImagePreview) {

		updateTestImagePreview(frame);

		wantsToUpdateTestImagePreview = false;
	}
}

void ImageOCRsHandler::performTestOcr(Image* frame)
{
	ImageOCR* selectedImageOCR = getSelectedImageOCR();
	if (selectedImageOCR == nullptr) return;

	Chronometer chronometer;

	ocrTestResult = selectedImageOCR->performOCROnRegion(frame, determineTestRegion(frame));

	const double timeElapsedInMs = chronometer.TimeElapsed(Chronometer::Unit::Milliseconds);
	timeTakenByOcrTestInMs = std::to_string(timeElapsedInMs);
}

void ImageOCRsHandler::updateTestImagePreview(Image* frame)
{
	delete testImagePreview;
	testImagePreview = nullptr;

	const Rectangle2D<int> testRegion = determineTestRegion(frame);

	if (!testRegion.isInsideOther(frame->getRegion())) return;

	testImagePreview = new Image(*frame);
	testImagePreview->crop(testRegion);

	if (!skipImagePreprocessingOnTestPreview->getValue()) {
		ImageOCR* selectedImageOCR = getSelectedImageOCR();
		if (selectedImageOCR != nullptr) {
			ImagePreprocessing* imagePreprocessing = selectedImageOCR->getImagePreprocessing();
			if (imagePreprocessing != nullptr) {
				imagePreprocessing->apply(testImagePreview);
			}
		}
	}
}

void ImageOCRsHandler::afterAppSetup()
{
	const std::vector<std::string> rectangularRegionNames = travailleurContext->rectangularRegionsNames();
	const std::vector<std::string> imagePreprocessingsNames = travailleurContext->imagePreprocessingsNames();
	for (auto& mapItem : mapFromNameToImageOCR)
	{
		ImageOCR* imageOCR = mapItem.second;
		imageOCR->informRectangularRegionsNames(rectangularRegionNames);
		imageOCR->informImagePreprocessingsNames(imagePreprocessingsNames);
	}
}

std::vector<std::string> ImageOCRsHandler::getImageOCRsNames() const
{
	std::vector<std::string> names;
	for (auto& mapItem : mapFromNameToImageOCR) {
		names.push_back(mapItem.first);
	}
	return names;
}

Pixel ImageOCRsHandler::characteristicColor()
{
	return Pixel(51, 204, 51);
}