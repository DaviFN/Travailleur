#ifndef RectangularRegion_H
#define RectangularRegion_H

#include <memory>
#include <string>
#include <vector>

#include "Common/Point2D.h"

#include "ImGuiWrappers/ImGuiCheckbox.h"
#include "ImGuiWrappers/ImGuiIntSlider.h"
#include "ImGuiWrappers/ImGuiSingleSelectionList.h"
#include "Common/Rectangle2D.h"

class TravailleurContext;

class RectangularRegion
{
public:
	RectangularRegion(const std::string& name, TravailleurContext* travailleurContext);
	~RectangularRegion();

	const std::string& getName() const;
	void setName(const std::string& name);

	const Point2D<int>& getTopLeft() const;
	void setTopLeft(const Point2D<int>& topLeft);

	const Point2D<int>& getSize() const;
	void setSize(const Point2D<int>& size);

	Rectangle2D<int> getAsRectangle2D() const;

	bool changedButNotMarkedAsDealtWith() const;

	void markAsDealtWithChange();

	void showGui();

	void informNamesOfOtherRectangularRegions(const std::vector<std::string>& namesOfOtherRectangularRegions);
	
	bool isRelativeToParent() const;

	std::string nameOfParent() const;

	void informImagePatternNames(
		const std::vector<std::string>& imagePatternNames
	);

	bool isRelativeToImagePatternDetection() const;

	std::string nameOfImagePatternToBeRelativeTo() const;

private:
	void load();
	void save() const;

	std::string getSaveDirectory() const;
	void assureSaveDirectoryExists();

	std::string getRectangleBinarySaveFilepath() const;

	std::string name;
	Point2D<int> topLeft;
	Point2D<int> size;

	Point2D<int> previousTopLeft;
	Point2D<int> previousSize;
	bool changed = false;
	bool changeDealtWith = false;

	std::unique_ptr<ImGuiIntSlider> topLeftXIntSlider;
	std::unique_ptr<ImGuiIntSlider> topLeftYIntSlider;
	std::unique_ptr<ImGuiIntSlider> sizeXIntSlider;
	std::unique_ptr<ImGuiIntSlider> sizeYIntSlider;

	std::unique_ptr<ImGuiCheckbox> relativeToParent;

	std::unique_ptr<ImGuiSingleSelectionList> parentSelector;

	std::unique_ptr<ImGuiCheckbox> relativeToImagePatternDetectionInParent;

	std::unique_ptr<ImGuiSingleSelectionList> imagePatternSelector;

	TravailleurContext* travailleurContext = nullptr;
};

#endif