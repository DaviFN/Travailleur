#ifndef PointInputHandler_H
#define PointInputHandler_H

#include <memory>

#include "Common/Point2D.h"
#include "ImGuiWrappers/ImGuiCheckbox.h"
#include "ImGuiWrappers/ImGuiSingleSelectionList.h"
#include "PointInput.h"

class Image;
class TravailleurContext;

class PointInputHandler
{
public:
	PointInputHandler(TravailleurContext* travailleurContext);
	~PointInputHandler();

	void showFrameOverlays(Image* frame, Image* frameOverlays);

	void showGui();

	void registerPointInput(const std::string& name);

	PointInput* getPointInput(const std::string& name) const;

private:
	void saveSelectedPointInput() const;
	void updateSelectedPointInputBasedOnSave();

	void updatePointInputNames();

	PointInput* getSelectedPointInput() const;

	void setSelectedPointInput(PointInput* pointInput);

	void highlightPointInFrameOverlay(Image* frame, Image* frameOverlays, const Point2D<int>& point);

	bool setToCurrentMousePositionHotkeyPressed() const;

	bool moveMouseToPointHotkeyPressed() const;

	TravailleurContext* travailleurContext = nullptr;

	std::unique_ptr<ImGuiSingleSelectionList> pointInputNames;

	std::map<std::string, PointInput*> mapFromNameToPointInput;

	std::unique_ptr<ImGuiCheckbox> allowFrameOverlays;

	std::unique_ptr<ImGuiCheckbox> applyDesktopPointCorrection;
};

#endif