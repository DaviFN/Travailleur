#include "ImagePatternsHandler.h"

#include "imgui.h"

#include "Common/Chronometer.h"
#include "Common/Point2DPersistenceUtils.h"
#include "Common/Rectangle2DPersistenceUtils.h"
#include "Common/TravailleurUtils.h"
#include "ImGuiWrappers/ImGuiSimpleColorSelector.h"
#include "ImGuiWrappers/ImGuiColorOutput.h"
#include "ImGuiWrappers/ImGuiSimpleIntRectangleSlider.h"
#include "ImGuiWrappers/ImGuiTextOutput.h"
#include "TravailleurContext.h"

ImagePatternsHandler::ImagePatternsHandler(TravailleurContext* travailleurContext):
	travailleurContext(travailleurContext)
{
	allowFrameOverlays = std::make_unique<ImGuiCheckbox>("Allow frame overlays", true, TravailleurUtils::getConfigFilenameFor("allow-image-patterns-handler-show-overlays", travailleurContext->getName()));

	imagePatternsNames = std::make_unique<ImGuiSingleSelectionList>("image-patterns-names");

	showPickColorFromFrameOptions = std::make_unique<ImGuiCheckbox>("Show pick color from frame options", true, TravailleurUtils::getConfigFilenameFor("show-pick-color-from-frame-options", travailleurContext->getName()));

	colorToPickPositionInFrame = std::make_unique<ImGuiPointInput>("Color to pick's position in frame", -2000, 2000, 1.0, TravailleurUtils::getConfigFilenameFor("color-to-pick-position-in-frame", travailleurContext->getName()));

	showPatternCaptureOptions = std::make_unique<ImGuiCheckbox>("Show pattern capture options", true, TravailleurUtils::getConfigFilenameFor("show-pattern-capture-options", travailleurContext->getName()));
	showPatternPreviewCheckbox = std::make_unique<ImGuiCheckbox>("Show pattern preview", true, TravailleurUtils::getConfigFilenameFor("show-pattern-preview", travailleurContext->getName()));
	showTransparentPixelsWithTransparentPixelsColor = std::make_unique<ImGuiCheckbox>("Show transparent pixels with transparent pixels color", true, TravailleurUtils::getConfigFilenameFor("show-transparent-pixels-with-transparent-pixels-color", travailleurContext->getName()));

	showTestPatternOptions = std::make_unique<ImGuiCheckbox>("Show test pattern options", true, TravailleurUtils::getConfigFilenameFor("show-test-pattern-options", travailleurContext->getName()));

	testAcrossWholeTestRegion = std::make_unique<ImGuiCheckbox>("Test across whole test region", true, TravailleurUtils::getConfigFilenameFor("test-across-whole-test-region", travailleurContext->getName()));

	showTestRegionPixelApproximationsCheckbox = std::make_unique<ImGuiCheckbox>("Show test region pixel approximations", true, TravailleurUtils::getConfigFilenameFor("show-test-region-pixel-approximations", travailleurContext->getName()));

	testRegionPixelApproximationsImageRenderer = std::make_unique<ImGuiImageRenderer>();

	performOnlyPreprocessingForTestRegionPixelApproximations = std::make_unique<ImGuiCheckbox>("Perform only preprocessing for test region pixel approximations", false, TravailleurUtils::getConfigFilenameFor("perform-only-preprocessing-for-test-region-pixel-approximations", travailleurContext->getName()));

	getImagePatternRegionPixelApproximationsImageRenderer = std::make_unique<ImGuiImageRenderer>();

	performOnlyPreprocessingForGetImagePatternRegionPixelApproximations = std::make_unique<ImGuiCheckbox>("Perform only preprocessing for get image pattern region pixel approximations", false, TravailleurUtils::getConfigFilenameFor("perform-only-preprocessing-for-get-image-pattern-region-pixel-approximations", travailleurContext->getName()));

	useRectangularRegionAsCaptureRegion = std::make_unique<ImGuiCheckbox>("Use rectangular region as capture region", false, TravailleurUtils::getConfigFilenameFor("use-rectangular-region-as-capture-region", travailleurContext->getName()));
	rectangularRegionsNamesList_captureRegion = std::make_unique<ImGuiSingleSelectionList>("rectangular-regions-names-list-capture-region");

	useRectangularRegionAsTestRegion = std::make_unique<ImGuiCheckbox>("Use rectangular region as test region", false, TravailleurUtils::getConfigFilenameFor("use-rectangular-region-as-test-region", travailleurContext->getName()));
	rectangularRegionsNamesList_testRegion = std::make_unique<ImGuiSingleSelectionList>("rectangular-regions-names-list-test-region");

	load();
}

