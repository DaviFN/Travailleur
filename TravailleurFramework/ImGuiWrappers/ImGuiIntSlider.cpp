#include "ImGuiIntSlider.h"

#include "imgui.h"

ImGuiIntSlider::ImGuiIntSlider(const std::string& label, const int defaultValue, const int rangeMin, const int rangeMax, const double dragSpeed, const std::string& persistenceFilepath):
    label(label),
    value(defaultValue),
    rangeMin(rangeMin),
    rangeMax(rangeMax),
    dragSpeed(dragSpeed),
    persistenceFilepath(persistenceFilepath)
{
    load();
}


ImGuiIntSlider::~ImGuiIntSlider()
{
    save();
}

void ImGuiIntSlider::load()
{
    auto result = SimplePersistenceUtils::loadInt32(
        FilesystemUtils::filenameFromFilepath(persistenceFilepath),
        value,
        FilesystemUtils::parentDirectoryOfFilepath(persistenceFilepath)
    );
    previousValue = value;
}

void ImGuiIntSlider::save() const
{
    SimplePersistenceUtils::saveInt32(
        FilesystemUtils::filenameFromFilepath(persistenceFilepath),
        value,
        FilesystemUtils::parentDirectoryOfFilepath(persistenceFilepath)
    );
}

const std::string& ImGuiIntSlider::getLabel() const
{
    return label;
}

void ImGuiIntSlider::setLabel(const std::string& label)
{
    this->label = label;
}

const int& ImGuiIntSlider::getRangeMin() const
{
    return rangeMin;
}

void ImGuiIntSlider::setRangeMin(const int rangeMin)
{
    this->rangeMin = rangeMin;
}

const int& ImGuiIntSlider::getRangeMax() const
{
    return rangeMax;
}

void ImGuiIntSlider::setRangeMax(const int rangeMax)
{
    this->rangeMax = rangeMax;
}

void ImGuiIntSlider::setDragSpeed(const double dragSpeed)
{
    this->dragSpeed = dragSpeed;
}

const double& ImGuiIntSlider::getDragSpeed() const
{
    return dragSpeed;
}

const int& ImGuiIntSlider::getValue() const
{
    return value;
}

void ImGuiIntSlider::setValue(const int value)
{
    this->value = value;
}

bool ImGuiIntSlider::changedButNotMarkedAsDealtWith() const
{
    return changed && !changeDealtWith;
}

void ImGuiIntSlider::markAsDealtWithChange()
{
    changed = false;
    changeDealtWith = true;
}

void ImGuiIntSlider::showGui()
{
    ImGui::DragScalar(label.c_str(), ImGuiDataType_S32, &value, dragSpeed, &rangeMin, &rangeMax, 0, 1.0f);
    if (value != previousValue) {
        save();
        previousValue = value;
        changed = true;
        changeDealtWith = false;
    }
}