#include "ImGuiColorSelector.h"

#include "ImGuiSimpleColorSelector.h"
#include "imgui.h"

ImGuiColorSelector::ImGuiColorSelector(const std::string& label, const Pixel& defaultColor, const std::string& persistenceFilepath):
    label(label),
    color(defaultColor),
    persistenceFilepath(persistenceFilepath)
{
    load();
}


ImGuiColorSelector::~ImGuiColorSelector()
{
    save();
}

void ImGuiColorSelector::load()
{
    if (!persistenceFilepath.empty()) {
        FILE* f = fopen(persistenceFilepath.c_str(), "rb");
        if (f != nullptr) {
            fread(&color.r, sizeof(color.r), 1, f);
            fread(&color.g, sizeof(color.g), 1, f);
            fread(&color.b, sizeof(color.b), 1, f);
            fread(&color.a, sizeof(color.a), 1, f);
            fclose(f);
        }
    }
}

void ImGuiColorSelector::save() const
{
    if (!persistenceFilepath.empty()) {
        FILE* f = fopen(persistenceFilepath.c_str(), "wb");
        if (f != nullptr) {
            fwrite(&color.r, sizeof(color.r), 1, f);
            fwrite(&color.g, sizeof(color.g), 1, f);
            fwrite(&color.b, sizeof(color.b), 1, f);
            fwrite(&color.a, sizeof(color.a), 1, f);
            fclose(f);
        }
    }
}

const std::string& ImGuiColorSelector::getLabel() const
{
    return label;
}

void ImGuiColorSelector::setLabel(const std::string& label)
{
    this->label = label;
}

const Pixel& ImGuiColorSelector::getColor() const
{
    return color;
}

bool ImGuiColorSelector::changedButNotMarkedAsDealtWith() const
{
    return changed && !changeDealtWith;
}

void ImGuiColorSelector::markAsDealtWithChange()
{
    changed = false;
    changeDealtWith = true;
}

void ImGuiColorSelector::showGui()
{
    ImGuiSimpleColorSelector(label, color);
    
    if (color != previousColor) {
        save();
        previousColor = color;
        changed = true;
    }
}