ImagePatternsHandler::~ImagePatternsHandler()
{
	save();
	saveSelectedImagePattern();
	saveRectangularRegionsNamesListSelection_captureRegion();
	saveRectangularRegionsNamesListSelection_testRegion();

	for (auto& mapItem : mapFromNameToImagePattern) {
		delete mapItem.second;
	}
}

void ImagePatternsHandler::save() const
{
	transparentPixelsColor.saveToBinaryFile(TravailleurUtils::getConfigFilenameFor("transparent-pixels-color", travailleurContext->getName()));
	colorToAddToPattern.saveToBinaryFile(TravailleurUtils::getConfigFilenameFor("color-to-add-to-pattern", travailleurContext->getName()));
	
	const std::string captureRegionFrameFilepath = TravailleurUtils::getConfigFilenameFor("capture-region", travailleurContext->getName());
	Rectangle2DPersistenceUtils::saveIntRectangle2D(FilesystemUtils::filenameFromFilepath(captureRegionFrameFilepath), captureRegion, FilesystemUtils::parentDirectoryOfFilepath(captureRegionFrameFilepath));

	const std::string testRegionFrameFilepath = TravailleurUtils::getConfigFilenameFor("test-region", travailleurContext->getName());
	Rectangle2DPersistenceUtils::saveIntRectangle2D(FilesystemUtils::filenameFromFilepath(testRegionFrameFilepath), testRegion, FilesystemUtils::parentDirectoryOfFilepath(testRegionFrameFilepath));
}

void ImagePatternsHandler::load()
{
	transparentPixelsColor.loadFromBinaryFile(TravailleurUtils::getConfigFilenameFor("transparent-pixels-color", travailleurContext->getName()));
	colorToAddToPattern.loadFromBinaryFile(TravailleurUtils::getConfigFilenameFor("color-to-add-to-pattern", travailleurContext->getName()));

	const std::string captureRegionFrameFilepath = TravailleurUtils::getConfigFilenameFor("capture-region", travailleurContext->getName());
	Rectangle2DPersistenceUtils::loadIntRectangle2D(FilesystemUtils::filenameFromFilepath(captureRegionFrameFilepath), captureRegion, FilesystemUtils::parentDirectoryOfFilepath(captureRegionFrameFilepath));

	const std::string testRegionFrameFilepath = TravailleurUtils::getConfigFilenameFor("test-region", travailleurContext->getName());
	Rectangle2DPersistenceUtils::loadIntRectangle2D(FilesystemUtils::filenameFromFilepath(testRegionFrameFilepath), testRegion, FilesystemUtils::parentDirectoryOfFilepath(testRegionFrameFilepath));
}

void ImagePatternsHandler::saveSelectedImagePattern() const
{
	ImagePattern* selectedImagePattern = getSelectedImagePattern();
	if (selectedImagePattern != nullptr) {
		const std::string selectedImagePatternName = selectedImagePattern->getName();
		SimplePersistenceUtils::saveString("selected-image-pattern-name", selectedImagePatternName, TravailleurUtils::getConfigPath(travailleurContext->getName()));
	}
}

void ImagePatternsHandler::updateSelectedImagePatternBasedOnSave()
{
	std::string selectedImagePatternName;
	SimplePersistenceUtils::loadString("selected-image-pattern-name", selectedImagePatternName, TravailleurUtils::getConfigPath(travailleurContext->getName()));
	ImagePattern* savedSelectedImagePattern = getImagePattern(selectedImagePatternName);
	if (savedSelectedImagePattern != nullptr) {
		setSelectedImagePattern(savedSelectedImagePattern);
	}
}

