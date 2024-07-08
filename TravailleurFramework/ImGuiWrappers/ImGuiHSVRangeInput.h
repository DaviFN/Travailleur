#ifndef ImGuiHSVRangeInput_H
#define ImGuiHSVRangeInput_H

#include <string>

void ImGuiHSVRangeInput(const std::string& prefix, double* h_lower, double* s_lower, double* v_lower, double* h_upper, double* s_upper, double* v_upper, const double dragSpeed);

#endif