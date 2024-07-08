#include "ImGuiImageEditor.h"

#include "imgui.h"

#include "ImGuiWrappers/ImGuiColorOutput.h"
#include "ImGuiWrappers/ImGuiTextOutput.h"

ImGuiImageEditor::ImGuiImageEditor(const std::string& label) :
	label(label),
	imageBeingEdited(std::make_unique<Image>()),
	imageCustomDisplay(std::make_unique<Image>()),
	imageButtonHandler(std::make_unique<ImGuiImageButtonHandler>()),
	colorToInsertSelector(std::make_unique<ImGuiCombo>(label + "'s colorToInsert selector")),
	penTypeSelector(std::make_unique<ImGuiCombo>(label + "'s penType")),
	penThickness(std::make_unique<ImGuiIntSlider>(label + "'s penThickness", 0, 0, 1620))
{
	penTypeSelector->addOption("diamond");
	penTypeSelector->addOption("cross");
	penTypeSelector->addOption("square-with-center-in-pen");
	penTypeSelector->addOption("square-with-top-left-in-pen");
}

const std::string& ImGuiImageEditor::getLabel() const
{
	return label;
}

void ImGuiImageEditor::setLabel(const std::string& label)
{
	this->label = label;
}

const Image& ImGuiImageEditor::getImage() const
{
	return *imageBeingEdited;
}

void ImGuiImageEditor::setImage(const Image& image)
{
	imageBeingEdited = std::make_unique<Image>(image);
	updateImageCustomDisplay();
}

void ImGuiImageEditor::clearImage()
{
	imageBeingEdited = std::make_unique<Image>();
}

void ImGuiImageEditor::drawImageCustomDisplayGrid()
{
	const size_t imageWidth = imageBeingEdited->getWidth();
	const size_t imageHeight = imageBeingEdited->getHeight();

	const int nHorizontalGridLines = imageHeight - 1;
	const int nVerticalGridLines = imageWidth - 1;
	for (int i = 0; i < nHorizontalGridLines; ++i) {
		const Point2D<int> rectangleTopLeft(0, (i + 1) * getCellSizeY() + i * getGridThicknessY());
		const Point2D<int> rectangleSize(imageWidth * getCellSizeX() + nVerticalGridLines * getGridThicknessX(), getGridThicknessY());
		imageCustomDisplay->drawRectangle(rectangleTopLeft, rectangleSize, getGridColor());
	}
	for (int j = 0; j < nVerticalGridLines; ++j) {
		const Point2D<int> rectangleTopLeft((j + 1) * getCellSizeX() + j * getGridThicknessX(), 0);
		const Point2D<int> rectangleSize(getGridThicknessX(), imageHeight * getCellSizeY() + nHorizontalGridLines * getGridThicknessY());
		imageCustomDisplay->drawRectangle(rectangleTopLeft, rectangleSize, getGridColor());
	}
}

void ImGuiImageEditor::drawImageCustomDisplayInvisiblePixel(const int customDisplayX, const int customDisplayY)
{
	const size_t imageWidth = imageBeingEdited->getWidth();
	const size_t imageHeight = imageBeingEdited->getHeight();

	const Point2D<int> cellTopLeft(customDisplayX, customDisplayY);
	const size_t invisibleCellSquareSize = getInvisibleCellSquareSize();
	const Point2D<int> squareSize(invisibleCellSquareSize, invisibleCellSquareSize);

	if (transparentPixelColor == nullptr) {
		const size_t nSquaresX = (getCellSizeX() / invisibleCellSquareSize);
		const size_t nSquaresY = (getCellSizeY() / invisibleCellSquareSize);
		for (size_t i = 0; i < nSquaresX; ++i) {
			for (size_t j = 0; j < nSquaresY; ++j) {
				const Pixel squareColor = (i + j) % 2 == 0 ? getInvisibleCellSquareColor1() : getInvisibleCellSquareColor2();
				const Point2D<int> topLeftOffset = Point2D<int>(i * squareSize.x, j * squareSize.y);
				imageCustomDisplay->drawRectangle(cellTopLeft + topLeftOffset, squareSize, squareColor);
			}
		}
	}
	else {
		const Point2D<int> cellSize(getCellSizeX(), getCellSizeY());
		imageCustomDisplay->drawRectangle(cellTopLeft, cellSize, *transparentPixelColor);
	}
}

void ImGuiImageEditor::drawImageCustomDisplayPixel(const int x, const int y, const Pixel& pixel)
{
	const size_t imageWidth = imageBeingEdited->getWidth();
	const size_t imageHeight = imageBeingEdited->getHeight();

	const Point2D<int> rectangleTopLeft(x * (getCellSizeX() + getGridThicknessX()), y * (getCellSizeY() + getGridThicknessY()));
	const Point2D<int> rectangleSize(getCellSizeX(), getCellSizeY());
	if (pixel.a == 255) {
		imageCustomDisplay->drawRectangle(rectangleTopLeft, rectangleSize, pixel);
	}
	else {
		drawImageCustomDisplayInvisiblePixel(rectangleTopLeft.x, rectangleTopLeft.y);
	}
}

