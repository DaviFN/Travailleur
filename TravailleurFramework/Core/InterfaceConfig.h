#ifndef InterfaceConfig_H
#define InterfaceConfig_H

#include <memory>

#include "Common/Image.h"
#include "ImGuiWrappers/ImGuiColorSelector.h"
#include "ImGuiWrappers/ImGuiCombo.h"
#include "ImGuiWrappers/ImGuiDoubleSlider.h"

class TravailleurContext;

class InterfaceConfig
{
public:
    enum class StyleColors {
        Undefined,
        Light,
        Classic,
        Dark
    };

    InterfaceConfig(TravailleurContext* travailleurContext);
    ~InterfaceConfig();

    StyleColors getStyleColors() const;

    float getFontScaleFactor() const;

    Pixel getTravailleurConsoleBackgroundColor() const;
    Pixel getTravailleurConsoleCommandInputMessageColor() const;
    Pixel getTravailleurConsoleUnrecognizedCommandMessageColor() const;
    Pixel getTravailleurConsoleNormalMessageColor() const;
    Pixel getTravailleurConsoleErrorMessageColor() const;
    Pixel getTravailleurConsoleWarningMessageColor() const;
    Pixel getTravailleurConsoleInfoMessageColor() const;
    Pixel getTravailleurConsoleSuccessMessageColor() const;
    Pixel getTravailleurConsoleDebugMessageColor() const;

    void showGui();

private:
    void save() const;
    void load();

    void assureSaveDirectoryExists() const;
    std::string getSaveDirectory() const;

    void initializeStyleColorsCombo();

    std::unique_ptr<ImGuiDoubleSlider> fontScaleFactor;

    TravailleurContext* travailleurContext = nullptr;

    ImGuiCombo styleColorsCombo;

    std::unique_ptr<ImGuiColorSelector> travailleurConsoleBackgroundColor;
    std::unique_ptr<ImGuiColorSelector> travailleurConsoleCommandInputMessageColor;
    std::unique_ptr<ImGuiColorSelector> travailleurConsoleUnrecognizedCommandMessageColor;
    std::unique_ptr<ImGuiColorSelector> travailleurConsoleNormalMessageColor;
    std::unique_ptr<ImGuiColorSelector> travailleurConsoleErrorMessageColor;
    std::unique_ptr<ImGuiColorSelector> travailleurConsoleWarningMessageColor;
    std::unique_ptr<ImGuiColorSelector> travailleurConsoleInfoMessageColor;
    std::unique_ptr<ImGuiColorSelector> travailleurConsoleSuccessMessageColor;
    std::unique_ptr<ImGuiColorSelector> travailleurConsoleDebugMessageColor;
};

#endif