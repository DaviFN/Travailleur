#ifndef ImageOCRsHandler_H
#define ImageOCRsHandler_H

#include <map>
#include <memory>
#include <string>

#include "Common/Image.h"
#include "ImGuiWrappers/ImGuiCheckbox.h"
#include "ImGuiWrappers/ImGuiImageRenderer.h"
#include "ImGuiWrappers/ImGuiSingleSelectionList.h"
#include "RectangularRegion.h"

#include "ImageOCR.h"

class TravailleurContext;

class ImageOCRsHandler
{
public:
	ImageOCRsHandler(TravailleurContext* travailleurContext);
	~ImageOCRsHandler();

	void registerImageOCR(const std::string& name);

	ImageOCR* getSelectedImageOCR() const;

	ImageOCR* getImageOCR(const std::string& name) const;

	void showGui();

	void showFrameOverlays(Image* frame, Image* frameOverlays);

	void performActionsThatDependOnFrame(Image* frame, Image* frameOverlays);

	void afterAppSetup();

	std::vector<std::string> getImageOCRsNames() const;

	static Pixel characteristicColor();

private:
	void save() const;
	void load();
	void saveSelectedImageOCR() const;
	void updateSelectedImageOCRBasedOnSave();

	void updateImageOCRsNames();

	std::string getImageOCRsFolder() const;

	std::string filepathOfImageOCR(const std::string& name) const;

	void setSelectedImageOCR(ImageOCR* rectangularRegion);

	bool shouldShowTestRegion() const;

	void showTestRegion(Image* frame, Image* frameOverlays);

	Rectangle2D<int> determineTestRegion(Image* frame);

	void performTestOcr(Image* frame);

	void updateTestImagePreview(Image* frame);

	std::map<std::string, ImageOCR*> mapFromNameToImageOCR;

	TravailleurContext* travailleurContext = nullptr;

	std::unique_ptr<ImGuiCheckbox> allowFrameOverlays;

	std::unique_ptr<ImGuiSingleSelectionList> imageOCRsNames;

	std::unique_ptr<ImGuiCheckbox> showTestImageOCROptions;

	Rectangle2D<int> testRegion;

	std::unique_ptr<ImGuiCheckbox> useCustomTestRegion;

	std::string ocrTestResult;
	std::string timeTakenByOcrTestInMs;

	std::unique_ptr<ImGuiImageRenderer> testImagePreviewRenderer;

	std::unique_ptr<ImGuiCheckbox> skipImagePreprocessingOnTestPreview;

	Image* testImagePreview = nullptr;

	bool wantsToPerformTestOCR = false;

	bool wantsToUpdateTestImagePreview = false;
};

#endif