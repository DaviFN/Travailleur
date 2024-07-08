#include "RectangularRegion.h"

#include "imgui.h"

#include <vector>

#include "Common/FilesystemUtils.h"
#include "Common/TravailleurUtils.h"
#include "Common/SimplePersistenceUtils.h"
#include "ImGuiWrappers/ImGuiTextOutput.h"
#include "TravailleurContext.h"

RectangularRegion::RectangularRegion(const std::string& name, TravailleurContext* travailleurContext) :
    name(name),
    travailleurContext(travailleurContext)
{
    assureSaveDirectoryExists();

	// default rectangular region
	topLeft = Point2D<int>(0, 0);
	size = Point2D<int>(100, 100);
    previousTopLeft = topLeft;
    previousSize = size;

	load();

    topLeftXIntSlider = std::make_unique<ImGuiIntSlider>("topLeft.x", topLeft.x, -2000, 2000);
    topLeftYIntSlider = std::make_unique<ImGuiIntSlider>("topLeft.y", topLeft.y, -2000, 2000);
    sizeXIntSlider = std::make_unique<ImGuiIntSlider>("size.x", size.x, 1, 2000);
    sizeYIntSlider = std::make_unique<ImGuiIntSlider>("size.y", size.y, 1, 2000);

    relativeToParent = std::make_unique<ImGuiCheckbox>("Relative to parent", false, getSaveDirectory() + "\\relative-to-parent");

    parentSelector = std::make_unique<ImGuiSingleSelectionList>("parent-selector");

    relativeToImagePatternDetectionInParent = std::make_unique<ImGuiCheckbox>("Relative to image pattern detection in parent", false, getSaveDirectory() + "\\relative-to-image-pattern-detection-in-parent");

    imagePatternSelector = std::make_unique<ImGuiSingleSelectionList>("image-pattern-selector");
}

RectangularRegion::~RectangularRegion()
{
	save();
}

void RectangularRegion::load()
{
    std::vector<int> dataAsVector;
    auto result = SimplePersistenceUtils::loadVectorOfInt32(
        FilesystemUtils::filenameFromFilepath(getRectangleBinarySaveFilepath()),
        dataAsVector,
        FilesystemUtils::parentDirectoryOfFilepath(getRectangleBinarySaveFilepath())
    );
    if (result == LoadOperationResult::LoadedSuccessfully) {
        if (dataAsVector.size() == 4) {
            topLeft.x = dataAsVector[0];
            topLeft.y = dataAsVector[1];
            size.x = dataAsVector[2];
            size.y = dataAsVector[3];
        }
    }
    previousTopLeft = topLeft;
    previousSize = size;
}

void RectangularRegion::save() const
{
    std::vector<int> dataAsVector;
    dataAsVector.push_back(topLeft.x);
    dataAsVector.push_back(topLeft.y);
    dataAsVector.push_back(size.x);
    dataAsVector.push_back(size.y);
    SimplePersistenceUtils::saveVectorOfInt32(
        FilesystemUtils::filenameFromFilepath(getRectangleBinarySaveFilepath()),
        dataAsVector,
        FilesystemUtils::parentDirectoryOfFilepath(getRectangleBinarySaveFilepath()));
    SimplePersistenceUtils::saveString(
        "name-of-parent-rectangular-region",
        nameOfParent(),
        getSaveDirectory());
    SimplePersistenceUtils::saveString(
        "name-of-image-pattern-to-be-relative-to",
        nameOfImagePatternToBeRelativeTo(),
        getSaveDirectory());
}

std::string RectangularRegion::getSaveDirectory() const
{
    return TravailleurUtils::getRectangularRegionsPath(travailleurContext->getName()) + "\\" + name;
}

void RectangularRegion::assureSaveDirectoryExists()
{
    FilesystemUtils::assureDirectoryExists(getSaveDirectory());
}

std::string RectangularRegion::getRectangleBinarySaveFilepath() const
{
    return getSaveDirectory() + "\\rectangle-binary-save";
}

