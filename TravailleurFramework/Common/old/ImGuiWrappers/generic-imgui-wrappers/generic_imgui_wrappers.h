#ifndef generic_imgui_wrappers_H
#define generic_imgui_wrappers_H

#include <string>
#include <vector>

#include "TravailleurFramework\3rdparty\GUI\ImGui\dear-imgui\imgui.h"

#include <iostream>
#include <cstdio>

#include "TravailleurFramework\Common\CStringVector\CStringVector.h"

#include "TravailleurFramework\Core\RawImage\RawImage.h"

#include "TravailleurFramework\Common\common_functions\common_functions.h"

void ImGuiShowColor(int color);
void ImGuiColor(char* colorName, int* color);

void ImGuiDoublePercentageSlider(char* label, double* value, double range_min, double range_max, double speed);
void ImGuiDoubleSlider(char* label, double* value, double range_min, double range_max, double speed);
void ImGuiIntSlider(char* label, int* value, int range_min, int range_max, double speed);

void ImGuiDirectoryTree();

void comboFromStrings(std::string comboName, std::vector<std::string> strings, int* index);

void ImGuiListBox(char* name, int* index, char** items, int howManyItems, int height_in_items);

void ImGuiTextInput(std::string textInputName, char* text, int bufferSize, int howManyLines);

void ImGuiShowRawImage(RawImage* image);

#endif