void ImagePatternsHandler::updateImagePatternsNames()
{
	std::vector<std::string> names;
	for (auto& mapItem : mapFromNameToImagePattern) {
		names.push_back(mapItem.first);
	}
	imagePatternsNames->setOptions(names);
	updateSelectedImagePatternBasedOnSave();
}

void ImagePatternsHandler::saveRectangularRegionsNamesListSelection_captureRegion() const
{
	if (rectangularRegionsNamesList_captureRegion->getSelectedIndex() != -1) {
		const std::string rectangularRegionsNamesListSelection = rectangularRegionsNamesList_captureRegion->getSelectedOption();
		SimplePersistenceUtils::saveString("rectangular-regions-names-list-selection-capture-region", rectangularRegionsNamesListSelection, TravailleurUtils::getConfigPath(travailleurContext->getName()));
	}
}

void ImagePatternsHandler::loadRectangularRegionsNamesListSelection_captureRegion()
{
	std::string rectangularRegionsNamesListSelection;
	SimplePersistenceUtils::loadString("rectangular-regions-names-list-selection-capture-region", rectangularRegionsNamesListSelection, TravailleurUtils::getConfigPath(travailleurContext->getName()));
	rectangularRegionsNamesList_captureRegion->setSelectedOption(rectangularRegionsNamesListSelection);
}

void ImagePatternsHandler::saveRectangularRegionsNamesListSelection_testRegion() const
{
	if (rectangularRegionsNamesList_testRegion->getSelectedIndex() != -1) {
		const std::string rectangularRegionsNamesListSelection = rectangularRegionsNamesList_testRegion->getSelectedOption();
		SimplePersistenceUtils::saveString("rectangular-regions-names-list-selection-test-region", rectangularRegionsNamesListSelection, TravailleurUtils::getConfigPath(travailleurContext->getName()));
	}
}

void ImagePatternsHandler::loadRectangularRegionsNamesListSelection_testRegion()
{
	std::string rectangularRegionsNamesListSelection;
	SimplePersistenceUtils::loadString("rectangular-regions-names-list-selection-test-region", rectangularRegionsNamesListSelection, TravailleurUtils::getConfigPath(travailleurContext->getName()));
	rectangularRegionsNamesList_testRegion->setSelectedOption(rectangularRegionsNamesListSelection);
}

void ImagePatternsHandler::registerImagePattern(const std::string& name)
{
	assert(getImagePattern(name) == nullptr);
	assert(mapFromNameToImagePattern.find(name) == mapFromNameToImagePattern.end());

	auto imagePattern = new ImagePattern(name, travailleurContext);
	mapFromNameToImagePattern[name] = imagePattern;

	updateImagePatternsNames();
}

void ImagePatternsHandler::afterAppSetup()
{
	const std::vector<std::string> imagePreprocessingsNames = travailleurContext->imagePreprocessingsNames();
	for (auto& item : mapFromNameToImagePattern) {
		ImagePattern* imagePattern = item.second;
		imagePattern->updateImagePreprocessingToUseToCaptureSingleSelectionList(imagePreprocessingsNames);
		imagePattern->updateImagePreprocessingToUseToDetectSingleSelectionList(imagePreprocessingsNames);
	}

	const std::vector<std::string> rectangularRegionsNames = travailleurContext->rectangularRegionsNames();
	rectangularRegionsNamesList_captureRegion->setOptions(rectangularRegionsNames);
	rectangularRegionsNamesList_testRegion->setOptions(rectangularRegionsNames);

	loadRectangularRegionsNamesListSelection_captureRegion();
	loadRectangularRegionsNamesListSelection_testRegion();
}

Rectangle2D<int> ImagePatternsHandler::determineCaptureRegion(Image* frame) const
{
	if (useRectangularRegionAsCaptureRegion->getValue()) {
		RectangularRegion* rectangularRegionToUse = travailleurContext->getRectangularRegion(rectangularRegionsNamesList_captureRegion->getSelectedOption());
		if (rectangularRegionToUse != nullptr) {
			const Point2D<int> topLeftConsideringParents = travailleurContext->getRectangularRegionsHandler()->determineTopLeftConsideringParents(frame, rectangularRegionToUse);
			return Rectangle2D<int>(topLeftConsideringParents, rectangularRegionToUse->getSize());
		}
	}
	return captureRegion;
}

