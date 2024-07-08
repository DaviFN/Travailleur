#include "ImageResourcesHandler.h"

#include "imgui.h"

#include "Common/TravailleurUtils.h"
#include "TravailleurContext.h"

ImageResourcesHandler::ImageResourcesHandler(TravailleurContext* travailleurContext):
	travailleurContext(travailleurContext),
	imGuiImageRenderer(new ImGuiImageRenderer)
{
	assureImageResourceDataDirectoryExists();
	assureImagesDirectoryExists();

	imageResourcesNames = std::make_unique<ImGuiSingleSelectionList>("image-resources-names");

	renderImageResource = std::make_unique<ImGuiCheckbox>("render image resource (test)", true, TravailleurUtils::getConfigFilenameFor("render-image-resource", travailleurContext->getName()));

	showReferenceRectangularRegion = std::make_unique<ImGuiCheckbox>("show reference rectangular region", true, TravailleurUtils::getConfigFilenameFor("show-reference-rectangular-region", travailleurContext->getName()));

	showImageHere = std::make_unique<ImGuiCheckbox>("show image here", true, TravailleurUtils::getConfigFilenameFor("show-image-here", travailleurContext->getName()));
}

ImageResourcesHandler::~ImageResourcesHandler()
{
	saveSelectedImageResource();

	for (auto& mapItem : mapFromNameToImageResource) {
		delete mapItem.second;
	}
}

void ImageResourcesHandler::saveSelectedImageResource() const
{
	ImageResource* selectedImageResource = getSelectedImageResource();
	if (selectedImageResource != nullptr) {
		const std::string selectedImageResourceName = selectedImageResource->getName();
		SimplePersistenceUtils::saveString("selected-image-resource-name", selectedImageResourceName, TravailleurUtils::getConfigPath(travailleurContext->getName()));
	}
}

void ImageResourcesHandler::updateSelectedImageResourceBasedOnSave()
{
	std::string selectedImageResourceName;
	SimplePersistenceUtils::loadString("selected-image-resource-name", selectedImageResourceName, TravailleurUtils::getConfigPath(travailleurContext->getName()));
	ImageResource* savedSelectedImageResource = getImageResource(selectedImageResourceName);
	if (savedSelectedImageResource != nullptr) {
		setSelectedImageResource(savedSelectedImageResource);
	}
}

std::string ImageResourcesHandler::getImageResourcesFolder() const
{
	return TravailleurUtils::getImageResourcesPath(travailleurContext->getName());
}

std::string ImageResourcesHandler::getImageResourcesDataFolder() const
{
	return getImageResourcesFolder() + "\\savedata";
}

std::string ImageResourcesHandler::getImagesFolder() const
{
	return getImageResourcesFolder() + "\\images";
}

void ImageResourcesHandler::assureImageResourceDataDirectoryExists()
{
	FilesystemUtils::assureDirectoryExists(getImageResourcesDataFolder());
}

void ImageResourcesHandler::assureImagesDirectoryExists()
{
	FilesystemUtils::assureDirectoryExists(getImagesFolder());
}

void ImageResourcesHandler::updateImageResourcesNames()
{
	std::vector<std::string> names;
	for (auto& mapItem : mapFromNameToImageResource) {
		names.push_back(mapItem.first);
	}
	imageResourcesNames->setOptions(names);
}

void ImageResourcesHandler::registerImageResource(const std::string& name)
{
	assert(getImageResource(name) == nullptr);
	assert(mapFromNameToImageResource.find(name) == mapFromNameToImageResource.end());

	auto imageResource = new ImageResource(name, travailleurContext);
	mapFromNameToImageResource[name] = imageResource;

	updateImageResourcesNames();
	updateSelectedImageResourceBasedOnSave();
}

ImageResource* ImageResourcesHandler::getImageResource(const std::string& name) const
{
	auto it = mapFromNameToImageResource.find(name);
	if (it == mapFromNameToImageResource.end()) {
		return nullptr;
	}
	return it->second;
}

ImageResource* ImageResourcesHandler::getSelectedImageResource() const
{
	if (!imageResourcesNames->hasSelectedOption()) {
		return nullptr;
	}
	return getImageResource(imageResourcesNames->getSelectedOption());
}

void ImageResourcesHandler::setSelectedImageResource(ImageResource* imageResource)
{
	if (imageResource == nullptr) return;
	const std::string imageResourceName = imageResource->getName();
	imageResourcesNames->setSelectedOption(imageResourceName);
}

void ImageResourcesHandler::showGui()
{
	ImGui::Begin("ImageResourcesHandler");

	imageResourcesNames->showGui();

	if (ImGui::Button("open images directory")) {
		FilesystemUtils::openDirectoryInExplorer(getImagesFolder());
	}

	ImageResource* selectedImageResource = getSelectedImageResource();
	if (selectedImageResource != nullptr)
	{
		renderImageResource->showGui();

		showReferenceRectangularRegion->showGui();

		selectedImageResource->showGui();

		showImageHere->showGui();
		if (showImageHere->getValue()) {
			Image* image = selectedImageResource->getImage();
			if (image != nullptr) {
				imGuiImageRenderer->renderImage(*image);
			}
			else {
				ImGuiTextOutput("no image");
			}
		}
	}
	else {
		ImGuiTextOutput("no selected image resource");
	}

	ImGui::End();
}

void ImageResourcesHandler::showFrameOverlays(Image* frame, Image* frameOverlays)
{
	ImageResource* selectedImageResource = getSelectedImageResource();
	if (selectedImageResource != nullptr) {
		if (showReferenceRectangularRegion->getValue()) {
			if (selectedImageResource->isRelativeToRectangularRegion()) {
				RectangularRegion* rectangularRegion = selectedImageResource->getRectangularRegionToBeRelativeTo();
				if (rectangularRegion != nullptr) {
					travailleurContext->getRectangularRegionsHandler()->showRectangularRegionOverlay(frame, frameOverlays, rectangularRegion, Pixel(255, 0, 0));
				}
			}
		}

		if (renderImageResource->getValue()) {
			selectedImageResource->renderImage(frame, frameOverlays);
		}
	}
}

void ImageResourcesHandler::afterAppSetup()
{
	const std::vector<std::string> rectangularRegionNames = travailleurContext->rectangularRegionsNames();
	for (auto& mapItem : mapFromNameToImageResource)
	{
		ImageResource* imageResource = mapItem.second;
		imageResource->informRectangularRegionsNames(rectangularRegionNames);
	}
}