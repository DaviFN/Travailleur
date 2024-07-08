#ifndef PointInput_H
#define PointInput_H

#include "Common/Point2D.h"
#include "ImGuiWrappers/ImGuiIntSlider.h"

class TravailleurContext;

class PointInput
{
public:
	PointInput(const std::string& name, TravailleurContext* travailleurContext);
	~PointInput();

	const std::string& getName() const;
	void setName(const std::string& name);

	const Point2D<int>& getPoint() const;
	void setPoint(const Point2D<int>& point);

	void showGui();

private:
	void load();
	void save() const;

	std::string getSaveDirectory() const;
	void assureSaveDirectoryExists();

	std::string getPointBinarySaveFilepath() const;

	TravailleurContext* travailleurContext = nullptr;

	std::string name;
	Point2D<int> point;

	std::unique_ptr<ImGuiIntSlider> xSlider;
	std::unique_ptr<ImGuiIntSlider> ySlider;
};

#endif