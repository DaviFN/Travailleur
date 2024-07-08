#ifndef ImGuiSimpleIntRectangleSlider_H
#define ImGuiSimpleIntRectangleSlider_H

#include <string>

#include "Common/Rectangle2D.h"

void ImGuiSimpleIntRectangleSlider(const std::string& label, Rectangle2D<int>* value, const double dragSpeed, const int topLeftRangeMin, const int topLeftRangeMax, const int sizeRangeMin, const int sizeRangeMax);

#endif