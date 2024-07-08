#include "ImGuiSimpleDoubleSlider.h"

#include "imgui.h"

void ImGuiSimpleDoubleSlider(const std::string& label, double* value, const double dragSpeed, const double rangeMin, const double rangeMax)
{
    float valueAsFloat = *value;
    ImGui::DragFloat(label.c_str(), &valueAsFloat, dragSpeed, rangeMin, rangeMax, "%.4f");
    *value = valueAsFloat;
}