#ifndef ImGuiImageEditor_H
#define ImGuiImageEditor_H

#include <map>
#include <memory>
#include <string>

#include "Common/Image.h"
#include "ImGuiWrappers/ImGuiImageButtonHandler.h"
#include "ImGuiWrappers/ImGuiIntSlider.h"
#include "ImGuiWrappers/ImGuiCombo.h"

class ImGuiImageEditor
{
public:
	ImGuiImageEditor(const std::string& label);

	const std::string& getLabel() const;
	void setLabel(const std::string& label);

	void setImage(const Image& image);
	const Image& getImage() const;

	void clearImage();

	void clearColorsToInsert();
	void addColorToInsert(const std::string& label, const Pixel& colorToInsert);

	void setTransparentPixelColor(const Pixel& color);
	void clearTransparentPixelColor();

	void showGui();
private:
	enum class PenType {
		Diamond,
		Cross,
		SquareWithCenterInPen,
		SquareWithTopLeftInPen
	};

	PenType getPenType() const;
	size_t getPenThickness() const;

	size_t getCellSizeX() const;
	size_t getCellSizeY() const;
	size_t getGridThicknessX() const;
	size_t getGridThicknessY() const;
	size_t getInvisibleCellSquareSize() const;
	Pixel getInvisibleCellSquareColor1() const;
	Pixel getInvisibleCellSquareColor2() const;
	Pixel getGridColor() const;
	Pixel getColorToInsert() const;

	void drawImageCustomDisplayGrid();
	void drawImageCustomDisplayInvisiblePixel(const int customDisplayX, const int customDisplayY);
	void drawImageCustomDisplayPixel(const int x, const int y, const Pixel& pixel);
	
	void updateImageCustomDisplay();
	void onMousePressedOnCustomDisplayPixel(const Point2D<int> positionPressed);
	void drawDiamond(const int x, const int y);
	void drawCross(const int x, const int y);
	void drawSquareWithCenterIn(const int x, const int y);
	void drawSquareWithTopLeftIn(const int x, const int y);
	void onMousePressedOnImageBeingEditedPixel(const int x, const int y);
	Point2D<int> translateFromCustomDisplayPixelPositionToImagePixelPosition(const Point2D<int>& customDisplayPixelPosition) const;

	void setPixelAndUpdate(const int x, const int y, const Pixel& pixel);

	std::string label;

	std::unique_ptr<Image> imageBeingEdited;
	std::unique_ptr<Image> imageCustomDisplay;
	std::unique_ptr<ImGuiImageButtonHandler> imageButtonHandler;

	std::unique_ptr<Pixel> transparentPixelColor;

	std::unique_ptr<ImGuiCombo> colorToInsertSelector;
	std::map<std::string, Pixel> mapStrToColorsToInsert;

	std::unique_ptr<ImGuiCombo> penTypeSelector;
	std::unique_ptr<ImGuiIntSlider> penThickness;
};

#endif