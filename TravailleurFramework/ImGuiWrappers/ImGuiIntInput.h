#ifndef ImGuiIntInput_H
#define ImGuiIntInput_H

#include <string>

class ImGuiIntInput
{
public:
	ImGuiIntInput(const std::string& label, const std::string& persistenceFilepath = "", const int defaultValue = 0);
	~ImGuiIntInput();

	const std::string& getLabel() const;
	void setLabel(const std::string& label);

	const int getValue() const;
	void setValue(const int value);

	bool changedButNotMarkedAsDealtWith() const;

	void markAsDealtWithChange();

	void showGui();
private:
	void load();
	void save() const;

	std::string label;
	int value = 0;
	std::string persistenceFilepath;
	int previousValue;
	bool changed = false;
	bool changeDealtWith = false;
};

#endif