Rectangle2D<int> ImagePatternsHandler::determineTestRegion(Image* frame) const
{
	if (useRectangularRegionAsTestRegion->getValue()) {
		RectangularRegion* rectangularRegionToUse = travailleurContext->getRectangularRegion(rectangularRegionsNamesList_testRegion->getSelectedOption());
		if (rectangularRegionToUse != nullptr) {
			const Point2D<int> topLeftConsideringParents = travailleurContext->getRectangularRegionsHandler()->determineTopLeftConsideringParents(frame, rectangularRegionToUse);
			return Rectangle2D<int>(topLeftConsideringParents, rectangularRegionToUse->getSize());
		}
	}
	return testRegion;
}

ImagePattern* ImagePatternsHandler::getImagePattern(const std::string& name) const
{
	auto it = mapFromNameToImagePattern.find(name);
	if (it == mapFromNameToImagePattern.end()) {
		return nullptr;
	}
	return it->second;
}

ImagePattern* ImagePatternsHandler::getSelectedImagePattern() const
{
	if (!imagePatternsNames->hasSelectedOption()) {
		return nullptr;
	}
	return getImagePattern(imagePatternsNames->getSelectedOption());
}

void ImagePatternsHandler::setSelectedImagePattern(ImagePattern* imagePattern)
{
	if (imagePattern == nullptr) return;
	const std::string imagePatternName = imagePattern->getName();
	imagePatternsNames->setSelectedOption(imagePatternName);
}

void ImagePatternsHandler::showFrameOverlays(Image* frame, Image* frameOverlays)
{
	if (!allowFrameOverlays->getValue()) {
		return;
	}

	if (shouldShowCaptureRegion()) {
		showCaptureRegion(frame, frameOverlays);
	}
	if (shouldShowPatternPreview()) {
		showPatternPreview(frame, frameOverlays);
	}
	if (shouldShowTestRegionPixelApproximations()) {
		showTestRegionPixelApproximations(frame, frameOverlays);
	}
	if (shouldShowTestPatternOverlays()) {
		showTestPatternOverlays(frame, frameOverlays);
	}
	if (shouldShowColorBeingPickedFromFrame()) {
		const Pixel colorBeingPicked = frame->pixelAt(positionOfColorBeingPickedFromFrame());
		setColorBeingPickedFromFrame(colorBeingPicked);
		showColorBeingPickedFromFrame(frameOverlays);
	}
	if (shouldUpdateTestRegionPixelAproximationsImage()) {
		updateTestRegionPixelApproximationsImage(frame);
	}
	if (shouldUpdateGetImagePatternRegionPixelAproximationsImage()) {
		updateGetImagePatternRegionPixelApproximationsImage(frame);
	}
}

void ImagePatternsHandler::performActionsThatDependOnFrame(Image* frame, Image* frameOverlays)
{
	if (shouldCapturePattern()) {
		capturePattern(frame);
	}
	if (shouldTestPattern()) {
		testPattern(frame, frameOverlays);
	}
}

