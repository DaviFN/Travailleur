#ifndef ImageResource_H
#define ImageResource_H

#include <memory>
#include <string>

#include "Common/Image.h"
#include "ImGuiWrappers/ImGuiCheckbox.h"
#include "ImGuiWrappers/ImGuiIntSlider.h"
#include "ImGuiWrappers/ImGuiPointInput.h"
#include "ImGuiWrappers/ImGuiSingleSelectionList.h"
#include "ImGuiWrappers/ImGuiTextInput.h"

class RectangularRegion;
class TravailleurContext;

class ImageResource
{
public:
	ImageResource(const std::string& name, TravailleurContext* travailleurContext);
	~ImageResource();

	const std::string& getName() const;
	void setName(const std::string& name);

	void showGui();

	Image* getImage() const;

	Point2D<int> getRenderPosition(Image* frame) const;

	void informRectangularRegionsNames(const std::vector<std::string>& rectangularRegionsNames);

	bool isRelativeToRectangularRegion() const;

	RectangularRegion* getRectangularRegionToBeRelativeTo() const;

	void renderImage(Image* frame, Image* frameOverlays);

private:
	void load();
	void save() const;

	std::string getImageResourcesDataFolder() const;
	std::string getImagesFolder() const;
	std::string getSaveDirectory() const;
	void assureSaveDirectoryExists();

	std::string getImagePath() const;
	void updateImage();
	void updateResizedImage();

	void performImageResizingIfNecessary();

	std::string name;
	std::unique_ptr<ImGuiTextInput> relativePathTextInput;
	std::unique_ptr<Image> image;
	std::unique_ptr<ImGuiCheckbox> resizeImageCheckbox;
	std::unique_ptr<ImGuiIntSlider> resizeWidthSlider;
	std::unique_ptr<ImGuiIntSlider> resizeHeightSlider;
	std::unique_ptr<Image> resizedImage;
	std::unique_ptr<ImGuiPointInput> renderPosition;
	std::unique_ptr<ImGuiCheckbox> relativeToRectangularRegion;
	std::unique_ptr<ImGuiSingleSelectionList> rectangularRegionsNamesList;

	TravailleurContext* travailleurContext = nullptr;
};

#endif