#include "ImagePattern.h"

#include "imgui.h"

#include <algorithm>
#include <vector>

#include "Common/FilesystemUtils.h"
#include "Common/TravailleurUtils.h"
#include "Common/SimplePersistenceUtils.h"
#include "ImGuiWrappers/ImGuiColorOutput.h"
#include "ImGuiWrappers/ImGuiSimpleDoubleSlider.h"
#include "ImGuiWrappers/ImGuiTextOutput.h"
#include "TravailleurContext.h"

double ImagePattern::PatternColor::defaultMinPercentageSimilarityNeededToApproximate = 80.0;
double ImagePattern::PatternColor::defaultMinMatchesPercentageToStillConsiderMatch = 0.0;
std::vector<std::unique_ptr<ImagePattern::ImageApproximationBufferData>> ImagePattern::imageApproximationBufferDatas;
std::mutex ImagePattern::mutex;

constexpr size_t maxNOfPatternColors = 16;

constexpr int width4k = 3840;
constexpr int height4k = 2160;

const int ImagePattern::bufferWidth = width4k;
const int ImagePattern::bufferHeight = height4k;

ImagePattern::ImagePattern(const std::string& name, TravailleurContext* travailleurContext) :
    name(name),
	travailleurContext(travailleurContext)
{
	assureSaveDirectoryExists();

	pattern = std::make_unique<Image>();
	refinedPattern = std::make_unique<Image>(*pattern);
	imageRenderer = std::make_unique<ImGuiImageRenderer>();

	searchStartCombo = std::make_unique<ImGuiCombo>("Search start");
	initializeSearchStartCombo();
	searchTypeCombo = std::make_unique<ImGuiCombo>("Search type");
	initializeSearchTypeCombo();

	usesImagePreprocessingToCaptureCheckbox = std::make_unique<ImGuiCheckbox>("Uses image preprocessing to capture", false);
	imagePreprocessingToUseToCaptureList = std::make_unique<ImGuiSingleSelectionList>("Image preprocessing to use (capture)");

	usesImagePreprocessingToDetectCheckbox = std::make_unique<ImGuiCheckbox>("Uses image preprocessing to detect", false);
	imagePreprocessingToUseToDetectList = std::make_unique<ImGuiSingleSelectionList>("Image preprocessing to use (detection)");

	assureInitialImageApproximationBuffersAreCreated();

	initOptimizationBuffers();

	useMemorizationTechnique = std::make_unique<ImGuiCheckbox>("Use memorization technique", false);

	useOptimizedVersion = std::make_unique<ImGuiCheckbox>("Use optimized version", false);

	showPatternRefinementUtils = std::make_unique<ImGuiCheckbox>("Show pattern refinement utils", false);
	useRefinedPattern = std::make_unique<ImGuiCheckbox>("Use refined pattern", false);

	showEraseBordersRefinement = std::make_unique<ImGuiCheckbox>("Show erase borders refinement", false);
	thicknessEraseBordersRefinement = std::make_unique<ImGuiIntInput>("thicknessEraseBordersRefinement");
	referencePatternColorEraseBordersRefinement = std::make_unique<ImGuiIntInput>("referencePatternColorEraseBordersRefinement");
	patternColorToRemoveEraseBordersRefinement = std::make_unique<ImGuiIntInput>("patternColorToRemoveEraseBordersRefinement");

	showImageEditorRefinement = std::make_unique<ImGuiCheckbox>("Show image editor refinement", false);

	imageEditor = std::make_unique<ImGuiImageEditor>("image pattern editor");

	load();

	updateImagePatternColorsInImageEditor();
}

ImagePattern::~ImagePattern()
{
	save();

	deinitOptimizationBuffers();
}

void ImagePattern::save() const
{
	pattern->saveToBinaryFile(getSaveDirectory() + "\\pattern");
	refinedPattern->saveToBinaryFile(getSaveDirectory() + "\\refinedPattern");
	savePatternColors();
	SimplePersistenceUtils::saveInt32("search-start", (int32_t)getSearchStart(), getSaveDirectory());
	SimplePersistenceUtils::saveInt32("search-type", (int32_t)getSearchType(), getSaveDirectory());

	SimplePersistenceUtils::saveBool("uses-image-preprocessing-to-capture", usesImagePreprocessingToCapture(), getSaveDirectory());

	SimplePersistenceUtils::saveBool("uses-image-preprocessing-to-detect", usesImagePreprocessingToDetect(), getSaveDirectory());

	SimplePersistenceUtils::saveBool("use-memorization-technique", usesMemorizationTechnique(), getSaveDirectory());

	SimplePersistenceUtils::saveBool("use-optimized-version", usesOptimizedVersion(), getSaveDirectory());

	saveImagePreprocessingToUseToCapture();
	saveImagePreprocessingToUseToDetect();

	SimplePersistenceUtils::saveBool("show-pattern-refinement-utils", showPatternRefinementUtils->getValue(), getSaveDirectory());
	SimplePersistenceUtils::saveBool("use-refined-pattern", useRefinedPattern->getValue(), getSaveDirectory());
	
	SimplePersistenceUtils::saveBool("show-erase-borders-refinement", showEraseBordersRefinement->getValue(), getSaveDirectory());
	SimplePersistenceUtils::saveInt32("thickness-erase-borders-refinement", thicknessEraseBordersRefinement->getValue(), getSaveDirectory());
	SimplePersistenceUtils::saveInt32("reference-pattern-color-erase-borders-refinement", referencePatternColorEraseBordersRefinement->getValue(), getSaveDirectory());
	SimplePersistenceUtils::saveInt32("pattern-color-to-remove-erase-borders-refinement", patternColorToRemoveEraseBordersRefinement->getValue(), getSaveDirectory());

	SimplePersistenceUtils::saveBool("show-image-editor-refinement", showImageEditorRefinement->getValue(), getSaveDirectory());
}

