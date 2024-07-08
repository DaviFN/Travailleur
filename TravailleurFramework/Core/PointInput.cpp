#include "PointInput.h"

#include "Common/TravailleurUtils.h"
#include "ImGuiWrappers/ImGuiTextOutput.h"
#include "TravailleurContext.h"

PointInput::PointInput(const std::string& name, TravailleurContext* travailleurContext):
	name(name),
	travailleurContext(travailleurContext)
{
    assureSaveDirectoryExists();

	load();

	xSlider = std::make_unique<ImGuiIntSlider>("x", getPoint().x, -2000, 2000);
	ySlider = std::make_unique<ImGuiIntSlider>("y", getPoint().y, -2000, 2000);
}

PointInput::~PointInput()
{
    save();
}

void PointInput::load()
{
    std::vector<int> dataAsVector;
    auto result = SimplePersistenceUtils::loadVectorOfInt32(
        FilesystemUtils::filenameFromFilepath(getPointBinarySaveFilepath()),
        dataAsVector,
        FilesystemUtils::parentDirectoryOfFilepath(getPointBinarySaveFilepath())
    );
    if (result == LoadOperationResult::LoadedSuccessfully) {
        if (dataAsVector.size() == 2) {
            point.x = dataAsVector[0];
            point.y = dataAsVector[1];
        }
    }
}

void PointInput::save() const
{
    std::vector<int> dataAsVector;
    dataAsVector.push_back(getPoint().x);
    dataAsVector.push_back(getPoint().y);
    SimplePersistenceUtils::saveVectorOfInt32(
        FilesystemUtils::filenameFromFilepath(getPointBinarySaveFilepath()),
        dataAsVector,
        FilesystemUtils::parentDirectoryOfFilepath(getPointBinarySaveFilepath()));
}

std::string PointInput::getSaveDirectory() const
{
    return TravailleurUtils::getPointInputsPath(travailleurContext->getName()) + "\\" + name;
}

void PointInput::assureSaveDirectoryExists()
{
    FilesystemUtils::assureDirectoryExists(getSaveDirectory());
}

std::string PointInput::getPointBinarySaveFilepath() const
{
    return getSaveDirectory() + "\\point-binary-save";
}

const std::string& PointInput::getName()const
{
	return name;
}

void PointInput::setName(const std::string& name)
{
	this->name = name;
}

const Point2D<int>& PointInput::getPoint() const
{
	return point;
}

void PointInput::setPoint(const Point2D<int>& point)
{
    this->point = point;
}

void PointInput::showGui()
{
	ImGuiTextOutput(name);
    xSlider->setValue(point.x);
    ySlider->setValue(point.y);
	xSlider->showGui();
	ySlider->showGui();
	point.x = xSlider->getValue();
	point.y = ySlider->getValue();
}