void ImGuiImageEditor::updateImageCustomDisplay()
{
	if (imageBeingEdited == nullptr) {
		imageCustomDisplay = nullptr;
		return;
	}

	const size_t imageWidth = imageBeingEdited->getWidth();
	const size_t imageHeight = imageBeingEdited->getHeight();

	const int nHorizontalGridLines = imageHeight - 1;
	const int nVerticalGridLines = imageWidth - 1;
	const int customDisplayWidth = getCellSizeX() * imageWidth + nVerticalGridLines * getGridThicknessX();
	const int customDisplayHeight = getCellSizeY() * imageHeight + nHorizontalGridLines * getGridThicknessY();

	imageCustomDisplay = std::make_unique<Image>(customDisplayWidth, customDisplayHeight);

	drawImageCustomDisplayGrid();

	for (size_t i = 0; i < imageBeingEdited->getWidth(); ++i) {
		for (size_t j = 0; j < imageBeingEdited->getHeight(); ++j) {
			drawImageCustomDisplayPixel(i, j, imageBeingEdited->pixelAt(i, j));
		}
	}
}

size_t ImGuiImageEditor::getCellSizeX() const
{
	return 10;
}

size_t ImGuiImageEditor::getCellSizeY() const
{
	return 10;
}

size_t ImGuiImageEditor::getGridThicknessX() const
{
	return 1;
}

size_t ImGuiImageEditor::getGridThicknessY() const
{
	return 1;
}

size_t ImGuiImageEditor::getInvisibleCellSquareSize() const
{
	return 2;
}

Pixel ImGuiImageEditor::getInvisibleCellSquareColor1() const
{
	return Pixel(60, 60, 60);
}

Pixel ImGuiImageEditor::getInvisibleCellSquareColor2() const
{
	return Pixel(200, 200, 200);
}

Pixel ImGuiImageEditor::getGridColor() const
{
	return Pixel(255, 0, 0, 255);
}

Pixel ImGuiImageEditor::getColorToInsert() const
{
	const int selectedIndex = colorToInsertSelector->getSelectedIndex();
	if (selectedIndex >= 0) {
		auto it = mapStrToColorsToInsert.find(colorToInsertSelector->getSelectedOption());
		return it->second;
	}
	return Pixel::transparentPixel();
}

void ImGuiImageEditor::setPixelAndUpdate(const int x, const int y, const Pixel& pixel)
{
	if (imageBeingEdited->isValidPixelLocation(x, y)) {
		imageBeingEdited->setPixel(x, y, pixel);
		drawImageCustomDisplayPixel(x, y, pixel);
	}
}

void ImGuiImageEditor::drawDiamond(const int x, const int y)
{
	const Pixel colorToInsert = getColorToInsert();

	const int penThickness = static_cast<int>(getPenThickness());
	const size_t startingX = x - penThickness;
	const size_t startingY = y - penThickness;
	const size_t diamondSize = 1 + 2 * penThickness;

	for (int i = 0; i < diamondSize; ++i) {
		for (int j = 0; j < diamondSize; ++j) {
			if (abs(penThickness - i) + abs(penThickness - j) <= penThickness) {
				setPixelAndUpdate(startingX + i, startingY + j, colorToInsert);
			}
		}
	}
}

void ImGuiImageEditor::drawCross(const int x, const int y)
{
	const Pixel colorToInsert = getColorToInsert();
	setPixelAndUpdate(x, y, colorToInsert);
	
	for (size_t i = 1; i <= getPenThickness(); ++i) {
		setPixelAndUpdate(x + i, y, colorToInsert);
		setPixelAndUpdate(x - i, y, colorToInsert);
		setPixelAndUpdate(x, y + i, colorToInsert);
		setPixelAndUpdate(x, y - i, colorToInsert);
	}
}

void ImGuiImageEditor::drawSquareWithCenterIn(const int x, const int y)
{
	const Pixel colorToInsert = getColorToInsert();

	const size_t penThickness = getPenThickness();
	const size_t startingX = x - penThickness;
	const size_t startingY = y - penThickness;
	const size_t squareSize = 1 + 2 * penThickness;
	for (size_t i = startingX; i < startingX + squareSize; ++i) {
		for (size_t j = startingY; j < startingY + squareSize; ++j) {
			setPixelAndUpdate(i, j, colorToInsert);
		}
	}
}

void ImGuiImageEditor::drawSquareWithTopLeftIn(const int x, const int y)
{
	const Pixel colorToInsert = getColorToInsert();

	const size_t penThickness = getPenThickness();
	for (size_t i = x; i < x + penThickness; ++i) {
		for (size_t j = y; j < y + penThickness; ++j) {
			setPixelAndUpdate(i, j, colorToInsert);
		}
	}
}

