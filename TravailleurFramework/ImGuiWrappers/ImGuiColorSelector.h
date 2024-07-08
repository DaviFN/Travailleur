#ifndef ImGuiColorSelector_H
#define ImGuiColorSelector_H

#include <string>

#include "Common/FilesystemUtils.h"
#include "Common/Image.h"
#include "Common/SimplePersistenceUtils.h"

class ImGuiColorSelector
{
public:
	ImGuiColorSelector(const std::string& label, const Pixel& defaultColor, const std::string& persistenceFilepath = "");
	~ImGuiColorSelector();

	const std::string& getLabel() const;
	void setLabel(const std::string& label);

	const Pixel& getColor() const;

	bool changedButNotMarkedAsDealtWith() const;

	void markAsDealtWithChange();

	void showGui();
private:
	void load();
	void save() const;

	std::string label;
	Pixel color;
	std::string persistenceFilepath;
	Pixel previousColor;
	bool changed = false;
	bool changeDealtWith = false;
};

#endif