#ifndef ImGuiSimpleIntSlider_H
#define ImGuiSimpleIntSlider_H

#include <string>

void ImGuiSimpleIntSlider(const std::string& label, int* value, const double dragSpeed, const int rangeMin, const int rangeMax);

#endif