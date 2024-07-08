#include "ImGuiDoubleSlider.h"

#include "imgui.h"

ImGuiDoubleSlider::ImGuiDoubleSlider(const std::string& label, const double defaultValue, const double rangeMin, const double rangeMax, const double dragSpeed, const std::string& persistenceFilepath):
    label(label),
    value(defaultValue),
    rangeMin(rangeMin),
    rangeMax(rangeMax),
    dragSpeed(dragSpeed),
    persistenceFilepath(persistenceFilepath)
{
    load();
}


ImGuiDoubleSlider::~ImGuiDoubleSlider()
{
    save();
}

void ImGuiDoubleSlider::load()
{
    auto result = SimplePersistenceUtils::loadDouble(
        FilesystemUtils::filenameFromFilepath(persistenceFilepath),
        value,
        FilesystemUtils::parentDirectoryOfFilepath(persistenceFilepath)
    );
    previousValue = value;
}

void ImGuiDoubleSlider::save() const
{
    SimplePersistenceUtils::saveDouble(
        FilesystemUtils::filenameFromFilepath(persistenceFilepath),
        value,
        FilesystemUtils::parentDirectoryOfFilepath(persistenceFilepath)
    );
}

const std::string& ImGuiDoubleSlider::getLabel() const
{
    return label;
}

void ImGuiDoubleSlider::setLabel(const std::string& label)
{
    this->label = label;
}

const double& ImGuiDoubleSlider::getRangeMin() const
{
    return rangeMin;
}

void ImGuiDoubleSlider::setRangeMin(const double rangeMin)
{
    this->rangeMin = rangeMin;
}

const double& ImGuiDoubleSlider::getRangeMax() const
{
    return rangeMax;
}

void ImGuiDoubleSlider::setRangeMax(const double rangeMax)
{
    this->rangeMax = rangeMax;
}

void ImGuiDoubleSlider::setDragSpeed(const double dragSpeed)
{
    this->dragSpeed = dragSpeed;
}

const double& ImGuiDoubleSlider::getDragSpeed() const
{
    return dragSpeed;
}

const double& ImGuiDoubleSlider::getValue() const
{
    return value;
}

bool ImGuiDoubleSlider::changedButNotMarkedAsDealtWith() const
{
    return changed && !changeDealtWith;
}

void ImGuiDoubleSlider::markAsDealtWithChange()
{
    changed = false;
    changeDealtWith = true;
}

void ImGuiDoubleSlider::showGui()
{
    float valueAsFloat = value;
    ImGui::DragFloat(label.c_str(), &valueAsFloat, dragSpeed, rangeMin, rangeMax, "%.4f");
    value = valueAsFloat;
    if (value != previousValue) {
        save();
        previousValue = value;
        changed = true;
    }
}