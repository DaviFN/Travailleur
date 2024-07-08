#include "ImGuiIntInput.h"

#include "imgui.h"

#include "Common/FilesystemUtils.h"
#include "Common/SimplePersistenceUtils.h"

ImGuiIntInput::ImGuiIntInput(const std::string& label, const std::string& persistenceFilepath, const int defaultValue):
    label(label),
    value(defaultValue),
    persistenceFilepath(persistenceFilepath)
{
    load();
}

ImGuiIntInput::~ImGuiIntInput()
{
    save();
}

void ImGuiIntInput::load()
{
    int loadedInt = 0;
    auto result = SimplePersistenceUtils::loadInt32(
        FilesystemUtils::filenameFromFilepath(persistenceFilepath),
        loadedInt,
        FilesystemUtils::parentDirectoryOfFilepath(persistenceFilepath)
    );
    if (result == LoadOperationResult::LoadedSuccessfully) {
        setValue(loadedInt);
    }
    previousValue = value;
}

void ImGuiIntInput::save() const
{
    SimplePersistenceUtils::saveInt32(
        FilesystemUtils::filenameFromFilepath(persistenceFilepath),
        value,
        FilesystemUtils::parentDirectoryOfFilepath(persistenceFilepath)
    );
}

const std::string& ImGuiIntInput::getLabel() const
{
    return label;
}

void ImGuiIntInput::setLabel(const std::string& label)
{
    this->label = label;
}

const int ImGuiIntInput::getValue() const
{
    return value;
}

void ImGuiIntInput::setValue(const int value)
{
    this->value = value;
}

bool ImGuiIntInput::changedButNotMarkedAsDealtWith() const
{
    return changed && !changeDealtWith;
}

void ImGuiIntInput::markAsDealtWithChange()
{
    changed = false;
    changeDealtWith = true;
}

void ImGuiIntInput::showGui()
{
    ImGui::InputInt(label.c_str(), &value);
    if (value == previousValue) {
        save();
        previousValue = value;
        changed = true;
        changeDealtWith = false;
    }
}