const std::string& RectangularRegion::getName() const
{
	return name;
}

void RectangularRegion::setName(const std::string& name)
{
	this->name = name;
}

const Point2D<int>& RectangularRegion::getTopLeft() const
{
	return topLeft;
}

void RectangularRegion::setTopLeft(const Point2D<int>& topLeft)
{
	this->topLeft = topLeft;
}

const Point2D<int>& RectangularRegion::getSize() const
{
	return size;
}

void RectangularRegion::setSize(const Point2D<int>& size)
{
	this->size = size;
}

Rectangle2D<int> RectangularRegion::getAsRectangle2D() const
{
    return Rectangle2D<int>(topLeft, size);
}

bool RectangularRegion::changedButNotMarkedAsDealtWith() const
{
    return changed && !changeDealtWith;
}

void RectangularRegion::markAsDealtWithChange()
{
    changed = false;
    changeDealtWith = true;
}

void RectangularRegion::showGui()
{
    ImGuiTextOutput(name);
    topLeftXIntSlider->showGui();
    topLeftYIntSlider->showGui();
    sizeXIntSlider->showGui();
    sizeYIntSlider->showGui();
    if (topLeftXIntSlider->getValue() != previousTopLeft.x ||
        topLeftYIntSlider->getValue() != previousTopLeft.y ||
        sizeXIntSlider->getValue() != previousSize.x ||
        sizeYIntSlider->getValue() != previousSize.y) {
        previousTopLeft.x = topLeft.x;
        previousTopLeft.y = topLeft.y;
        previousSize.x = size.x;
        previousSize.y = size.y;
        topLeft.x = topLeftXIntSlider->getValue();
        topLeft.y = topLeftYIntSlider->getValue();
        size.x = sizeXIntSlider->getValue();
        size.y = sizeYIntSlider->getValue();
    }

    relativeToParent->showGui();

    if (travailleurContext->getRectangularRegionsHandler()->parentsOfRectangularRegionFormCycle(this)) {
        ImGuiTextOutput("warning: parents form cycle; invalid rectangular region");
    }

    if(relativeToParent->getValue()) {
        parentSelector->showGui();
        relativeToImagePatternDetectionInParent->showGui();
        if (relativeToImagePatternDetectionInParent->getValue()) {
            imagePatternSelector->showGui();
        }
    }
}

void RectangularRegion::informNamesOfOtherRectangularRegions(const std::vector<std::string>& namesOfOtherRectangularRegions)
{
    parentSelector->setOptions(namesOfOtherRectangularRegions);
    std::string savedNameOfParentRectangularRegion;
    SimplePersistenceUtils::loadString(
        "name-of-parent-rectangular-region",
        savedNameOfParentRectangularRegion,
        getSaveDirectory());
    parentSelector->setSelectedOption(savedNameOfParentRectangularRegion);
}

bool RectangularRegion::isRelativeToParent() const
{
    return relativeToParent->getValue();
}

std::string RectangularRegion::nameOfParent() const
{
    return isRelativeToParent() ? parentSelector->getSelectedOption() : "";
}

void RectangularRegion::informImagePatternNames(
    const std::vector<std::string>& imagePatternNames
)
{
    imagePatternSelector->setOptions(imagePatternNames);
    std::string savedNameOfImagePatternToBeRelativeTo;
    SimplePersistenceUtils::loadString(
        "name-of-image-pattern-to-be-relative-to",
        savedNameOfImagePatternToBeRelativeTo,
        getSaveDirectory());
    imagePatternSelector->setSelectedOption(savedNameOfImagePatternToBeRelativeTo);
}

bool RectangularRegion::isRelativeToImagePatternDetection() const
{
    return relativeToImagePatternDetectionInParent->getValue();
}

std::string RectangularRegion::nameOfImagePatternToBeRelativeTo() const
{
    return isRelativeToImagePatternDetection() ? imagePatternSelector->getSelectedOption() : "";
}