void ImagePattern::load()
{
	pattern->loadFromBinaryFile(getSaveDirectory() + "\\pattern");
	refinedPattern->loadFromBinaryFile(getSaveDirectory() + "\\refinedPattern");
	loadPatternColors();
	int32_t loadedValue = 0;
	SimplePersistenceUtils::loadInt32("search-start", loadedValue, getSaveDirectory());
	searchStartCombo->setSelectedIndex(loadedValue);
	SimplePersistenceUtils::loadInt32("search-type", loadedValue, getSaveDirectory());
	searchTypeCombo->setSelectedIndex(loadedValue);

	bool usesImagePreprocessingToCaptureCheckboxValue = false;
	SimplePersistenceUtils::loadBool("uses-image-preprocessing-to-capture", usesImagePreprocessingToCaptureCheckboxValue, getSaveDirectory());
	usesImagePreprocessingToCaptureCheckbox->setValue(usesImagePreprocessingToCaptureCheckboxValue);

	bool usesImagePreprocessingToDetectCheckboxValue = false;
	SimplePersistenceUtils::loadBool("uses-image-preprocessing-to-detect", usesImagePreprocessingToDetectCheckboxValue, getSaveDirectory());
	usesImagePreprocessingToDetectCheckbox->setValue(usesImagePreprocessingToDetectCheckboxValue);

	bool useMemorizationTechniqueCheckboxValue = false;
	SimplePersistenceUtils::loadBool("use-memorization-technique", useMemorizationTechniqueCheckboxValue, getSaveDirectory());
	useMemorizationTechnique->setValue(useMemorizationTechniqueCheckboxValue);

	bool useOptimizedVersionCheckboxValue = false;
	SimplePersistenceUtils::loadBool("use-optimized-version", useOptimizedVersionCheckboxValue, getSaveDirectory());
	useOptimizedVersion->setValue(useOptimizedVersionCheckboxValue);

	bool showPatternRefinementUtilsCheckboxValue = false;
	SimplePersistenceUtils::loadBool("show-pattern-refinement-utils", showPatternRefinementUtilsCheckboxValue, getSaveDirectory());
	showPatternRefinementUtils->setValue(showPatternRefinementUtilsCheckboxValue);

	bool useRefinedPatternCheckboxValue = false;
	SimplePersistenceUtils::loadBool("use-refined-pattern", useRefinedPatternCheckboxValue, getSaveDirectory());
	useRefinedPattern->setValue(useRefinedPatternCheckboxValue);

	bool showEraseBordersRefinementCheckboxValue = false;
	SimplePersistenceUtils::loadBool("show-erase-borders-refinement", showEraseBordersRefinementCheckboxValue, getSaveDirectory());
	showEraseBordersRefinement->setValue(showEraseBordersRefinementCheckboxValue);

	int thicknessEraseBordersRefinementLoadedValue = 0;
	SimplePersistenceUtils::loadInt32("thickness-erase-borders-refinement", thicknessEraseBordersRefinementLoadedValue, getSaveDirectory());
	thicknessEraseBordersRefinement->setValue(thicknessEraseBordersRefinementLoadedValue);

	int referencePatternColorEraseBordersRefinementLoadedValue = 0;
	SimplePersistenceUtils::loadInt32("reference-pattern-color-erase-borders-refinement", referencePatternColorEraseBordersRefinementLoadedValue, getSaveDirectory());
	referencePatternColorEraseBordersRefinement->setValue(referencePatternColorEraseBordersRefinementLoadedValue);

	int patternColorToRemoveEraseBordersRefinementLoadedValue = 0;
	SimplePersistenceUtils::loadInt32("pattern-color-to-remove-erase-borders-refinement", patternColorToRemoveEraseBordersRefinementLoadedValue, getSaveDirectory());
	patternColorToRemoveEraseBordersRefinement->setValue(patternColorToRemoveEraseBordersRefinementLoadedValue);

	bool showImageEditorRefinementCheckboxValue = false;
	SimplePersistenceUtils::loadBool("show-image-editor-refinement", showImageEditorRefinementCheckboxValue, getSaveDirectory());
	showImageEditorRefinement->setValue(showImageEditorRefinementCheckboxValue);
}

void ImagePattern::saveImagePreprocessingToUseToCapture() const
{
	const std::string imagePreprocessingToUseToCaptureName = imagePreprocessingToUseToCaptureList->getSelectedOption();
	SimplePersistenceUtils::saveString("image-preprocessing-to-use-to-capture-name", imagePreprocessingToUseToCaptureName, getSaveDirectory());
}

void ImagePattern::saveImagePreprocessingToUseToDetect() const
{
	const std::string imagePreprocessingToUseToDetectName = imagePreprocessingToUseToDetectList->getSelectedOption();
	SimplePersistenceUtils::saveString("image-preprocessing-to-use-to-detect-name", imagePreprocessingToUseToDetectName, getSaveDirectory());
}

