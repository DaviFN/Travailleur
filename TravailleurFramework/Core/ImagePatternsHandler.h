#ifndef ImagePatternsHandler_H
#define ImagePatternsHandler_H

#include <map>
#include <memory>

#include "Common/Image.h"
#include "Common/Rectangle2D.h"

#include "ImagePattern.h"
#include "ImGuiWrappers/ImGuiCheckbox.h"
#include "ImGuiWrappers/ImGuiSingleSelectionList.h"
#include "ImGuiWrappers/ImGuiDoubleSlider.h"
#include "ImGuiWrappers/ImGuiImageRenderer.h"
#include "ImGuiWrappers/ImGuiPointInput.h"

class TravailleurContext;

class ImagePatternsHandler
{
public:
	ImagePatternsHandler(TravailleurContext* travailleurContext);
	~ImagePatternsHandler();

	void registerImagePattern(const std::string& name);

	void afterAppSetup();

	ImagePattern* getSelectedImagePattern() const;

	ImagePattern* getImagePattern(const std::string& name) const;

	void showFrameOverlays(Image* frame, Image* frameOverlays);

	void performActionsThatDependOnFrame(Image* frame, Image* frameOverlays);

	void showGui();

	bool shouldShowColorBeingPickedFromFrame() const;

	Point2D<int> positionOfColorBeingPickedFromFrame() const;

	std::vector<std::string> getImagePatternsNames() const;

	static Pixel characteristicColor();

	Pixel getTransparentPixelsColor() const;

private:
	void save() const;
	void load();
	void saveSelectedImagePattern() const;
	void updateSelectedImagePatternBasedOnSave();
	void saveRectangularRegionsNamesListSelection_captureRegion() const;
	void loadRectangularRegionsNamesListSelection_captureRegion();
	void saveRectangularRegionsNamesListSelection_testRegion() const;
	void loadRectangularRegionsNamesListSelection_testRegion();

	bool shouldShowCaptureRegion() const;
	void showCaptureRegion(Image* frame, Image* frameOverlays);
	bool shouldShowPatternPreview() const;
	void showPatternPreview(Image* frame, Image* frameOverlays);
	bool shouldCapturePattern() const;
	void capturePattern(Image* frame);
	bool shouldShowTestRegionPixelApproximations() const;
	void showTestRegionPixelApproximations(Image* frame, Image* frameOverlays);
	bool shouldShowTestPatternOverlays() const;
	void showTestPatternOverlays(Image* frame, Image* frameOverlays);
	void setColorBeingPickedFromFrame(const Pixel& colorBeingPickedFromFrame);
	void showColorBeingPickedFromFrame(Image* frameOverlays);

	bool shouldTestPattern() const;
	void testPattern(Image* frame, Image* frameOverlays);

	void updateImagePatternsNames();

	void addColorToPattern();

	void setSelectedImagePattern(ImagePattern* imagePattern);

	void showPixelApproximations(Image* frame, Image* frameOverlays, const Rectangle2D<int>& region);
	bool shouldUpdateTestRegionPixelAproximationsImage() const;
	bool shouldUpdateGetImagePatternRegionPixelAproximationsImage() const;
	void updateTestRegionPixelApproximationsImage(Image* frame);
	void updateGetImagePatternRegionPixelApproximationsImage(Image* frame);
	void approximateImage(Image* image, ImagePattern* imagePattern);

	std::map<std::string, ImagePattern*> mapFromNameToImagePattern;

	TravailleurContext* travailleurContext = nullptr;

	std::unique_ptr<ImGuiSingleSelectionList> imagePatternsNames;

	Pixel transparentPixelsColor;
	Pixel colorToAddToPattern;

	std::unique_ptr<ImGuiCheckbox> allowFrameOverlays;

	std::unique_ptr<ImGuiCheckbox> showPickColorFromFrameOptions;
	std::unique_ptr<ImGuiPointInput> colorToPickPositionInFrame;
	Pixel colorBeingPickedFromFrame;

	std::unique_ptr<ImGuiCheckbox> showPatternCaptureOptions;
	std::unique_ptr<ImGuiCheckbox> showPatternPreviewCheckbox;
	std::unique_ptr<ImGuiCheckbox> showTransparentPixelsWithTransparentPixelsColor;
	Rectangle2D<int> captureRegion;
	bool wantsToCapturePattern = false;

	std::unique_ptr<ImGuiCheckbox> showTestPatternOptions;
	Rectangle2D<int> testRegion;
	std::vector<Point2D<int>> patternTestResults;
	bool wantsToTestPattern = false;
	std::unique_ptr<ImGuiCheckbox> testAcrossWholeTestRegion;
	std::unique_ptr<ImGuiCheckbox> showTestRegionPixelApproximationsCheckbox;

	Image* testRegionPixelApproximationsImage = nullptr;
	std::unique_ptr<ImGuiImageRenderer> testRegionPixelApproximationsImageRenderer;
	bool wantsToUpdateTestRegionPixelAproximationsImage = false;
	std::unique_ptr<ImGuiCheckbox> performOnlyPreprocessingForTestRegionPixelApproximations;
	std::string timeTakenByTestInMs;

	Image* getImagePatternRegionPixelApproximationsImage = nullptr;
	std::unique_ptr<ImGuiImageRenderer> getImagePatternRegionPixelApproximationsImageRenderer;
	bool wantsToUpdateGetImagePatternRegionPixelAproximationsImage = false;
	std::unique_ptr<ImGuiCheckbox> performOnlyPreprocessingForGetImagePatternRegionPixelApproximations;

	std::unique_ptr<ImGuiCheckbox> useRectangularRegionAsCaptureRegion;
	std::unique_ptr<ImGuiSingleSelectionList> rectangularRegionsNamesList_captureRegion;
	Rectangle2D<int> determineCaptureRegion(Image* frame) const;

	std::unique_ptr<ImGuiCheckbox> useRectangularRegionAsTestRegion;
	std::unique_ptr<ImGuiSingleSelectionList> rectangularRegionsNamesList_testRegion;
	Rectangle2D<int> determineTestRegion(Image* frame) const;
};

#endif