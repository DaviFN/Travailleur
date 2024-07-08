#ifndef ImageOCR_H
#define ImageOCR_H

#include <memory>
#include <string>
#include <vector>

#include "Common/Point2D.h"
#include "Common/Rectangle2D.h"

#include "ImGuiWrappers/ImGuiCheckbox.h"
#include "ImGuiWrappers/ImGuiCombo.h"
#include "ImGuiWrappers/ImGuiIntSlider.h"
#include "ImGuiWrappers/ImGuiSingleSelectionList.h"

class Image;
class ImagePreprocessing;
class RectangularRegion;
class TravailleurContext;

class ImageOCR
{
public:

	enum class PSMType {
		PsmOsdOnly,            //   Orientation and script detection only.
		PsmAutoOsd,            //   Automatic page segmentation with orientation and script detection. (OSD)
		PsmAutoOnly,           // 	Automatic page segmentation, but no OSD, or OCR.
		PsmAuto,               //   Fully automatic page segmentation, but no OSD.
		PsmSingleColumn,	   //	Assume a single column of text of variable sizes.
		PsmSingleBlockVertText,//	Assume a single uniform block of vertically aligned text.
		PsmSingleBlock,	       //	Assume a single uniform block of text. (Default.)
		PsmSingleLine,	       //	Treat the image as a single text line.
		PsmSingleWord,	       //	Treat the image as a single word.
		PsmCircleWord,	       //	Treat the image as a single word in a circle.
		PsmSingleChar,	       //	Treat the image as a single character.
		PsmSparseText,	       //	Find as much text as possible in no particular order.
		PsmSparseTextOcd,	   //	Sparse text with orientation and script det.
		PsmRawLine             //	Treat the image as a single text line, bypassing hacks that are Tesseract - specific.
	};

	ImageOCR(const std::string& name, TravailleurContext* travailleurContext);
	~ImageOCR();

	const std::string& getName() const;
	void setName(const std::string& name);

	void showGui();

	RectangularRegion* getRectangularRegion() const;

	bool usesImagePreprocessing() const;

	ImagePreprocessing* getImagePreprocessing() const;

	void informRectangularRegionsNames(const std::vector<std::string>& rectangularRegionsNames);

	void informImagePreprocessingsNames(const std::vector<std::string>& imagePreprocessingsNames);

	std::string performOCROnRegion(Image* frame, const Rectangle2D<int>& region);

private:
	void load();
	void save() const;

	std::string getSaveDirectory() const;
	void assureSaveDirectoryExists();

	void initializePsmTypeCombo();

	std::string name;

	std::unique_ptr<ImGuiSingleSelectionList> rectangularRegionsNamesList;
	std::unique_ptr<ImGuiCheckbox> usesImagePreprocessingCheckbox;
	std::unique_ptr<ImGuiSingleSelectionList> imagePreprocessingsNamesList;

	std::unique_ptr<ImGuiCombo> psmTypeCombo;

	TravailleurContext* travailleurContext = nullptr;
};

#endif