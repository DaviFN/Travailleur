#ifndef ImageResourcesHandler_H
#define ImageResourcesHandler_H

#include <map>
#include <memory>
#include <string>

#include "Common/Image.h"
#include "ImageResource.h"
#include "ImGuiWrappers/ImGuiCheckbox.h"
#include "ImGuiWrappers/ImGuiImageRenderer.h"
#include "ImGuiWrappers/ImGuiSingleSelectionList.h"

class TravailleurContext;

class ImageResourcesHandler
{
public:
	ImageResourcesHandler(TravailleurContext* travailleurContext);
	~ImageResourcesHandler();

	void registerImageResource(const std::string& name);

	ImageResource* getImageResource(const std::string& name) const;

	void showGui();

	void showFrameOverlays(Image* frame, Image* frameOverlays);

	std::string getImageResourcesDataFolder() const;

	std::string getImagesFolder() const;

	void afterAppSetup();

private:
	void saveSelectedImageResource() const;
	void updateSelectedImageResourceBasedOnSave();

	std::string getImageResourcesFolder() const;

	void assureImageResourceDataDirectoryExists();
	void assureImagesDirectoryExists();

	void updateImageResourcesNames();

	void setSelectedImageResource(ImageResource* imageResource);

	ImageResource* getSelectedImageResource() const;

	std::map<std::string, ImageResource*> mapFromNameToImageResource;

	std::unique_ptr<ImGuiSingleSelectionList> imageResourcesNames;

	std::unique_ptr<ImGuiImageRenderer> imGuiImageRenderer;

	std::unique_ptr<ImGuiCheckbox> renderImageResource;

	std::unique_ptr<ImGuiCheckbox> showReferenceRectangularRegion;

	std::unique_ptr<ImGuiCheckbox> showImageHere;

	TravailleurContext* travailleurContext = nullptr;
};

#endif