bool ImagePattern::usesImagePreprocessingToCapture() const
{
	return usesImagePreprocessingToCaptureCheckbox->getValue();
}

bool ImagePattern::usesImagePreprocessingToDetect() const
{
	return usesImagePreprocessingToDetectCheckbox->getValue();
}

bool ImagePattern::usesMemorizationTechnique() const
{
	return useMemorizationTechnique->getValue();
}

bool ImagePattern::usesOptimizedVersion() const
{
	return useOptimizedVersion->getValue();
}

ImagePreprocessing* ImagePattern::getImagePreprocessingToUseToCapture() const
{
	return usesImagePreprocessingToCapture() ? travailleurContext->getImagePreprocessing(imagePreprocessingToUseToCaptureList->getSelectedOption()) : nullptr;
}

ImagePreprocessing* ImagePattern::getImagePreprocessingToUseToDetect() const
{
	return usesImagePreprocessingToDetect() ? travailleurContext->getImagePreprocessing(imagePreprocessingToUseToDetectList->getSelectedOption()) : nullptr;
}

void ImagePattern::updateImagePreprocessingToUseToCaptureSingleSelectionList(const std::vector<std::string>& options)
{
	imagePreprocessingToUseToCaptureList->setOptions(options);
	updateImagePreprocessingToUseToCaptureListBasedOnSave();
}

void ImagePattern::updateImagePreprocessingToUseToCaptureListBasedOnSave()
{
	std::string imagePreprocessingToUseToCaptureName;
	SimplePersistenceUtils::loadString("image-preprocessing-to-use-to-capture-name", imagePreprocessingToUseToCaptureName, getSaveDirectory());
	imagePreprocessingToUseToCaptureList->setSelectedOption(imagePreprocessingToUseToCaptureName);
}

void ImagePattern::updateImagePreprocessingToUseToDetectSingleSelectionList(const std::vector<std::string>& options)
{
	imagePreprocessingToUseToDetectList->setOptions(options);
	updateImagePreprocessingToUseToDetectListBasedOnSave();
}

void ImagePattern::updateImagePreprocessingToUseToDetectListBasedOnSave()
{
	std::string imagePreprocessingToUseToDetectName;
	SimplePersistenceUtils::loadString("image-preprocessing-to-use-to-detect-name", imagePreprocessingToUseToDetectName, getSaveDirectory());
	imagePreprocessingToUseToDetectList->setSelectedOption(imagePreprocessingToUseToDetectName);
}

void ImagePattern::initializeSearchStartCombo()
{
	std::vector<std::string> options;
	options.push_back("TopLeft");
	options.push_back("TopRight");
	options.push_back("BottomLeft");
	options.push_back("BottomRight");
	searchStartCombo->setOptions(options);
}

void ImagePattern::initializeSearchTypeCombo()
{
	std::vector<std::string> options;
	options.push_back("HorizontalFirst");
	options.push_back("VerticalFirst");
	searchTypeCombo->setOptions(options);
}

ImagePattern::SearchStart ImagePattern::getSearchStart() const
{
	return (ImagePattern::SearchStart)searchStartCombo->getSelectedIndex();
}

ImagePattern::SearchType ImagePattern::getSearchType() const
{
	return (ImagePattern::SearchType)searchTypeCombo->getSelectedIndex();
}

void ImagePattern::savePatternColors() const
{
	FILE* f = fopen((getSaveDirectory() + "\\pattern-colors").c_str(), "wb");
	if (f != nullptr) {
		const size_t nPatternColors = patternColors.size();
		fwrite(&nPatternColors, sizeof(size_t), 1, f);
		for (size_t i = 0; i < nPatternColors; ++i) {
			const PatternColor& patternColor = patternColors[i];
			fwrite(&patternColor.color.r, sizeof(uint8_t), 1, f);
			fwrite(&patternColor.color.g, sizeof(uint8_t), 1, f);
			fwrite(&patternColor.color.b, sizeof(uint8_t), 1, f);
			fwrite(&patternColor.color.a, sizeof(uint8_t), 1, f);
			fwrite(&patternColor.minPercentageSimilarityNeededToApproximate, sizeof(double), 1, f);
			fwrite(&patternColor.minMatchesPercentageToStillConsiderMatch, sizeof(double), 1, f);
		}
		fclose(f);
	}
}

void ImagePattern::loadPatternColors()
{
	FILE* f = fopen((getSaveDirectory() + "\\pattern-colors").c_str(), "rb");
	if (f != nullptr) {
		size_t nPatternColors = 0;
		fread((void*)&nPatternColors, sizeof(size_t), 1, f);
		for (size_t i = 0; i < nPatternColors; ++i) {
			int8_t r = 0;
			int8_t g = 0;
			int8_t b = 0;
			int8_t a = 255;
			double minPercentageSimilarityNeededToApproximate = PatternColor::defaultMinPercentageSimilarityNeededToApproximate;
			double minMatchesPercentageToStillConsiderMatch = PatternColor::defaultMinMatchesPercentageToStillConsiderMatch;
			fread(&r, sizeof(uint8_t), 1, f);
			fread(&g, sizeof(uint8_t), 1, f);
			fread(&b, sizeof(uint8_t), 1, f);
			fread(&a, sizeof(uint8_t), 1, f);
			fread(&minPercentageSimilarityNeededToApproximate, sizeof(double), 1, f);
			fread(&minMatchesPercentageToStillConsiderMatch, sizeof(double), 1, f);
			PatternColor patternColor(Pixel(r, g, b, a));
			patternColor.minPercentageSimilarityNeededToApproximate = minPercentageSimilarityNeededToApproximate;
			patternColor.minMatchesPercentageToStillConsiderMatch = minMatchesPercentageToStillConsiderMatch;
			patternColor.updateMaximumSquaredDistanceToConsiderApproximation();
			patternColors.push_back(patternColor);
		}
		fclose(f);
	}
}