void ImagePatternsHandler::showGui()
{
	ImGui::Begin("ImagePatternsHandler");

	allowFrameOverlays->showGui();

	imagePatternsNames->showGui();
	ImagePattern* selectedImagePattern = getSelectedImagePattern();

	ImGuiSimpleColorSelector("Transparent pixels' color", transparentPixelsColor);
	ImGuiSimpleColorSelector("Color to add to pattern", colorToAddToPattern);

	if (ImGui::Button("Add color to pattern")) {
		addColorToPattern();
	}

	showPickColorFromFrameOptions->showGui();
	if (showPickColorFromFrameOptions->getValue()) {
		colorToPickPositionInFrame->showGui();
		ImGuiColorOutput("Color being picked from frame", colorBeingPickedFromFrame);
		if (ImGui::Button("Define as transparent pixels' color")) {
			transparentPixelsColor = colorBeingPickedFromFrame;
		}
		if (ImGui::Button("Define as color to add to pattern")) {
			colorToAddToPattern = colorBeingPickedFromFrame;
		}
	}

	showPatternCaptureOptions->showGui();
	if (showPatternCaptureOptions->getValue()) {
		if (selectedImagePattern != nullptr && selectedImagePattern->usesImagePreprocessingToCapture()) {
			performOnlyPreprocessingForGetImagePatternRegionPixelApproximations->showGui();
			if (ImGui::Button("update get image pattern region pixel approximations image")) {
				wantsToUpdateGetImagePatternRegionPixelAproximationsImage = true;
			}
			if (ImGui::Button("clear get image pattern region pixel approximations image")) {
				delete getImagePatternRegionPixelApproximationsImage;
				getImagePatternRegionPixelApproximationsImage = nullptr;
			}
			if (getImagePatternRegionPixelApproximationsImage != nullptr) {
				getImagePatternRegionPixelApproximationsImageRenderer->renderImage(*getImagePatternRegionPixelApproximationsImage);
			}
			else {
				ImGuiTextOutput("no update get image pattern region pixel approximations image");
			}
		}
		else {
			showPatternPreviewCheckbox->showGui();
			if (showPatternPreviewCheckbox->getValue()) {
				showTransparentPixelsWithTransparentPixelsColor->showGui();
			}
		}

		useRectangularRegionAsCaptureRegion->showGui();
		if (useRectangularRegionAsCaptureRegion->getValue()) {
			rectangularRegionsNamesList_captureRegion->showGui();
		}
		else {
			ImGuiSimpleIntRectangleSlider("captureRegion", &captureRegion, 1.0, -2000, 2000, 0, 2000);
		}


		if (ImGui::Button("Capture pattern")) {
			wantsToCapturePattern = true;
		}
	}

	showTestPatternOptions->showGui();
	if (showTestPatternOptions->getValue())
	{

		useRectangularRegionAsTestRegion->showGui();
		if (useRectangularRegionAsTestRegion->getValue()) {
			rectangularRegionsNamesList_testRegion->showGui();
		}
		else {
			ImGuiSimpleIntRectangleSlider("testRegion", &testRegion, 1.0, -2000, 2000, 0, 2000);
		}

		if (selectedImagePattern != nullptr && selectedImagePattern->usesImagePreprocessingToDetect()) {
			performOnlyPreprocessingForTestRegionPixelApproximations->showGui();
			if (ImGui::Button("update test region pixel approximations image")) {
				wantsToUpdateTestRegionPixelAproximationsImage = true;
			}
			if (ImGui::Button("clear test region pixel approximations image")) {
				delete testRegionPixelApproximationsImage;
				testRegionPixelApproximationsImage = nullptr;
			}
			if (testRegionPixelApproximationsImage != nullptr) {
				testRegionPixelApproximationsImageRenderer->renderImage(*testRegionPixelApproximationsImage);
			}
			else {
				ImGuiTextOutput("no update test region pixel approximations image");
			}
		}
		else {
			showTestRegionPixelApproximationsCheckbox->showGui();
		}
		testAcrossWholeTestRegion->showGui();

		if (ImGui::Button("Test image pattern")) {
			wantsToTestPattern = true;
		}
		if (timeTakenByTestInMs != "") {
			ImGuiTextOutput("took " + timeTakenByTestInMs + " ms to perform last test");
		}
		if (ImGui::Button("Clear test results")) {
			patternTestResults.clear();
			timeTakenByTestInMs = "";
		}
	}

	if (selectedImagePattern != nullptr)
	{
		selectedImagePattern->showGui();
	}
	else {
		ImGuiTextOutput("no selected image pattern");
	}

	ImGui::End();
}

bool ImagePatternsHandler::shouldShowColorBeingPickedFromFrame() const
{
	return showPickColorFromFrameOptions->getValue();
}

Point2D<int> ImagePatternsHandler::positionOfColorBeingPickedFromFrame() const
{
	return colorToPickPositionInFrame->getValue();
}

void ImagePatternsHandler::setColorBeingPickedFromFrame(const Pixel& colorBeingPickedFromFrame)
{
	this->colorBeingPickedFromFrame = colorBeingPickedFromFrame;
}

void ImagePatternsHandler::showColorBeingPickedFromFrame(Image* frameOverlays)
{
	frameOverlays->drawCross(colorToPickPositionInFrame->getValue(), 200, 2, colorBeingPickedFromFrame);
}

