#include "ImGuiTextOutput.h"

#include "imgui.h"

#include "ImGuiWrappers/ImGuiUtils.h"

void ImGuiTextOutput(const std::string& text)
{
    ImGui::Text(text.c_str());
}

void ImGuiTextOutput(const std::string& text, const Pixel color)
{
    const ImVec4 imGuiColor = ImGuiUtils::pixelToImVec4(color);
    ImGui::PushStyleColor(ImGuiCol_Text, imGuiColor);
    ImGui::Text(text.c_str());
    ImGui::PopStyleColor();
}