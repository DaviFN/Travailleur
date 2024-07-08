#ifndef ImGuiIntSlider_H
#define ImGuiIntSlider_H

#include <string>

#include "Common/FilesystemUtils.h"
#include "Common/SimplePersistenceUtils.h"

class ImGuiIntSlider
{
public:
	ImGuiIntSlider(const std::string& label, const int defaultValue, const int range_min, const int range_max, const double dragSpeed = 1.0, const std::string& persistenceFilepath = "");
	~ImGuiIntSlider();

	const std::string& getLabel() const;
	void setLabel(const std::string& label);

	void setRangeMin(const int rangeMin);
	const int& getRangeMin() const;

	void setRangeMax(const int rangeMax);
	const int& getRangeMax() const;

	void setDragSpeed(const double dragSpeed);
	const double& getDragSpeed() const;

	const int& getValue() const;
	void setValue(const int value);

	bool changedButNotMarkedAsDealtWith() const;

	void markAsDealtWithChange();

	void showGui();
private:
	void load();
	void save() const;

	std::string label;
	int value;
	int rangeMin;
	int rangeMax;
	double dragSpeed;
	std::string persistenceFilepath;
	int previousValue;
	bool changed = false;
	bool changeDealtWith = false;
};

#endif