void ImagePatternsHandler::addColorToPattern()
{
	ImagePattern* selectedImagePattern = getSelectedImagePattern();
	if (selectedImagePattern != nullptr) {
		selectedImagePattern->addColor(colorToAddToPattern);
	}
}

bool ImagePatternsHandler::shouldShowCaptureRegion() const
{
	return showPatternCaptureOptions->getValue();
}

void ImagePatternsHandler::showCaptureRegion(Image* frame, Image* frameOverlays)
{
	const Rectangle2D<int> region = determineCaptureRegion(frame);
	frameOverlays->highlightRectangularRegion(region.topLeft, region.size, 2, Pixel(0, 0, 255));
}

bool ImagePatternsHandler::shouldShowPatternPreview() const
{
	ImagePattern* selectedImagePattern = getSelectedImagePattern();
	return showPatternCaptureOptions->getValue() && showPatternPreviewCheckbox->getValue() && selectedImagePattern != nullptr && !selectedImagePattern->usesImagePreprocessingToCapture();
}

bool ImagePatternsHandler::shouldShowTestRegionPixelApproximations() const
{
	ImagePattern* selectedImagePattern = getSelectedImagePattern();
	return showTestPatternOptions->getValue() && showTestRegionPixelApproximationsCheckbox->getValue() && selectedImagePattern != nullptr && !selectedImagePattern->usesImagePreprocessingToDetect();
}

void ImagePatternsHandler::showPixelApproximations(Image* frame, Image* frameOverlays, const Rectangle2D<int>& region)
{
	ImagePattern* selectedImagePattern = getSelectedImagePattern();
	if (selectedImagePattern != nullptr) {
		for (int i = region.topLeft.x; i < region.topLeft.x + region.size.x; ++i) {
			for (int j = region.topLeft.y; j < region.topLeft.y + region.size.y; ++j) {
				if (frame->isValidPixelLocation(i, j)) {
					const Pixel approximation = selectedImagePattern->getPixelApproximation(frame->pixelAt(i, j), transparentPixelsColor);
					if (approximation.isTransparent() && showTransparentPixelsWithTransparentPixelsColor->getValue()) {
						frameOverlays->setPixel(i, j, transparentPixelsColor);
					}
					else {
						frameOverlays->setPixel(i, j, approximation);
					}
				}
			}
		}
	}
}

bool ImagePatternsHandler::shouldUpdateTestRegionPixelAproximationsImage() const
{
	return wantsToUpdateTestRegionPixelAproximationsImage;
}

bool ImagePatternsHandler::shouldUpdateGetImagePatternRegionPixelAproximationsImage() const
{
	return wantsToUpdateGetImagePatternRegionPixelAproximationsImage;
}

void ImagePatternsHandler::updateTestRegionPixelApproximationsImage(Image* frame)
{
	wantsToUpdateTestRegionPixelAproximationsImage = false;

	delete testRegionPixelApproximationsImage;
	testRegionPixelApproximationsImage = nullptr;

	const Rectangle2D<int> region = determineTestRegion(frame);

	if (!region.isValid()) return;
	if (!region.isInsideOther(frame->getRegion())) return;

	ImagePattern* selectedImagePattern = getSelectedImagePattern();
	ImagePreprocessing* imagePreprocessingToUse = selectedImagePattern->getImagePreprocessingToUseToDetect();
	if (imagePreprocessingToUse != nullptr) {
		testRegionPixelApproximationsImage = new Image(*frame);
		testRegionPixelApproximationsImage->crop(region);
		imagePreprocessingToUse->apply(testRegionPixelApproximationsImage);

		if (!performOnlyPreprocessingForTestRegionPixelApproximations->getValue()) {
			approximateImage(testRegionPixelApproximationsImage, selectedImagePattern);
		}
	}
}

