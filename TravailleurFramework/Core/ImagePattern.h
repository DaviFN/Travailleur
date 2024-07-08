#ifndef ImagePattern_H
#define ImagePattern_H

#include <memory>
#include <mutex>
#include <string>
#include <thread>

#include "Common/Image.h"
#include "Common/Point2D.h"
#include "Common/Rectangle2D.h"
#include "ImagePreprocessing.h"
#include "ImGuiWrappers/ImGuiCombo.h"
#include "ImGuiWrappers/ImGuiCheckbox.h"
#include "ImGuiWrappers/ImGuiImageEditor.h"
#include "ImGuiWrappers/ImGuiImageRenderer.h"
#include "ImGuiWrappers/ImGuiIntInput.h"
#include "ImGuiWrappers/ImGuiIntSlider.h"
#include "ImGuiWrappers/ImGuiSingleSelectionList.h"

class TravailleurContext;
class ImagePatternsHandler;

class ImagePattern
{
public:
	enum class SearchStart {
		TopLeft,
		TopRight,
		BottomLeft,
		BottomRight
	};

	enum class SearchType {
		HorizontalFirst,
		VerticalFirst
	};

	ImagePattern(const std::string& name, TravailleurContext* travailleurContext);
	~ImagePattern();

	const std::string& getName() const;
	void setName(const std::string& name);

	void showGui();

	bool hasColor(const Pixel& color) const;

	void addColor(const Pixel& color);

	void removeColor(const int index);

	Pixel getPixelApproximation(const Pixel& pixel, const Pixel transparentPixelsColor = Pixel(0, 0, 0, 0)) const;

	void capturePattern(Image* frame, const Rectangle2D<int>& captureRegion, const Pixel& transparentPixelsColor);

	bool performSearchForAllOccurrences(std::vector<Point2D<int>>& occurrences, Image* image, const Rectangle2D<int>& searchRegion) const;

	bool performSearchForFirstOccurrence(std::vector<Point2D<int>>& occurrences, Image* image, const Rectangle2D<int>& searchRegion) const;

	bool occursIn(Image* image, const Rectangle2D<int> searchRegion) const;

	bool usesImagePreprocessingToCapture() const;

	bool usesImagePreprocessingToDetect() const;

	ImagePreprocessing* getImagePreprocessingToUseToCapture() const;

	ImagePreprocessing* getImagePreprocessingToUseToDetect() const;

	void updateImagePreprocessingToUseToCaptureSingleSelectionList(const std::vector<std::string>& options);
	void updateImagePreprocessingToUseToDetectSingleSelectionList(const std::vector<std::string>& options);

	static const int bufferWidth;
	static const int bufferHeight;

	struct PatternColor {
		PatternColor(const Pixel& color) : color(color) {
			squareOfPosition = color.getPositionSquared();
		};

		static double defaultMinPercentageSimilarityNeededToApproximate;
		static double defaultMinMatchesPercentageToStillConsiderMatch;

		Pixel color;
		double minPercentageSimilarityNeededToApproximate = defaultMinPercentageSimilarityNeededToApproximate;
		double minMatchesPercentageToStillConsiderMatch = defaultMinMatchesPercentageToStillConsiderMatch;

		int squareOfPosition = 0; // cached value for performance reasons
		int maximumSquaredDistanceToConsiderApproximation = 0; // cached value for performance reasons

		void updateMaximumSquaredDistanceToConsiderApproximation()
		{
			const double percentageOfComponent = 255.0 - (255.0 * minPercentageSimilarityNeededToApproximate) / 100.0;
			const double& pc = percentageOfComponent;
			maximumSquaredDistanceToConsiderApproximation = floor(pc * pc + pc * pc + pc * pc);
		};
	};

private:
	struct ImageApproximationBufferData {
		std::unique_ptr<Image> imageApproximationBuffer;
		std::thread::id idOfThreadThatIsUsing;
		bool isInUse = false;
	};

	Image* getPatternBeingUsed() const;

	void assureInitialImageApproximationBuffersAreCreated();
	static void addImageApproximationBuffer();
	static void acquireImageApproximationBufferForCurrentThread();
	static ImagePattern::ImageApproximationBufferData* getImageApproximationBufferDataForCurrentThread();
	static Image* getImageApproximationBufferForCurrentThread();
	static void releaseImageApproximationBufferForCurrentThread();

	void performSearch(std::vector<Point2D<int>>& occurrences, Image* image, const Rectangle2D<int>& searchRegion, const bool stopSearchOnFirstOccurrence = true) const;
	void computeImageApproximationConsideringWholeImage(Image* image) const;
	void computeImageApproximation(Image* image, const Rectangle2D<int>& searchRegion) const;
	void iterateAppropriatedlyOverApproximatedPixelsAndCheckMatches(Image* image, std::vector<Point2D<int>>& occurrences, const Rectangle2D<int>& searchRegion, const bool stopSearchOnFirstOccurrence = true) const;
	bool checkForMatch(const Rectangle2D<int>& searchRegion, const int x, const int y) const;

