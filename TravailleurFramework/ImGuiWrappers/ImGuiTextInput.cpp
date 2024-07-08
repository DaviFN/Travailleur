#include "ImGuiTextInput.h"

#include "imgui.h"

#include "Common/FilesystemUtils.h"
#include "Common/SimplePersistenceUtils.h"

size_t ImGuiTextInput::bufferSize = 4096;

ImGuiTextInput::ImGuiTextInput(const std::string& label, const std::string& persistenceFilepath, const std::string& defaultValue):
    label(label),
    persistenceFilepath(persistenceFilepath)
{
    value = new char[bufferSize];
    setValue(defaultValue);

    load();
}

ImGuiTextInput::~ImGuiTextInput()
{
    save();

    delete value;
}

void ImGuiTextInput::load()
{
    std::string loadedString;
    auto result = SimplePersistenceUtils::loadString(
        FilesystemUtils::filenameFromFilepath(persistenceFilepath),
        loadedString,
        FilesystemUtils::parentDirectoryOfFilepath(persistenceFilepath)
    );
    if (result == LoadOperationResult::LoadedSuccessfully) {
        setValue(loadedString);
    }
    previousValue = value;
}

void ImGuiTextInput::save() const
{
    SimplePersistenceUtils::saveString(
        FilesystemUtils::filenameFromFilepath(persistenceFilepath),
        value,
        FilesystemUtils::parentDirectoryOfFilepath(persistenceFilepath)
    );
}

const std::string& ImGuiTextInput::getLabel() const
{
    return label;
}

void ImGuiTextInput::setLabel(const std::string& label)
{
    this->label = label;
}

const char* ImGuiTextInput::getValue() const
{
    return value;
}

void ImGuiTextInput::setValue(const std::string& value)
{
    strcpy(this->value, value.c_str());
}

bool ImGuiTextInput::changedButNotMarkedAsDealtWith() const
{
    return changed && !changeDealtWith;
}

void ImGuiTextInput::markAsDealtWithChange()
{
    changed = false;
    changeDealtWith = true;
}

bool ImGuiTextInput::isMultiLine() const
{
    return multiLine;
}

void ImGuiTextInput::setMultiLine(const bool multiLine)
{
    this->multiLine = multiLine;
}

int ImGuiTextInput::getMultiLineWidth() const
{
    return multiLineWidth;
}

void ImGuiTextInput::setMultiLineWidth(const int multiLineWidth)
{
    this->multiLineWidth = multiLineWidth;
}

int ImGuiTextInput::getMultiLineHeight() const
{
    return multiLineHeight;
}

void ImGuiTextInput::setMultiLineHeight(const int multiLineHeight)
{
    this->multiLineHeight = multiLineHeight;
}

void ImGuiTextInput::showGui()
{
    static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
    
    if (multiLine) {
        ImGui::InputTextMultiline(label.c_str(), value, bufferSize, ImVec2(multiLineWidth, multiLineHeight), flags);
    }
    else {
        ImGui::InputText(label.c_str(), value, bufferSize, flags);
    }
    
    if (strcmp(value, previousValue.c_str()) != 0) {
        save();
        previousValue = value;
        changed = true;
        changeDealtWith = false;
    }
}