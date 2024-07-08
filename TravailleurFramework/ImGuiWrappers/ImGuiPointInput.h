#ifndef ImGuiPointInput_H
#define ImGuiPointInput_H

#include <string>

#include "Common/Point2D.h"

class ImGuiPointInput
{
public:
	ImGuiPointInput(const std::string& label, const int rangeMin, const double rangeMax, const double dragSpeed, const std::string& persistenceFilepath = "", const Point2D<int>& defaultValue = Point2D<int>(0, 0));
	~ImGuiPointInput();

	const std::string& getLabel() const;
	void setLabel(const std::string& label);

	Point2D<int> getValue() const;
	void setValue(const Point2D<int>& value);

	bool changedButNotMarkedAsDealtWith() const;

	void markAsDealtWithChange();

	void showGui();
private:
	void load();
	void save() const;

	static size_t bufferSize;

	std::string label;
	Point2D<int> value;
	int rangeMin;
	int rangeMax;
	double dragSpeed;
	std::string persistenceFilepath;
	Point2D<int> previousValue;
	bool changed = false;
	bool changeDealtWith = false;
};

#endif