	void save() const;
	void load();

	void savePatternColors() const;
	void loadPatternColors();

	void saveImagePreprocessingToUseToCapture() const;
	void saveImagePreprocessingToUseToDetect() const;
	void updateImagePreprocessingToUseToCaptureListBasedOnSave();
	void updateImagePreprocessingToUseToDetectListBasedOnSave();

	void initializeSearchStartCombo();
	void initializeSearchTypeCombo();

	std::string getSaveDirectory() const;
	void assureSaveDirectoryExists();

	bool usesMemorizationTechnique() const;

	void updateMemorizationInfoBasedOnDetections(Image* image, const std::vector<Point2D<int>>& occurrences, const Rectangle2D<int>& searchRegion) const;
	void checkForMemorizedDetections(Image* image, std::vector<Point2D<int>>& occurrences, const Rectangle2D<int>& searchRegion) const;
	void clearMemorizationTechniqueInformation();

	bool usesOptimizedVersion() const;

	void initOptimizationBuffers();
	void initImageOptimizationBuffer();
	void initPatternOptimizationBuffer();
	void deinitOptimizationBuffers();
	void deinitImageOptimizationBuffer();
	void deinitPatternOptimizationBuffer();

	void fillOptimizationBuffers(Image* image, const Rectangle2D<int>& searchRegion) const;
	void fillOptimizationBuffer(void* buffer, Image* image) const;
	bool checkForMatch_optimized(const Rectangle2D<int>& searchRegion, const int x, const int y) const;

	void showPatternRefinementUtilsGui();
	void applyEraseBordersRefinement();

	bool shouldUseRefinedPattern() const;

	const Pixel* getPatternColorAtIndex(const int index) const;

	ImagePatternsHandler* getImagePatternsHandler() const;

	void patternColorsChanged();
	void updateImagePatternColorsInImageEditor();

	std::string name;
	std::unique_ptr<Image> pattern;
	std::unique_ptr<Image> refinedPattern;

	std::vector<PatternColor> patternColors;

	std::unique_ptr<ImGuiImageRenderer> imageRenderer;

	static std::vector<std::unique_ptr<ImageApproximationBufferData>> imageApproximationBufferDatas;

	SearchStart getSearchStart() const;
	SearchType getSearchType() const;
	std::unique_ptr<ImGuiCombo> searchStartCombo;
	std::unique_ptr<ImGuiCombo> searchTypeCombo;

	std::unique_ptr<ImGuiCheckbox> usesImagePreprocessingToCaptureCheckbox;
	std::unique_ptr<ImGuiSingleSelectionList> imagePreprocessingToUseToCaptureList;
	std::unique_ptr<ImGuiCheckbox> usesImagePreprocessingToDetectCheckbox;
	std::unique_ptr<ImGuiSingleSelectionList> imagePreprocessingToUseToDetectList;

	// feature to memorize occurrences in a same region and image aiming primarily speeding up rectangular regions that are relative to image pattern detections
	std::unique_ptr<ImGuiCheckbox> useMemorizationTechnique;
	struct MemorizationInfo {
		std::vector<Point2D<int>> memorizedOccurrences;
	};
	mutable std::map<Rectangle2D<int>, MemorizationInfo> mapFromSearchRegionToMemorizationInfo;
	mutable std::unique_ptr<Image> lastImageThePatternWasSearchedIn;

	// attempt to speed up image pattern detection process; currently, there is noticeable gain of speeds only in debug version
	std::unique_ptr<ImGuiCheckbox> useOptimizedVersion;
	void* imageOptimizationBuffer = nullptr;
	void* patternOptimizationBuffer = nullptr;
	uint32_t* maxWrongPixelsAllowedOptimizationBuffer = nullptr;

	std::unique_ptr<ImGuiCheckbox> showPatternRefinementUtils;
	std::unique_ptr<ImGuiCheckbox> useRefinedPattern;

	std::unique_ptr<ImGuiCheckbox> showEraseBordersRefinement;
	std::unique_ptr<ImGuiIntInput> thicknessEraseBordersRefinement;
	std::unique_ptr<ImGuiIntInput> referencePatternColorEraseBordersRefinement;
	std::unique_ptr<ImGuiIntInput> patternColorToRemoveEraseBordersRefinement;

	std::unique_ptr<ImGuiCheckbox> showImageEditorRefinement;

	std::unique_ptr<ImGuiImageEditor> imageEditor;

	TravailleurContext* travailleurContext = nullptr;

	static std::mutex mutex;
};

#endif