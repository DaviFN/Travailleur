#include "InterfaceConfig.h"

#include "Common/SimplePersistenceUtils.h"
#include "ImGuiWrappers/ImGuiConsole.h"
#include "TravailleurContext.h"

#include "imgui.h"

InterfaceConfig::InterfaceConfig(TravailleurContext* travailleurContext):
	travailleurContext(travailleurContext),
	styleColorsCombo("Style colors")
{
	assureSaveDirectoryExists();

	initializeStyleColorsCombo();
	fontScaleFactor = std::make_unique<ImGuiDoubleSlider>("fontScaleFactor", 1, 1, 2, 0.01, getSaveDirectory() + "\\fontScaleFactor");
	
	travailleurConsoleBackgroundColor = std::make_unique<ImGuiColorSelector>("travailleurConsoleBackgroundColor", ImGuiConsole::getDefaultBackgroundColor(), travailleurContext->getPath() + "\\travailleurConsoleBackgroundColor");
	travailleurConsoleCommandInputMessageColor = std::make_unique<ImGuiColorSelector>("travailleurConsoleCommandInputMessageColor", ImGuiConsole::getDefaultCommandInputMessageColor(), travailleurContext->getPath() + "\\travailleurConsoleCommandInputMessageColor");
	travailleurConsoleUnrecognizedCommandMessageColor = std::make_unique<ImGuiColorSelector>("travailleurConsoleUnrecognizedCommandMessageColor", ImGuiConsole::getDefaultUnrecognizedCommandMessageColor(), travailleurContext->getPath() + "\\travailleurConsoleUnrecognizedCommandMessageColor");
	travailleurConsoleNormalMessageColor = std::make_unique<ImGuiColorSelector>("travailleurConsoleNormalMessageColor", ImGuiConsole::getDefaultNormalMessageColor(), travailleurContext->getPath() + "\\travailleurConsoleNormalMessageColor");
	travailleurConsoleErrorMessageColor = std::make_unique<ImGuiColorSelector>("travailleurConsoleErrorMessageColor", ImGuiConsole::getDefaultErrorMessageColor(), travailleurContext->getPath() + "\\travailleurConsoleErrorMessageColor");
	travailleurConsoleWarningMessageColor = std::make_unique<ImGuiColorSelector>("travailleurConsoleWarningMessageColor", ImGuiConsole::getDefaultWarningMessageColor(), travailleurContext->getPath() + "\\travailleurConsoleWarningMessageColor");
	travailleurConsoleInfoMessageColor = std::make_unique<ImGuiColorSelector>("travailleurConsoleInfoMessageColor", ImGuiConsole::getDefaultInfoMessageColor(), travailleurContext->getPath() + "\\travailleurConsoleInfoMessageColor");
	travailleurConsoleSuccessMessageColor = std::make_unique<ImGuiColorSelector>("travailleurConsoleSuccessMessageColor", ImGuiConsole::getDefaultSuccessMessageColor(), travailleurContext->getPath() + "\\travailleurConsoleSuccessMessageColor");
	travailleurConsoleDebugMessageColor = std::make_unique<ImGuiColorSelector>("travailleurConsoleDebugMessageColor", ImGuiConsole::getDefaultDebugMessageColor(), travailleurContext->getPath() + "\\travailleurConsoleDebugMessageColor");

	load();
}

InterfaceConfig::~InterfaceConfig()
{
	save();
}

void InterfaceConfig::save() const
{
	SimplePersistenceUtils::saveString("styleColors", styleColorsCombo.getSelectedOption(), getSaveDirectory());
}

void InterfaceConfig::load()
{
	std::string savedStyleColors;
	SimplePersistenceUtils::loadString("styleColors", savedStyleColors, getSaveDirectory());
	styleColorsCombo.setSelectedOption(savedStyleColors);
	styleColorsCombo.selectFirstOptionIfNoneIsSelected();
}

void InterfaceConfig::assureSaveDirectoryExists() const
{
	FilesystemUtils::assureDirectoryExists(getSaveDirectory());
}

std::string InterfaceConfig::getSaveDirectory() const
{
	return travailleurContext->getPath() + "\\interface-config";
}

void InterfaceConfig::initializeStyleColorsCombo()
{
	std::vector<std::string> options;
	options.push_back("Light");
	options.push_back("Classic");
	options.push_back("Dark");
	styleColorsCombo.setOptions(options);
}

InterfaceConfig::StyleColors InterfaceConfig::getStyleColors() const
{
	const std::string selectedOption = styleColorsCombo.getSelectedOption();
	if (selectedOption == "Light") {
		return StyleColors::Light;
	}
	if (selectedOption == "Classic") {
		return StyleColors::Classic;
	}
	if (selectedOption == "Dark") {
		return StyleColors::Dark;
	}

	return StyleColors::Light;
}

float InterfaceConfig::getFontScaleFactor() const
{
	return static_cast<float>(fontScaleFactor->getValue());
}

Pixel InterfaceConfig::getTravailleurConsoleBackgroundColor() const
{
	return travailleurConsoleBackgroundColor->getColor();
}

Pixel InterfaceConfig::getTravailleurConsoleCommandInputMessageColor() const
{
	return travailleurConsoleCommandInputMessageColor->getColor();
}

Pixel InterfaceConfig::getTravailleurConsoleUnrecognizedCommandMessageColor() const
{
	return travailleurConsoleUnrecognizedCommandMessageColor->getColor();
}

Pixel InterfaceConfig::getTravailleurConsoleNormalMessageColor() const
{
	return travailleurConsoleNormalMessageColor->getColor();
}

Pixel InterfaceConfig::getTravailleurConsoleErrorMessageColor() const
{
	return travailleurConsoleErrorMessageColor->getColor();
}

Pixel InterfaceConfig::getTravailleurConsoleWarningMessageColor() const
{
	return travailleurConsoleWarningMessageColor->getColor();
}

Pixel InterfaceConfig::getTravailleurConsoleInfoMessageColor() const
{
	return travailleurConsoleInfoMessageColor->getColor();
}

Pixel InterfaceConfig::getTravailleurConsoleSuccessMessageColor() const
{
	return travailleurConsoleSuccessMessageColor->getColor();
}

Pixel InterfaceConfig::getTravailleurConsoleDebugMessageColor() const
{
	return travailleurConsoleDebugMessageColor->getColor();
}

void InterfaceConfig::showGui()
{
	ImGui::Begin("InterfaceConfig");

	styleColorsCombo.showGui();
	fontScaleFactor->showGui();
	
	travailleurConsoleBackgroundColor->showGui();
	travailleurConsoleCommandInputMessageColor->showGui();
	travailleurConsoleUnrecognizedCommandMessageColor->showGui();
	travailleurConsoleNormalMessageColor->showGui();
	travailleurConsoleErrorMessageColor->showGui();
	travailleurConsoleWarningMessageColor->showGui();
	travailleurConsoleInfoMessageColor->showGui();
	travailleurConsoleSuccessMessageColor->showGui();
	travailleurConsoleDebugMessageColor->showGui();

	ImGui::End();
}