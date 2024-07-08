#ifndef ImGuiTextInput_H
#define ImGuiTextInput_H

#include <string>

class ImGuiTextInput
{
public:
	ImGuiTextInput(const std::string& label, const std::string& persistenceFilepath = "", const std::string& defaultValue = "");
	~ImGuiTextInput();

	const std::string& getLabel() const;
	void setLabel(const std::string& label);

	const char* getValue() const;
	void setValue(const std::string& value);

	bool changedButNotMarkedAsDealtWith() const;

	void markAsDealtWithChange();

	bool isMultiLine() const;
	void setMultiLine(const bool multiLine);

	int getMultiLineWidth() const;
	void setMultiLineWidth(const int multiLineWidth);

	int getMultiLineHeight() const;
	void setMultiLineHeight(const int multiLineHeight);

	void showGui();
private:
	void load();
	void save() const;

	static size_t bufferSize;

	std::string label;
	char* value;
	std::string persistenceFilepath;
	std::string previousValue;
	bool changed = false;
	bool changeDealtWith = false;

	bool multiLine = false;
	int multiLineWidth = 500;
	int multiLineHeight = 500;
};

#endif