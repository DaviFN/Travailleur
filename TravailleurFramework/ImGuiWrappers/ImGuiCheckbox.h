#ifndef ImGuiCheckbox_H
#define ImGuiCheckbox_H

#include <string>

#include "Common/SimplePersistenceUtils.h"

class ImGuiCheckbox
{
public:
	ImGuiCheckbox(const std::string& label, const bool defaultValue = false, const std::string& persistenceFilepath = "");
	~ImGuiCheckbox();

	static void simpleCheckbox(const std::string& label, bool* boolean);

	bool getValue() const;
	void setValue(const bool value);

	void showGui();

private:
	void load();
	void save() const;

	std::string label;
	std::string persistenceFilepath;
	bool boolean;

	bool previousBooleanValue;
};

#endif