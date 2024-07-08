#ifndef ImagePreprocessingsHandler_H
#define ImagePreprocessingsHandler_H

#include <map>
#include <memory>

#include "Common/Image.h"
#include "Common/Rectangle2D.h"

#include "ImagePreprocessing.h"

#include "ImGuiWrappers/ImGuiCheckbox.h"
#include "ImGuiWrappers/ImGuiSingleSelectionList.h"
#include "ImGuiWrappers/ImGuiDoubleSlider.h"
#include "ImGuiWrappers/ImGuiImageRenderer.h"
#include "ImGuiWrappers/ImGuiPointInput.h"
#include "ImagePattern.h"

class TravailleurContext;

class ImagePreprocessingsHandler
{
public:
	ImagePreprocessingsHandler(TravailleurContext* travailleurContext);
	~ImagePreprocessingsHandler();

	void registerImagePreprocessing(const std::string& name);

	void afterAppSetup();

	ImagePreprocessing* getSelectedImagePreprocessing() const;

	ImagePreprocessing* getImagePreprocessing(const std::string& name) const;

	void showFrameOverlays(Image* frame, Image* frameOverlays);

	void performActionsThatDependOnFrame(Image* frame, Image* frameOverlays);

	void showGui();

	std::vector<std::string> getImagePreprocessingsNames() const;

private:
	void save() const;
	void load();
	void saveSelectedImagePreprocessing() const;
	void saveRectangularRegionsNamesListSelection_preprocessingTestRegion() const;
	void loadRectangularRegionsNamesListSelection_preprocessingTestRegion();
	void updateSelectedImagePreprocessingBasedOnSave();

	bool shouldShowPreprocessingTestRegion() const;
	void showPreprocessingTestRegion(Image* frame, Image* frameOverlays);

	bool shouldTestPreprocessing() const;
	void testPreprocessing(Image* frame);

	void updateImagePreprocessingsNames();

	void setSelectedImagePreprocessing(ImagePreprocessing* imagePattern);

	std::map<std::string, ImagePreprocessing*> mapFromNameToImagePreprocessing;

	TravailleurContext* travailleurContext = nullptr;

	std::unique_ptr<ImGuiCheckbox> allowFrameOverlays;

	std::unique_ptr<ImGuiSingleSelectionList> imagePreprocessingsNames;

	std::unique_ptr<ImGuiCheckbox> showPreprocessingTestRegionCheckbox;
	Rectangle2D<int> preprocessingTestRegion;

	std::unique_ptr<ImGuiCheckbox> useRectangularRegionAsPreprocessingTestRegion;
	std::unique_ptr<ImGuiSingleSelectionList> rectangularRegionsNamesList_preprocessingTestRegion;
	Rectangle2D<int> determinePreprocessingTestRegion(Image* frame) const;

	bool wantsToTestPreprocessing = false;

	std::unique_ptr<ImGuiImageRenderer> originalTestImageRenderer;
	std::unique_ptr<ImGuiImageRenderer> preprocessedTestImageRenderer;

	std::unique_ptr<ImGuiCheckbox> showOriginalTestImageCheckbox;
	std::unique_ptr<ImGuiCheckbox> showPreprocessedTestImageCheckbox;

	Image* originalTestImage = nullptr;
	Image* preprocessedTestImage = nullptr;

	std::string timeTakenByTestInMs;
};

#endif