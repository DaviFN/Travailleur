#include "ImageResource.h"

#include "imgui.h"

#include "Common/TravailleurUtils.h"
#include "ImGuiWrappers/ImGuiTextOutput.h"
#include "TravailleurContext.h"

ImageResource::ImageResource(const std::string& name, TravailleurContext* travailleurContext) :
    name(name),
    travailleurContext(travailleurContext),
	image(new Image)
{
	assureSaveDirectoryExists();

	relativePathTextInput = std::make_unique<ImGuiTextInput>("relative path", getSaveDirectory() + "\\relative-path");

	resizeImageCheckbox = std::make_unique<ImGuiCheckbox>("resize image", false, getSaveDirectory() + "\\scale-image");
	resizeWidthSlider = std::make_unique<ImGuiIntSlider>("width", 50, 1, 2000, 1.0, getSaveDirectory() + "\\resize-width");
	resizeHeightSlider = std::make_unique<ImGuiIntSlider>("height", 50, 1, 2000, 1.0, getSaveDirectory() + "\\resize-height");

	updateImage();
	updateResizedImage();

	renderPosition = std::make_unique<ImGuiPointInput>("render position", -2000, 2000, 1.0, getSaveDirectory() + "\\render-position");

	relativeToRectangularRegion = std::make_unique<ImGuiCheckbox>("relative to rectangular region", false, getSaveDirectory() + "\\relative-to-rectangular-region");

	rectangularRegionsNamesList = std::make_unique<ImGuiSingleSelectionList>("rectangular-regions-names-image-resource");

	load();
}

ImageResource::~ImageResource()
{
	save();
}

void ImageResource::load()
{
}

void ImageResource::save() const
{
	const std::string nameOfSelectedRectangularRegion = rectangularRegionsNamesList->getSelectedOption();
	SimplePersistenceUtils::saveString(
		"name-of-selected-rectangular-region",
		nameOfSelectedRectangularRegion,
		getSaveDirectory());
}

std::string ImageResource::getImageResourcesDataFolder() const
{
	return travailleurContext->getImageResourcesHandler()->getImageResourcesDataFolder();
}

std::string ImageResource::getImagesFolder() const
{
	return travailleurContext->getImageResourcesHandler()->getImagesFolder();
}

std::string ImageResource::getSaveDirectory() const
{
    return getImageResourcesDataFolder() + "\\" + name;
}

void ImageResource::assureSaveDirectoryExists()
{
    FilesystemUtils::assureDirectoryExists(getSaveDirectory());
}

const std::string& ImageResource::getName() const
{
	return name;
}

void ImageResource::setName(const std::string& name)
{
	this->name = name;
}

std::string ImageResource::getImagePath() const
{
	return getImagesFolder() + "\\" + relativePathTextInput->getValue();
}

void ImageResource::updateImage()
{
	image->loadFromFile(getImagePath());
}

void ImageResource::updateResizedImage()
{
	if (resizeImageCheckbox->getValue()) {
		if (image != nullptr) {
			resizedImage = std::make_unique<Image>(*image);
			resizedImage->resize(resizeWidthSlider->getValue(), resizeHeightSlider->getValue());
		}
		else {
			resizedImage = nullptr;
		}
	}
}

void ImageResource::performImageResizingIfNecessary()
{
	if (resizeWidthSlider->changedButNotMarkedAsDealtWith() || resizeHeightSlider->changedButNotMarkedAsDealtWith()) {

		if (image != nullptr) {
			updateResizedImage();
		}

		resizeWidthSlider->markAsDealtWithChange();
		resizeHeightSlider->markAsDealtWithChange();
	}
}

void ImageResource::showGui()
{
	ImGuiTextOutput(name);

	relativePathTextInput->showGui();
	if (relativePathTextInput->changedButNotMarkedAsDealtWith()) {
		updateImage();
		relativePathTextInput->markAsDealtWithChange();
	}

	resizeImageCheckbox->showGui();
	resizeWidthSlider->showGui();
	resizeHeightSlider->showGui();
	performImageResizingIfNecessary();

	renderPosition->showGui();
	relativeToRectangularRegion->showGui();
	if (isRelativeToRectangularRegion()) {
		rectangularRegionsNamesList->showGui();
	}
}

Image* ImageResource::getImage() const
{
	return resizeImageCheckbox->getValue() ? resizedImage.get() : image.get();
}

Point2D<int> ImageResource::getRenderPosition(Image* frame) const
{
	RectangularRegion* rectangularRegionToBeRelativeTo = getRectangularRegionToBeRelativeTo();
	if (rectangularRegionToBeRelativeTo == nullptr) {
		return renderPosition->getValue();
	}

	const Point2D<int> reference = travailleurContext->getRectangularRegionsHandler()->determineTopLeftConsideringParents(frame, rectangularRegionToBeRelativeTo);
	return renderPosition->getValue() + reference;
}

void ImageResource::informRectangularRegionsNames(const std::vector<std::string>& rectangularRegionsNames)
{
	rectangularRegionsNamesList->setOptions(rectangularRegionsNames);
	std::string nameOfSelectedRectangularRegion;
	SimplePersistenceUtils::loadString(
		"name-of-selected-rectangular-region",
		nameOfSelectedRectangularRegion,
		getSaveDirectory());
	rectangularRegionsNamesList->setSelectedOption(nameOfSelectedRectangularRegion);
}

bool ImageResource::isRelativeToRectangularRegion() const
{
	return relativeToRectangularRegion->getValue();
}

RectangularRegion* ImageResource::getRectangularRegionToBeRelativeTo() const
{
	if (!isRelativeToRectangularRegion()) return nullptr;

	if (!rectangularRegionsNamesList->hasSelectedOption()) return nullptr;
	return travailleurContext->getRectangularRegion(rectangularRegionsNamesList->getSelectedOption());
}

void ImageResource::renderImage(Image* frame, Image* frameOverlays)
{
	Image* image = getImage();
	if (image != nullptr) {
		frameOverlays->drawRawImage(getRenderPosition(frame), *image);
	}
}