std::string ImagePattern::getSaveDirectory() const
{
	return TravailleurUtils::getImagePatternsPath(travailleurContext->getName()) + "\\" + name;
}

void ImagePattern::assureSaveDirectoryExists()
{
	FilesystemUtils::assureDirectoryExists(getSaveDirectory());
}

const std::string& ImagePattern::getName() const
{
	return name;
}

void ImagePattern::setName(const std::string& name)
{
	this->name = name;
}

void ImagePattern::showGui()
{
    ImGuiTextOutput(name);

	searchStartCombo->showGui();
	searchTypeCombo->showGui();

	if (patternColors.size() == 0) {
		ImGuiTextOutput("no pattern colors");
	}
	else {

		int indexToDelete = -1;

		for (size_t i = 0; i < patternColors.size(); ++i) {
			PatternColor& patternColor = patternColors[i];
			const std::string patternColorsName = "Pattern color #" + std::to_string(i + 1);
			ImGuiColorOutput(patternColorsName, patternColor.color);
			
			const std::string minPercentageSimilarityNeededToApproximatesName = patternColorsName + "'s minPercentageSimilarityNeededToApproximate";
			const double previousMinPercentageSimilarityNeededToApproximate = patternColor.minPercentageSimilarityNeededToApproximate;
			ImGuiSimpleDoubleSlider(minPercentageSimilarityNeededToApproximatesName, &patternColor.minPercentageSimilarityNeededToApproximate, 1.0, 0, 100);
			if (patternColor.minPercentageSimilarityNeededToApproximate != previousMinPercentageSimilarityNeededToApproximate) {
				patternColor.updateMaximumSquaredDistanceToConsiderApproximation();
			}

			const std::string minMatchesPercentageToStillConsiderMatchName = patternColorsName + "'s minMatchesPercentageToStillConsiderMatch";
			ImGuiSimpleDoubleSlider(minMatchesPercentageToStillConsiderMatchName, &patternColor.minMatchesPercentageToStillConsiderMatch, 1.0, 0, 100);

			if (ImGui::Button(("Remove pattern color #" + std::to_string(i + 1)).c_str())) {
				indexToDelete = i;
			}
		}

		if (indexToDelete >= 0) {
			removeColor(indexToDelete);
		}

	}

	usesImagePreprocessingToCaptureCheckbox->showGui();
	if (usesImagePreprocessingToCapture()) {
		imagePreprocessingToUseToCaptureList->showGui();
	}

	usesImagePreprocessingToDetectCheckbox->showGui();
	if (usesImagePreprocessingToDetect()) {
		imagePreprocessingToUseToDetectList->showGui();
	}

	useMemorizationTechnique->showGui();
	if (useMemorizationTechnique->getValue()) {
		ImGuiTextOutput("# of memorization infos: " + std::to_string(mapFromSearchRegionToMemorizationInfo.size()));
		if (ImGui::Button("Clear memorization technique information")) {
			clearMemorizationTechniqueInformation();
		}
	}

	useOptimizedVersion->showGui();

	showPatternRefinementUtils->showGui();
	if (showPatternRefinementUtils->getValue()) {
		showPatternRefinementUtilsGui();
	}

	useRefinedPattern->showGui();

	imageRenderer->renderImage(*getPatternBeingUsed());
}

void ImagePattern::showPatternRefinementUtilsGui()
{
	if (ImGui::Button("restart refined pattern")) {
		refinedPattern = std::make_unique<Image>(*pattern);
	}

	if (ImGui::Button("change tranparent pixels to transparent color")) {
		refinedPattern->changeAllPixelsOfSpecificColor(getImagePatternsHandler()->getTransparentPixelsColor(), Pixel::transparentPixel());
	}

	if (ImGui::Button("change tranparent color pixels to transparent")) {
		refinedPattern->changeAllPixelsOfSpecificColor(Pixel::transparentPixel(), getImagePatternsHandler()->getTransparentPixelsColor());
	}

	showEraseBordersRefinement->showGui();
	if (showEraseBordersRefinement->getValue()) {
		thicknessEraseBordersRefinement->showGui();
		referencePatternColorEraseBordersRefinement->showGui();
		patternColorToRemoveEraseBordersRefinement->showGui();
		if (ImGui::Button("apply erase borders refinement")) {
			applyEraseBordersRefinement();
		}
	}

	showImageEditorRefinement->showGui();
	if (showImageEditorRefinement->getValue()) {

		imageEditor->setTransparentPixelColor(getImagePatternsHandler()->getTransparentPixelsColor());
		imageEditor->showGui();

		if (ImGui::Button("update image into editor")) {
			imageEditor->setImage(*getPatternBeingUsed());
		}
		if (ImGui::Button("update image from editor")) {
			*getPatternBeingUsed() = imageEditor->getImage();
		}
	}
}

