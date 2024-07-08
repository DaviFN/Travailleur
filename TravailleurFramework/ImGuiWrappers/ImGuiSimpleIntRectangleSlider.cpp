#include "ImGuiSimpleIntRectangleSlider.h"

#include "imgui.h"

void ImGuiSimpleIntRectangleSlider(const std::string& label, Rectangle2D<int>* value, const double dragSpeed, const int topLeftRangeMin, const int topLeftRangeMax, const int sizeRangeMin, const int sizeRangeMax)
{
    ImGui::DragScalar((label + ".topLeft.x").c_str(), ImGuiDataType_S32, &value->topLeft.x, dragSpeed, &topLeftRangeMin, &topLeftRangeMax, 0, 1.0f);
    ImGui::DragScalar((label + ".topLeft.y").c_str(), ImGuiDataType_S32, &value->topLeft.y, dragSpeed, &topLeftRangeMin, &topLeftRangeMax, 0, 1.0f);
    ImGui::DragScalar((label + ".size.x").c_str(), ImGuiDataType_S32, &value->size.x, dragSpeed, &sizeRangeMin, &sizeRangeMax, 0, 1.0f);
    ImGui::DragScalar((label + ".size.y").c_str(), ImGuiDataType_S32, &value->size.y, dragSpeed, &sizeRangeMin, &sizeRangeMax, 0, 1.0f);
}