void ImGuiImageEditor::onMousePressedOnImageBeingEditedPixel(const int x, const int y)
{
	const PenType penType = getPenType();
	if (penType == PenType::Diamond) {
		drawDiamond(x, y);
	}
	if (penType == PenType::Cross) {
		drawCross(x, y);
	}
	else if(penType == PenType::SquareWithCenterInPen) {
		drawSquareWithCenterIn(x, y);
	}
	else if (penType == PenType::SquareWithTopLeftInPen) {
		drawSquareWithTopLeftIn(x, y);
	}
}

void ImGuiImageEditor::onMousePressedOnCustomDisplayPixel(const Point2D<int> positionPressed)
{
	onMousePressedOnImageBeingEditedPixel(positionPressed.x, positionPressed.y);
}

Point2D<int> ImGuiImageEditor::translateFromCustomDisplayPixelPositionToImagePixelPosition(const Point2D<int>& customDisplayPixelPosition) const
{
	const size_t imageWidth = imageBeingEdited->getWidth();
	const size_t imageHeight = imageBeingEdited->getHeight();

	const int nHorizontalGridLines = imageHeight - 1;
	const int nVerticalGridLines = imageWidth - 1;
	const int customDisplayWidth = getCellSizeX() * imageWidth + nVerticalGridLines * getGridThicknessX();
	const int customDisplayHeight = getCellSizeY() * imageHeight + nHorizontalGridLines * getGridThicknessY();

	const int& customDisplayPixelPositionX = customDisplayPixelPosition.x;
	const int& customDisplayPixelPositionY = customDisplayPixelPosition.y;
	if (customDisplayPixelPositionX < 0 || customDisplayPixelPositionX >= customDisplayWidth) {
		return Point2D<int>(-1, -1);
	}
	if (customDisplayPixelPositionY < 0 || customDisplayPixelPositionY >= customDisplayHeight) {
		return Point2D<int>(-1, -1);
	}

	const int& restOfDivisionX = customDisplayPixelPositionX % (getCellSizeX() + getGridThicknessY());
	const int& restOfDivisionY = customDisplayPixelPositionY % (getCellSizeY() + getGridThicknessX());
	if (restOfDivisionX >= getCellSizeX()) {
		return Point2D<int>(-1, -1);
	}
	if (restOfDivisionY >= getCellSizeY()) {
		return Point2D<int>(-1, -1);
	}

	const int x = customDisplayPixelPositionX / (getCellSizeX() + getGridThicknessY());
	const int y = customDisplayPixelPositionY / (getCellSizeY() + getGridThicknessX());

	return Point2D<int>(x, y);
}

void ImGuiImageEditor::clearColorsToInsert()
{
	colorToInsertSelector->clear();
	mapStrToColorsToInsert.clear();
}

void ImGuiImageEditor::addColorToInsert(const std::string& label, const Pixel& colorToInsert)
{
	colorToInsertSelector->addOption(label);
	mapStrToColorsToInsert[label] = colorToInsert;
}

void ImGuiImageEditor::setTransparentPixelColor(const Pixel& color)
{
	transparentPixelColor = std::make_unique<Pixel>(color);
}

void ImGuiImageEditor::clearTransparentPixelColor()
{
	transparentPixelColor = nullptr;
}

ImGuiImageEditor::PenType ImGuiImageEditor::getPenType() const
{
	if (penTypeSelector->getSelectedOption() == "diamond") {
		return PenType::Diamond;
	}
	if (penTypeSelector->getSelectedOption() == "cross") {
		return PenType::Cross;
	}
	if (penTypeSelector->getSelectedOption() == "square-with-center-in-pen") {
		return PenType::SquareWithCenterInPen;
	}
	if (penTypeSelector->getSelectedOption() == "square-with-top-left-in-pen") {
		return PenType::SquareWithTopLeftInPen;
	}
	return PenType::Diamond;
}

size_t ImGuiImageEditor::getPenThickness() const
{
	const int thickness = penThickness->getValue();
	return static_cast<size_t>(thickness);
}

void ImGuiImageEditor::showGui()
{
	ImGuiTextOutput(label);
	
	imageButtonHandler->renderImageButton(label.c_str(), *imageCustomDisplay);
	
	if (imageButtonHandler->getWasPressed()) {
		const Point2D<int> positionPressedInImageCustomDisplay = imageButtonHandler->getPositionPressed();
		const Point2D<int> positionPressed = translateFromCustomDisplayPixelPositionToImagePixelPosition(positionPressedInImageCustomDisplay);
		if (positionPressed != Point2D<int>(-1, -1)) {
			onMousePressedOnCustomDisplayPixel(positionPressed);
		}
	}

	colorToInsertSelector->showGui();
	ImGuiColorOutput(label + "'s colorToInsert", getColorToInsert());

	penTypeSelector->showGui();
	penThickness->showGui();
}