const Pixel* ImagePattern::getPatternColorAtIndex(const int index) const
{
	if (index < 0 || index >= patternColors.size()) return nullptr;
	return &patternColors[index].color;
}

ImagePatternsHandler* ImagePattern::getImagePatternsHandler() const
{
	return travailleurContext->getImagePatternsHandler();
}

void ImagePattern::applyEraseBordersRefinement()
{
	const Pixel* referencePatternColor = getPatternColorAtIndex(referencePatternColorEraseBordersRefinement->getValue() - 1);
	if (referencePatternColor == nullptr) {
		TravailleurUtils::showErrorMessage("error: could not determine referencePatternColor");
		return;
	}
	const Pixel* patternColorToRemove = getPatternColorAtIndex(patternColorToRemoveEraseBordersRefinement->getValue() - 1);
	if (patternColorToRemove == nullptr) {
		TravailleurUtils::showErrorMessage("error: could not determine patternColorToRemove");
		return;
	}
	if (referencePatternColor == patternColorToRemove) {
		TravailleurUtils::showErrorMessage("error: referencePatternColor and patternColorToRemove cannot be the same");
		return;
	}
	const int thickness = thicknessEraseBordersRefinement->getValue();
	if (thickness <= 0) {
		TravailleurUtils::showErrorMessage("error: thickness must be a positive value");
		return;
	}

	for (size_t i = 0; i < refinedPattern->getWidth(); ++i) {
		for (size_t j = 0; j < refinedPattern->getHeight(); ++j) {
			const Pixel currentPixel = refinedPattern->pixelAt(i, j);
			if (currentPixel == *referencePatternColor) {
				const int squareStartX = i - thickness;
				const int squareEndX = i + thickness;
				const int squareStartY = j - thickness;
				const int squareEndY = j + thickness;

				for (size_t k = squareStartX; k <= squareEndX; ++k) {
					for (size_t l = squareStartY; l <= squareEndY; ++l) {
						if (refinedPattern->isValidPixelLocation(k, l)) {
							if (k != i || l != j) {
								const Pixel pixelBeingPossiblyRemoved = refinedPattern->pixelAt(k, l);
								if (pixelBeingPossiblyRemoved == *patternColorToRemove) {
									refinedPattern->setPixel(k, l, Pixel::transparentPixel());
								}
							}
						}
					}
				}
			}
		}
	}
}

bool ImagePattern::shouldUseRefinedPattern() const
{
	return useRefinedPattern->getValue();
}

bool ImagePattern::hasColor(const Pixel& color) const
{
	return std::any_of(patternColors.begin(), patternColors.end(), [&color](const PatternColor& patternColor) {
		return patternColor.color == color;
    });
}

void ImagePattern::addColor(const Pixel& color)
{
	if (patternColors.size() < maxNOfPatternColors && !hasColor(color)) {
		const PatternColor newPatternColor(color);
		patternColors.push_back(newPatternColor);
	}

	patternColorsChanged();
}

void ImagePattern::removeColor(const int index)
{
	if (index >= 0 && index + 1 <= patternColors.size()) {
		const PatternColor& patternColor = patternColors[index];
		for (int i = 0; i < pattern->getWidth(); ++i) {
			for (int j = 0; j < pattern->getHeight(); ++j) {
				if (pattern->pixelAt(i, j) == patternColor.color) {
					pattern->setPixel(i, j, Pixel(0, 0, 0, 0));
				}
			}
		}
		patternColors.erase(patternColors.begin() + index);
	}

	patternColorsChanged();
}

void ImagePattern::patternColorsChanged()
{
	imageEditor->clearImage();
	updateImagePatternColorsInImageEditor();
}

void ImagePattern::updateImagePatternColorsInImageEditor()
{
	imageEditor->clearColorsToInsert();
	imageEditor->addColorToInsert("transparent", Pixel::transparentPixel());
	for (size_t i = 0; i < patternColors.size(); ++i) {
		const PatternColor& patternColor = patternColors[i];
		imageEditor->addColorToInsert("patternColor" + std::to_string(i), patternColor.color);
	}
}

Pixel ImagePattern::getPixelApproximation(const Pixel& pixel, const Pixel transparentPixelsColor) const
{
	Pixel transparentPixel(0, 0, 0, 0);
	if (pixel == transparentPixelsColor) {
		return transparentPixel;
	}
	Pixel& approximation = transparentPixel;
	const int squareOfPixelPosition = pixel.getPositionSquared();
	int currentSmallestPixelDistance = Pixel::getMaximumPossiblePositionSquared() + 1;
	for (const PatternColor& patternColor : patternColors) {
		const int squareOfCurrentPatternColorPosition = patternColor.squareOfPosition;
		const int squareOfDistance = abs(squareOfPixelPosition - squareOfCurrentPatternColorPosition);
		if (squareOfDistance <= patternColor.maximumSquaredDistanceToConsiderApproximation && squareOfDistance < currentSmallestPixelDistance) {
			currentSmallestPixelDistance = squareOfDistance;
			approximation = patternColor.color;
		}
	}
	return approximation;
}

