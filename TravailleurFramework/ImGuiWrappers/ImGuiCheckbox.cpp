#include "ImGuiCheckbox.h"

#include "Common/FilesystemUtils.h"
#include "imgui.h"

ImGuiCheckbox::ImGuiCheckbox(const std::string& label, const bool defaultValue, const std::string& persistenceFilepath) :
label(label),
persistenceFilepath(persistenceFilepath),
boolean(defaultValue)
{
	load();
}

ImGuiCheckbox::~ImGuiCheckbox()
{
	save();
}

void ImGuiCheckbox::load()
{
	if (persistenceFilepath == "") return;

	auto result = SimplePersistenceUtils::loadBool(
		FilesystemUtils::filenameFromFilepath(persistenceFilepath),
		boolean,
		FilesystemUtils::parentDirectoryOfFilepath(persistenceFilepath)
	);
	previousBooleanValue = boolean;
}

void ImGuiCheckbox::save() const
{
	if (persistenceFilepath == "") return;

	SimplePersistenceUtils::saveBool(
		FilesystemUtils::filenameFromFilepath(persistenceFilepath),
		boolean,
		FilesystemUtils::parentDirectoryOfFilepath(persistenceFilepath)
	);
}

bool ImGuiCheckbox::getValue() const
{
	return boolean;
}

void ImGuiCheckbox::setValue(const bool value)
{
	boolean = value;
	save();
}

void ImGuiCheckbox::showGui()
{
	if (ImGui::Checkbox(label.c_str(), &boolean)) {
		if (previousBooleanValue != true) {
			save();
			previousBooleanValue = false;
		}
	}
	else {
		if (previousBooleanValue != false) {
			save();
			previousBooleanValue = true;
		}
	}
}

void ImGuiCheckbox::simpleCheckbox(const std::string& label, bool* boolean)
{
	ImGui::Checkbox(label.c_str(), boolean);
}
