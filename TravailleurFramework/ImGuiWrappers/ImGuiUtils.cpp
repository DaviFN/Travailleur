#include "ImGuiUtils.h"

ImColor ImGuiUtils::pixelToImVec4(const Pixel& pixel)
{
	return ImVec4((float)(pixel.r) / 255.0, (float)(pixel.g) / 255.0, (float)(pixel.b) / 255.0, (float)(pixel.a) / 255.0);
}