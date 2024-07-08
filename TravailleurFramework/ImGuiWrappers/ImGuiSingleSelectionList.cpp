#include "ImGuiSingleSelectionList.h"

#include "imgui.h"

#include "ImGuiWrappers/ImGuiTextOutput.h"

ImGuiSingleSelectionList::ImGuiSingleSelectionList(const std::string& name):
	name(name),
	optionsAsCStr(new CStringVector)
{
}

ImGuiSingleSelectionList::ImGuiSingleSelectionList(const std::string& name, const std::vector<std::string>& options) :
	name(name),
	optionsAsCStr(new CStringVector(options))
{
	setIndexToFirstOptionIfItExists();
}

ImGuiSingleSelectionList::~ImGuiSingleSelectionList()
{
	delete optionsAsCStr;
}

void ImGuiSingleSelectionList::setSelectionToNone()
{
	selectedIndex = -1;
}

void ImGuiSingleSelectionList::setIndexToFirstOptionIfItExists()
{
	if (optionsAsCStr->getSize() > 0) {
		selectedIndex = 0;
	}
	else {
		selectedIndex = -1;
	}
}

void ImGuiSingleSelectionList::setName(const std::string& name)
{
	this->name = name;
}

std::string ImGuiSingleSelectionList::getName() const
{
	return name;
}

void ImGuiSingleSelectionList::setOptions(const std::vector<std::string>& options)
{
	delete optionsAsCStr;
	optionsAsCStr = new CStringVector(options);
	setIndexToFirstOptionIfItExists();
}

std::vector<std::string> ImGuiSingleSelectionList::getOptions() const
{
	return optionsAsCStr->getAsVectorOfStrings();
}

bool ImGuiSingleSelectionList::addOption(const std::string& option)
{
	if (hasOption(option)) return false;
	std::vector<std::string> options = getOptions();
	options.push_back(option);
	setOptions(options);
	return true;
}

bool ImGuiSingleSelectionList::hasOption(const std::string& option)
{
	return optionsAsCStr->hasOption(option.c_str());
}

bool ImGuiSingleSelectionList::hasSelectedOption() const
{
	return getSelectedIndex() >= 0;
}

int ImGuiSingleSelectionList::getSelectedIndex() const
{
	return selectedIndex;
}

void ImGuiSingleSelectionList::setSelectedIndex(const int selectedIndex)
{
	if (selectedIndex < 0 || optionsAsCStr->getSize() == 0 || selectedIndex > optionsAsCStr->getSize()) {
		this->selectedIndex = -1;
	}
	else {
		this->selectedIndex = selectedIndex;
	}
	lastSelectedIndex = this->selectedIndex;
}

std::string ImGuiSingleSelectionList::getSelectedOption() const
{
	if (selectedIndex < 0) {
		return "";
	}
	const std::string selectedOption = (*optionsAsCStr)[selectedIndex];
	return selectedOption;
}

void ImGuiSingleSelectionList::setSelectedOption(const std::string& option)
{
	const int index = optionsAsCStr->getIndexOf(option.c_str());
	if (index >= 0) {
		selectedIndex = index;
	}
	lastSelectedIndex = index;
}

void ImGuiSingleSelectionList::showGui()
{
	ImGuiTextOutput(name);
	ImGuiTextOutput("selected option: " + getSelectedOption());
	ImGui::ListBox(name.c_str(), &selectedIndex, optionsAsCStr->getBuffer(), optionsAsCStr->getSize(), howManyItemsToShow);
	if (selectedIndex != lastSelectedIndex) {
		changed = true;
		changeDealtWith = false;
		previouslySelectedIndex = lastSelectedIndex;
		lastSelectedIndex = selectedIndex;
	}
}

void ImGuiSingleSelectionList::setHowManyItemsToShow(const size_t howManyItemsToShow)
{
	this->howManyItemsToShow = howManyItemsToShow;
}

bool ImGuiSingleSelectionList::changedButNotMarkedAsDealtWith() const
{
	return changed && !changeDealtWith;
}

void ImGuiSingleSelectionList::markAsDealtWithChange()
{
	changed = false;
	changeDealtWith = true;
}

int ImGuiSingleSelectionList::nOptions() const
{
	return optionsAsCStr->getSize();
}

void ImGuiSingleSelectionList::clear()
{
	const std::vector<std::string> noOptions;
	setOptions(noOptions);
}

bool ImGuiSingleSelectionList::hasOptionWithIndex(const int index) const
{
	if (index < 0) return false;
	if (index >= optionsAsCStr->getSize()) return false;
	return true;
}

int ImGuiSingleSelectionList::getPreviouslySelectedIndex() const
{
	return previouslySelectedIndex;
}