#include "ImGuiHSVRangeInput.h"

#include "ImGuiColorOutput.h"
#include "ImGuiSimpleDoubleSlider.h"
#include "imgui.h"
#include "Common/ColorUtils.h"

void assureDoubleIsInRange(double* d, double rangeMin, double rangeMax)
{
    if (*d < rangeMin)
    {
        *d = rangeMin;
    }
    else if (*d > rangeMax)
    {
        *d = rangeMax;
    }
}

void ImGuiHSVRangeInput(const std::string& prefix, double* h_lower, double* s_lower, double* v_lower, double* h_upper, double* s_upper, double* v_upper, const double dragSpeed)
{
    assureDoubleIsInRange(h_lower, 0.0, 179.0);
    assureDoubleIsInRange(s_lower, 0.0, 255.0);
    assureDoubleIsInRange(v_lower, 0.0, 255.0);

    int r_lower = 0;
    int g_lower = 0;
    int b_lower = 0;
    ColorUtils::HSVtoRGB(*h_lower, *s_lower, *v_lower, r_lower, g_lower, b_lower);

    ImGuiColorOutput("hsv lower:", Pixel(r_lower, g_lower, b_lower));

    ImGuiSimpleDoubleSlider(prefix + " h_lower", h_lower, dragSpeed, 0.0, 179.0);
    ImGuiSimpleDoubleSlider(prefix + " s_lower", s_lower, dragSpeed, 0.0, 255.0);
    ImGuiSimpleDoubleSlider(prefix + " v_lower", v_lower, dragSpeed, 0.0, 255.0);

    int r_upper = 0;
    int g_upper = 0;
    int b_upper = 0;
    ColorUtils::HSVtoRGB(*h_upper, *s_upper, *v_upper, r_upper, g_upper, b_upper);

    ImGuiColorOutput("hsv upper:", Pixel(r_upper, g_upper, b_upper));

    ImGuiSimpleDoubleSlider(prefix + " h_upper", h_upper, dragSpeed, std::min(0.0, *h_lower), 179.0);
    ImGuiSimpleDoubleSlider(prefix + " s_upper", s_upper, dragSpeed, std::min(0.0, *s_lower), 255.0);
    ImGuiSimpleDoubleSlider(prefix + " v_upper", v_upper, dragSpeed, std::min(0.0, *v_lower), 255.0);

    *h_upper = std::max(*h_lower, *h_upper);
    *s_upper = std::max(*s_lower, *s_upper);
    *v_upper = std::max(*v_lower, *v_upper);
}