#include "ImGuiColorOutput.h"

#include "imgui.h"

#include "ImGuiWrappers/ImGuiTextOutput.h"

void ImGuiColorOutput(const std::string& label, const Pixel& color)
{
    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(color.r, color.g, color.b, color.a));
    const std::string rgbDescription = std::to_string(color.r) + ", " + std::to_string(color.g) + ", " + std::to_string(color.b);
    ImGui::Button((label + " (" + rgbDescription + ")").c_str());
    ImGui::PopStyleColor();
}