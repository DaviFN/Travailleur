#include "ImGuiSimpleIntSlider.h"

#include "imgui.h"

void ImGuiSimpleIntSlider(const std::string& label, int* value, const double dragSpeed, const int rangeMin, const int rangeMax)
{
    ImGui::DragScalar(label.c_str(), ImGuiDataType_S32, value, dragSpeed, &rangeMin, &rangeMax, 0, 1.0f);
}