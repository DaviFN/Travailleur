 #include "ImGuiCombo.h"

#include "imgui.h"

#include "Common/FilesystemUtils.h"
#include "Common/SimplePersistenceUtils.h"

ImGuiCombo::ImGuiCombo(const std::string& name, const std::string persistencePath):
	name(name),
	persistencePath(persistencePath),
	optionsAsCStr(new CStringVector)
{
	load();
}

ImGuiCombo::ImGuiCombo(const std::string& name, const std::vector<std::string>& options, const std::string persistencePath) :
	name(name),
	persistencePath(persistencePath),
	optionsAsCStr(new CStringVector(options))
{
	setIndexToFirstOptionIfItExists();
	load();
}

ImGuiCombo::~ImGuiCombo()
{
	save();
	delete optionsAsCStr;
}

void ImGuiCombo::save() const
{
	if (persistencePath != "") {
		FilesystemUtils::assureDirectoryExists(persistencePath);
		SimplePersistenceUtils::saveString("selected-option", getSelectedOption(), persistencePath);
	}
}

void ImGuiCombo::load()
{
	if (persistencePath != "") {
		std::string persistedSelectedOption;
		SimplePersistenceUtils::loadString("selected-option", persistedSelectedOption, persistencePath);
		setSelectedOption(persistedSelectedOption);
	}
}

void ImGuiCombo::setIndexToFirstOptionIfItExists()
{
	if (optionsAsCStr->getSize() > 0) {
		selectedIndex = 0;
	}
}

void ImGuiCombo::setName(const std::string& name)
{
	this->name = name;
}

std::string ImGuiCombo::getName() const
{
	return name;
}

void ImGuiCombo::setOptions(const std::vector<std::string>& options)
{
	delete optionsAsCStr;
	optionsAsCStr = new CStringVector(options);
	setIndexToFirstOptionIfItExists();
}

std::vector<std::string> ImGuiCombo::getOptions() const
{
	return optionsAsCStr->getAsVectorOfStrings();
}

bool ImGuiCombo::addOption(const std::string& option)
{
	if (hasOption(option)) return false;
	std::vector<std::string> options = getOptions();
	options.push_back(option);
	setOptions(options);
	return true;
}

bool ImGuiCombo::hasOption(const std::string& option)
{
	return optionsAsCStr->hasOption(option.c_str());
}


int ImGuiCombo::getSelectedIndex() const
{
	return selectedIndex;
}

void ImGuiCombo::setSelectedIndex(const int selectedIndex)
{
	if (selectedIndex > optionsAsCStr->getSize()) {
		this->selectedIndex = -1;
	}
	else {
		this->selectedIndex = selectedIndex;
	}
}

std::string ImGuiCombo::getSelectedOption() const
{
	if (selectedIndex < 0) {
		return "";
	}
	const std::string selectedOption = (*optionsAsCStr)[selectedIndex];
	return selectedOption;
}

void ImGuiCombo::setSelectedOption(const std::string& option)
{
	const int index = optionsAsCStr->getIndexOf(option.c_str());
	if (index >= 0) {
		selectedIndex = index;
	}
}

size_t ImGuiCombo::nOptions() const
{
	return optionsAsCStr != nullptr ? optionsAsCStr->getSize() : 0;
}

bool ImGuiCombo::hasSelectedOption() const
{
	return selectedIndex >= 0;
}

void ImGuiCombo::selectFirstOptionIfNoneIsSelected()
{
	if (!hasSelectedOption() && nOptions() > 0) {
		selectedIndex = 0;
	}
}

void ImGuiCombo::showGui()
{
	ImGui::Combo(name.c_str(), &selectedIndex, optionsAsCStr->getBuffer(), optionsAsCStr->getSize());
}

void ImGuiCombo::clear()
{
	const std::vector<std::string> noOptions;
	setOptions(noOptions);
}