void ImagePattern::capturePattern(Image* frame, const Rectangle2D<int>& captureRegion, const Pixel& transparentPixelsColor)
{
	if (!captureRegion.isValid()) {
		pattern = std::make_unique<Image>();
		refinedPattern = std::make_unique<Image>(*pattern);
		return;
	}

	pattern = std::make_unique<Image>(*frame);
	pattern->crop(captureRegion);

	if (usesImagePreprocessingToCapture()) {
		ImagePreprocessing* imagePreprocessingToUse = getImagePreprocessingToUseToCapture();
		if (imagePreprocessingToUse == nullptr) {
			pattern = std::make_unique<Image>();
			return;
		}
		imagePreprocessingToUse->apply(pattern.get());
	}

	for (int i = 0; i < pattern->getWidth(); ++i) {
		for (int j = 0; j < pattern->getHeight(); ++j) {
			const Pixel pixel = pattern->pixelAt(i, j);
			const Pixel approximatedPixel = getPixelApproximation(pixel, transparentPixelsColor);
			pattern->setPixel(i, j, approximatedPixel);
		}
	}

	refinedPattern = std::make_unique<Image>(*pattern);
}

bool ImagePattern::performSearchForAllOccurrences(std::vector<Point2D<int>>& occurrences, Image* image, const Rectangle2D<int>& searchRegion) const
{
	performSearch(occurrences, image, searchRegion, false);
	return occurrences.size() != 0;
}

bool ImagePattern::performSearchForFirstOccurrence(std::vector<Point2D<int>>& occurrences, Image* image, const Rectangle2D<int>& searchRegion) const
{
	performSearch(occurrences, image, searchRegion, true);
	return occurrences.size() != 0;
}

bool ImagePattern::occursIn(Image* image, const Rectangle2D<int> searchRegion) const
{
	std::vector<Point2D<int>> occurrences;
	return performSearchForFirstOccurrence(occurrences, image, searchRegion);
}

void ImagePattern::performSearch(std::vector<Point2D<int>>& occurrences, Image* image, const Rectangle2D<int>& searchRegion, const bool stopSearchOnFirstOccurrence) const
{
	occurrences.clear();

	if (!searchRegion.isValid()) return;

	if (usesImagePreprocessingToDetect()) {
		ImagePreprocessing* imagePreprocessingToUse = getImagePreprocessingToUseToDetect();
		if (imagePreprocessingToUse == nullptr) return;
		Image preprocessedImage(*image);
		preprocessedImage.crop(searchRegion);
		imagePreprocessingToUse->apply(&preprocessedImage);
		if (preprocessedImage.getWidth() >= bufferWidth || preprocessedImage.getHeight() >= bufferHeight) return;

		acquireImageApproximationBufferForCurrentThread();

		computeImageApproximationConsideringWholeImage(&preprocessedImage);
		if (usesOptimizedVersion()) {
			fillOptimizationBuffers(&preprocessedImage, searchRegion);
		}

		const Rectangle2D region(Point2D(0, 0), Point2D<int>(preprocessedImage.getWidth(), preprocessedImage.getHeight()));
		iterateAppropriatedlyOverApproximatedPixelsAndCheckMatches(&preprocessedImage, occurrences, region, stopSearchOnFirstOccurrence);

		releaseImageApproximationBufferForCurrentThread();

		// if the dimensions were changed because of some image preprocessing step, we gotta scale the occurrences to match the original size
		if (preprocessedImage.getWidth() != searchRegion.getWidth() || preprocessedImage.getHeight() != searchRegion.getHeight()) {
			const double ratioX = (double)(searchRegion.getWidth()) / preprocessedImage.getWidth();
			const double ratioY = (double)(searchRegion.getHeight()) / preprocessedImage.getHeight();
			for (Point2D<int>& occurrence : occurrences) {
				occurrence.x = occurrence.x * ratioX;
				occurrence.y = occurrence.y * ratioY;
			}
		}

		for (Point2D<int>& occurrence : occurrences) {
			occurrence.x += searchRegion.getTopLeft().x;
			occurrence.y += searchRegion.getTopLeft().y;
		}
	}
	else {
		acquireImageApproximationBufferForCurrentThread();

		computeImageApproximation(image, searchRegion);
		if (usesOptimizedVersion()) {
			fillOptimizationBuffers(image, searchRegion);
		}

		iterateAppropriatedlyOverApproximatedPixelsAndCheckMatches(image, occurrences, searchRegion, stopSearchOnFirstOccurrence);
		
		releaseImageApproximationBufferForCurrentThread();
	}

	updateMemorizationInfoBasedOnDetections(image, occurrences, searchRegion);
}

void ImagePattern::computeImageApproximationConsideringWholeImage(Image* image) const
{
	computeImageApproximation(image, image->getRegion());
}

void ImagePattern::computeImageApproximation(Image* image, const Rectangle2D<int>& searchRegion) const
{
	Image* imageApproximationBuffer = getImageApproximationBufferForCurrentThread();
	for (int i = 0; i < searchRegion.size.x; ++i) {
		for (int j = 0; j < searchRegion.size.y; ++j) {
			const Pixel& pixel = image->pixelAt(searchRegion.topLeft.x + i, searchRegion.topLeft.y + j);
			imageApproximationBuffer->setPixel(i, j, getPixelApproximation(pixel));
		}
	}
}

