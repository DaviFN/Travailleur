#include "ImGuiSimpleColorSelector.h"

#include "imgui.h"

void ImGuiSimpleColorSelector(const std::string& label, Pixel& color)
{
	float colorComponents[3];
	colorComponents[0] = static_cast<float>(color.r) / 255.0;
	colorComponents[1] = static_cast<float>(color.g) / 255.0;
	colorComponents[2] = static_cast<float>(color.b) / 255.0;
	ImGui::ColorEdit3(label.c_str(), colorComponents);
	color.r = colorComponents[0] * 255.0;
	color.g = colorComponents[1] * 255.0;
	color.b = colorComponents[2] * 255.0;
}