void ImagePatternsHandler::updateGetImagePatternRegionPixelApproximationsImage(Image* frame)
{
	wantsToUpdateGetImagePatternRegionPixelAproximationsImage = false;

	delete getImagePatternRegionPixelApproximationsImage;
	getImagePatternRegionPixelApproximationsImage = nullptr;

	const Rectangle2D<int> region = determineCaptureRegion(frame);

	if (!region.isValid()) return;
	if (!region.isInsideOther(frame->getRegion())) return;

	ImagePattern* selectedImagePattern = getSelectedImagePattern();
	ImagePreprocessing* imagePreprocessingToUse = selectedImagePattern->getImagePreprocessingToUseToCapture();
	if (imagePreprocessingToUse != nullptr) {
		getImagePatternRegionPixelApproximationsImage = new Image(*frame);
		getImagePatternRegionPixelApproximationsImage->crop(region);
		imagePreprocessingToUse->apply(getImagePatternRegionPixelApproximationsImage);

		if (!performOnlyPreprocessingForGetImagePatternRegionPixelApproximations->getValue()) {
			approximateImage(getImagePatternRegionPixelApproximationsImage, selectedImagePattern);
		}
	}
}

void ImagePatternsHandler::approximateImage(Image* image, ImagePattern* imagePattern)
{
	if (image == nullptr) return;
	for (int i = 0; i < image->getWidth(); ++i) {
		for (int j = 0; j < image->getHeight(); ++j) {
			if (image->isValidPixelLocation(i, j)) {
				const Pixel approximation = imagePattern->getPixelApproximation(image->pixelAt(i, j), transparentPixelsColor);
				if (approximation.isTransparent() && showTransparentPixelsWithTransparentPixelsColor->getValue()) {
					image->setPixel(i, j, transparentPixelsColor);
				}
				else {
					image->setPixel(i, j, approximation);
				}
			}
		}
	}
}

void ImagePatternsHandler::showPatternPreview(Image* frame, Image* frameOverlays)
{
	const Rectangle2D<int> region = determineCaptureRegion(frame);
	showPixelApproximations(frame, frameOverlays, region);
}

void ImagePatternsHandler::showTestRegionPixelApproximations(Image* frame, Image* frameOverlays)
{
	const Rectangle2D<int> region = determineTestRegion(frame);
	showPixelApproximations(frame, frameOverlays, region);
}

bool ImagePatternsHandler::shouldCapturePattern() const
{
	return wantsToCapturePattern;
}

void ImagePatternsHandler::capturePattern(Image* frame)
{
	const Rectangle2D<int> region = determineCaptureRegion(frame);
	ImagePattern* selectedImagePattern = getSelectedImagePattern();
	if (selectedImagePattern != nullptr) {
		selectedImagePattern->capturePattern(frame, region, transparentPixelsColor);
	}
	wantsToCapturePattern = false;
}

bool ImagePatternsHandler::shouldShowTestPatternOverlays() const
{
	return showTestPatternOptions->getValue();
}

void ImagePatternsHandler::showTestPatternOverlays(Image* frame, Image* frameOverlays)
{
	const Rectangle2D<int> region = determineTestRegion(frame);
	frameOverlays->highlightRectangularRegion(region.topLeft, region.size, 2, characteristicColor());

	for (const Point2D<int>& patternTestResult : patternTestResults) {
		frameOverlays->drawCross(patternTestResult, 5, 1, Pixel(255, 255, 0));
	}
}

bool ImagePatternsHandler::shouldTestPattern() const
{
	return wantsToTestPattern;
}

void ImagePatternsHandler::testPattern(Image* frame, Image* frameOverlays)
{
	wantsToTestPattern = false;
	patternTestResults.clear();

	const Rectangle2D<int> region = determineTestRegion(frame);

	ImagePattern* selectedImagePattern = getSelectedImagePattern();
	if (selectedImagePattern != nullptr) {
		Chronometer chronometer;

		if (testAcrossWholeTestRegion->getValue()) {
			selectedImagePattern->performSearchForAllOccurrences(patternTestResults, frame, region);
		}
		else {
			selectedImagePattern->performSearchForFirstOccurrence(patternTestResults, frame, region);
		}

		const double timeElapsedInMs = chronometer.TimeElapsed(Chronometer::Unit::Milliseconds);
		timeTakenByTestInMs = std::to_string(timeElapsedInMs);
	}
}

std::vector<std::string> ImagePatternsHandler::getImagePatternsNames() const
{
	return imagePatternsNames->getOptions();
}

Pixel ImagePatternsHandler::characteristicColor()
{
	return Pixel(163, 150, 97);
}

Pixel ImagePatternsHandler::getTransparentPixelsColor() const
{
	return transparentPixelsColor;
}