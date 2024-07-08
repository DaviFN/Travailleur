#ifndef ImGuiDoubleSlider_H
#define ImGuiDoubleSlider_H

#include <string>

#include "Common/FilesystemUtils.h"
#include "Common/SimplePersistenceUtils.h"

class ImGuiDoubleSlider
{
public:
	ImGuiDoubleSlider(const std::string& label, const double defaultValue, const double range_min, const double range_max, const double dragSpeed = 0.05, const std::string& persistenceFilepath = "");
	~ImGuiDoubleSlider();

	const std::string& getLabel() const;
	void setLabel(const std::string& label);

	void setRangeMin(const double rangeMin);
	const double& getRangeMin() const;

	void setRangeMax(const double rangeMax);
	const double& getRangeMax() const;

	void setDragSpeed(const double dragSpeed);
	const double& getDragSpeed() const;

	const double& getValue() const;

	bool changedButNotMarkedAsDealtWith() const;

	void markAsDealtWithChange();

	void showGui();
private:
	void load();
	void save() const;

	std::string label;
	double value;
	double rangeMin;
	double rangeMax;
	double dragSpeed;
	std::string persistenceFilepath;
	int previousValue;
	bool changed = false;
	bool changeDealtWith = false;
};

#endif