#include "ImGuiPointInput.h"

#include "imgui.h"

#include "Common/FilesystemUtils.h"
#include "Common/SimplePersistenceUtils.h"

ImGuiPointInput::ImGuiPointInput(const std::string& label, const int rangeMin, const double rangeMax, const double dragSpeed, const std::string& persistenceFilepath, const Point2D<int>& defaultValue):
    label(label),
    persistenceFilepath(persistenceFilepath),
    value(defaultValue),
    rangeMin(rangeMin),
    rangeMax(rangeMax),
    dragSpeed(dragSpeed)
{
    load();
}


ImGuiPointInput::~ImGuiPointInput()
{
    save();
}

void ImGuiPointInput::load()
{
    std::vector<int> components;
    auto result = SimplePersistenceUtils::loadVectorOfInt32(
        FilesystemUtils::filenameFromFilepath(persistenceFilepath),
        components,
        FilesystemUtils::parentDirectoryOfFilepath(persistenceFilepath)
    );
    if (result == LoadOperationResult::LoadedSuccessfully && components.size() == 2) {
        value.x = components[0];
        value.y = components[1];
    }
    previousValue = value;
}

void ImGuiPointInput::save() const
{
    std::vector<int> components;
    components.push_back(value.x);
    components.push_back(value.y);
    SimplePersistenceUtils::saveVectorOfInt32(
        FilesystemUtils::filenameFromFilepath(persistenceFilepath),
        components,
        FilesystemUtils::parentDirectoryOfFilepath(persistenceFilepath)
    );
}

const std::string& ImGuiPointInput::getLabel() const
{
    return label;
}

void ImGuiPointInput::setLabel(const std::string& label)
{
    this->label = label;
}

Point2D<int> ImGuiPointInput::getValue() const
{
    return value;
}

void ImGuiPointInput::setValue(const Point2D<int>& value)
{
    this->value = value;
}

bool ImGuiPointInput::changedButNotMarkedAsDealtWith() const
{
    return changed && !changeDealtWith;
}

void ImGuiPointInput::markAsDealtWithChange()
{
    changed = false;
    changeDealtWith = true;
}

void ImGuiPointInput::showGui()
{
    ImGui::DragScalar((label + ".x").c_str(), ImGuiDataType_S32, &value.x, dragSpeed, &rangeMin, &rangeMax, 0, 1.0f);
    ImGui::DragScalar((label + ".y").c_str(), ImGuiDataType_S32, &value.y, dragSpeed, &rangeMin, &rangeMax, 0, 1.0f);
    if (value != previousValue) {
        save();
        previousValue = value;
        changed = true;
    }
    
}