void ImagePattern::updateMemorizationInfoBasedOnDetections(Image* image, const std::vector<Point2D<int>>& occurrences, const Rectangle2D<int>& searchRegion) const
{
	mutex.lock();

	if (lastImageThePatternWasSearchedIn == nullptr || *lastImageThePatternWasSearchedIn != *image) {
		lastImageThePatternWasSearchedIn = std::make_unique<Image>(*image);
		mapFromSearchRegionToMemorizationInfo.clear();
	}

	auto it = mapFromSearchRegionToMemorizationInfo.find(searchRegion);
	if (it == mapFromSearchRegionToMemorizationInfo.end()) {
		MemorizationInfo& memorizationInfo = mapFromSearchRegionToMemorizationInfo[searchRegion];
		for (const Point2D<int> occurrence : occurrences) {
			memorizationInfo.memorizedOccurrences.push_back(occurrence);
		}
		memorizationInfo.memorizedOccurrences = occurrences;
	}

	mutex.unlock();
}

void ImagePattern::checkForMemorizedDetections(Image* image, std::vector<Point2D<int>>& occurrences, const Rectangle2D<int>& searchRegion) const
{
	mutex.lock();

	if (lastImageThePatternWasSearchedIn != nullptr && *lastImageThePatternWasSearchedIn == *image) {
		auto it = mapFromSearchRegionToMemorizationInfo.find(searchRegion);
		if (it != mapFromSearchRegionToMemorizationInfo.end()) {
			MemorizationInfo& memorizationInfo = it->second;
			occurrences = memorizationInfo.memorizedOccurrences;
		}
	}

	mutex.unlock();
}

void ImagePattern::clearMemorizationTechniqueInformation()
{
	mapFromSearchRegionToMemorizationInfo.clear();
	lastImageThePatternWasSearchedIn = nullptr;
}

void ImagePattern::iterateAppropriatedlyOverApproximatedPixelsAndCheckMatches(Image* image, std::vector<Point2D<int>>& occurrences, const Rectangle2D<int>& searchRegion, const bool stopSearchOnFirstOccurrence) const
{
	if (usesMemorizationTechnique()) {
		checkForMemorizedDetections(image, occurrences, searchRegion);
		if (!occurrences.empty()) return;
	}

	if (getSearchStart() == SearchStart::TopLeft && getSearchType() == SearchType::HorizontalFirst) {
		for (int j = 0; j < searchRegion.size.y; ++j) {
			for (int i = 0; i < searchRegion.size.x; ++i) {
				if (checkForMatch(searchRegion, i, j)) {
					occurrences.push_back(Point2D<int>(i, j) + searchRegion.topLeft);
					if (stopSearchOnFirstOccurrence) return;
				}
			}
		}
	}
	if (getSearchStart() == SearchStart::TopLeft && getSearchType() == SearchType::VerticalFirst) {
		for (int i = 0; i < searchRegion.size.x; ++i) {
			for (int j = 0; j < searchRegion.size.y; ++j) {
				if (checkForMatch(searchRegion, i, j)) {
					occurrences.push_back(Point2D<int>(i, j) + searchRegion.topLeft);
					if (stopSearchOnFirstOccurrence) return;
				}
			}
		}
	}
	if (getSearchStart() == SearchStart::TopRight && getSearchType() == SearchType::HorizontalFirst) {
		for (int j = 0; j < searchRegion.size.y; ++j) {
			for (int i = searchRegion.size.x - 1; i >= 0; --i) {
				if (checkForMatch(searchRegion, i, j)) {
					occurrences.push_back(Point2D<int>(i, j) + searchRegion.topLeft);
					if (stopSearchOnFirstOccurrence) return;
				}
			}
		}
	}
	if (getSearchStart() == SearchStart::TopRight && getSearchType() == SearchType::VerticalFirst) {
		for (int i = searchRegion.size.x - 1; i >= 0; --i) {
			for (int j = 0; j < searchRegion.size.y; ++j) {
				if (checkForMatch(searchRegion, i, j)) {
					occurrences.push_back(Point2D<int>(i, j) + searchRegion.topLeft);
					if (stopSearchOnFirstOccurrence) return;
				}
			}
		}
	}
	if (getSearchStart() == SearchStart::BottomLeft && getSearchType() == SearchType::HorizontalFirst) {
		for (int j = searchRegion.size.y - 1; j >= 0; --j) {
			for (int i = 0; i < searchRegion.size.x; ++i) {
				if (checkForMatch(searchRegion, i, j)) {
					occurrences.push_back(Point2D<int>(i, j) + searchRegion.topLeft);
					if (stopSearchOnFirstOccurrence) return;
				}
			}
		}
	}
	if (getSearchStart() == SearchStart::BottomLeft && getSearchType() == SearchType::VerticalFirst) {
		for (int i = 0; i < searchRegion.size.x; ++i) {
			for (int j = searchRegion.size.y - 1; j >=0 ; --j) {
				if (checkForMatch(searchRegion, i, j)) {
					occurrences.push_back(Point2D<int>(i, j) + searchRegion.topLeft);
					if (stopSearchOnFirstOccurrence) return;
				}
			}
		}
	}
	if (getSearchStart() == SearchStart::BottomRight && getSearchType() == SearchType::HorizontalFirst) {
		for (int j = searchRegion.size.y - 1; j >= 0; --j) {
			for (int i = searchRegion.size.x - 1; i >= 0; --i) {
				if (checkForMatch(searchRegion, i, j)) {
					occurrences.push_back(Point2D<int>(i, j) + searchRegion.topLeft);
					if (stopSearchOnFirstOccurrence) return;
				}
			}
		}
	}
	if (getSearchStart() == SearchStart::BottomRight && getSearchType() == SearchType::VerticalFirst) {
		for (int i = searchRegion.size.x - 1; i >= 0; --i) {
			for (int j = searchRegion.size.y - 1; j >= 0; --j) {
				if (checkForMatch(searchRegion, i, j)) {
					occurrences.push_back(Point2D<int>(i, j) + searchRegion.topLeft);
					if (stopSearchOnFirstOccurrence) return;
				}
			}
		}
	}
}

bool ImagePattern::checkForMatch(const Rectangle2D<int>& searchRegion, const int x, const int y) const
{
	if (usesOptimizedVersion()) {
		return checkForMatch_optimized(searchRegion, x, y);
	}

	const Image* patternToUse = getPatternBeingUsed();

	if (patternColors.size() == 0 || searchRegion.size.x == 0 || searchRegion.size.y == 0 || x > searchRegion.getWidth() - pattern->getWidth() || y > searchRegion.getHeight() - patternToUse->getHeight()) return false;
	
	Image* imageApproximationBuffer = getImageApproximationBufferForCurrentThread();
	
	const size_t nPixelsInPattern = patternToUse->nPixels();
	for (const PatternColor& patternColor : patternColors) {
		const int maxWrongPixelsAllowed = floor((nPixelsInPattern * (100.0 - patternColor.minMatchesPercentageToStillConsiderMatch)) / 100.0);
		int wrongPixelsCount = 0;
		for (int j = y; j < y + patternToUse->getHeight(); ++j) {
			for (int i = x; i < x + patternToUse->getWidth(); ++i) {
				if (patternToUse->pixelAt(i - x, j - y) == patternColor.color) {
					const Pixel& approximatedPixel = imageApproximationBuffer->pixelAt(i, j);
					if (approximatedPixel != patternColor.color) {
						++wrongPixelsCount;
						if (wrongPixelsCount > maxWrongPixelsAllowed) {
							return false;
						}
					}
				}
			}
		}
	}
	return true;
}

Image* ImagePattern::getPatternBeingUsed() const
{
	return shouldUseRefinedPattern() ? refinedPattern.get() : pattern.get();
}

void ImagePattern::assureInitialImageApproximationBuffersAreCreated()
{
	static bool alreadyCreated = false;
	if (!alreadyCreated) {
		constexpr size_t initialNumberOfImageApproximationBuffers = 16;
		for (size_t i = 0; i < initialNumberOfImageApproximationBuffers; ++i) {
			addImageApproximationBuffer();
		}
		alreadyCreated = true;
	}
}

void ImagePattern::addImageApproximationBuffer()
{
	imageApproximationBufferDatas.push_back(std::make_unique<ImageApproximationBufferData>());
	ImageApproximationBufferData* imageApproximationBufferData = imageApproximationBufferDatas.back().get();
	imageApproximationBufferData->imageApproximationBuffer = std::make_unique<Image>(bufferWidth, bufferHeight);
}

void ImagePattern::acquireImageApproximationBufferForCurrentThread()
{
	mutex.lock();

	int index = -1;

	for (size_t i = 0; i < imageApproximationBufferDatas.size(); ++i) {
		ImageApproximationBufferData* imageApproximationBufferData = imageApproximationBufferDatas[i].get();
		if (!imageApproximationBufferData->isInUse) {
			index = i;
			break;
		}
	}

	if (index < 0) {
		addImageApproximationBuffer();
		index = imageApproximationBufferDatas.size() - 1;
	}

	ImageApproximationBufferData* imageApproximationBufferData = imageApproximationBufferDatas[index].get();
	imageApproximationBufferData->isInUse = true;
	imageApproximationBufferData->idOfThreadThatIsUsing = std::this_thread::get_id();

	mutex.unlock();
}

ImagePattern::ImageApproximationBufferData* ImagePattern::getImageApproximationBufferDataForCurrentThread()
{
	mutex.lock();

 	ImageApproximationBufferData* imageApproximationBufferDataBeingUsed = nullptr;

	for (size_t i = 0; i < imageApproximationBufferDatas.size(); ++i) {
		ImageApproximationBufferData* imageApproximationBufferData = imageApproximationBufferDatas[i].get();
		if (imageApproximationBufferData->isInUse && imageApproximationBufferData->idOfThreadThatIsUsing == std::this_thread::get_id()) {
			imageApproximationBufferDataBeingUsed = imageApproximationBufferData;
		}
	}

	assert(imageApproximationBufferDataBeingUsed != nullptr);

	mutex.unlock();

	return imageApproximationBufferDataBeingUsed;
}

Image* ImagePattern::getImageApproximationBufferForCurrentThread()
{
	ImageApproximationBufferData* imageApproximationBufferData = getImageApproximationBufferDataForCurrentThread();
	assert(imageApproximationBufferData != nullptr);
	return imageApproximationBufferData->imageApproximationBuffer.get();
}

void ImagePattern::releaseImageApproximationBufferForCurrentThread()
{
	ImageApproximationBufferData* imageApproximationBufferDataBeingUsed = getImageApproximationBufferDataForCurrentThread();

	assert(imageApproximationBufferDataBeingUsed != nullptr);

	mutex.lock();

	imageApproximationBufferDataBeingUsed->isInUse = false;

	mutex.unlock();
}