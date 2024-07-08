#ifndef RectangularRegionsHandler_H
#define RectangularRegionsHandler_H

#include <map>
#include <memory>
#include <set>
#include <string>

#include "Common/Image.h"
#include "ImGuiWrappers/ImGuiCheckbox.h"
#include "ImGuiWrappers/ImGuiSingleSelectionList.h"
#include "RectangularRegion.h"

class TravailleurContext;

class RectangularRegionsHandler
{
public:
	RectangularRegionsHandler(TravailleurContext* travailleurContext);
	~RectangularRegionsHandler();

	void registerRectangularRegion(const std::string& name);

	RectangularRegion* getSelectedRectangularRegion() const;

	RectangularRegion* getRectangularRegion(const std::string& name) const;

	void showGui();

	void showFrameOverlays(Image* frame, Image* frameOverlays);

	void afterAppSetup();

	std::vector<std::string> getRectangularRegionsNames() const;

	bool parentsOfRectangularRegionFormCycle(RectangularRegion* rectangularRegion) const;

	Point2D<int> determineTopLeftConsideringParents(Image* frame, RectangularRegion* rectangularRegion) const;

	bool rectangularRegionIsValidOnFrame(Image* frame, RectangularRegion* rectangularRegion) const;

	void showRectangularRegionOverlay(Image* frame, Image* frameOverlays, RectangularRegion* rectangularRegion, const Pixel& pixel);

private:
	void saveSelectedRectangularRegion() const;
	void updateSelectedRectangularRegionBasedOnSave();
	void updateOtherRectangularRegionsForEverySpecificRectangularRegion() const;

	void informImagePatternNamesToEveryRectangularRegion();

	void updateRectangularRegionsNames();

	bool shouldShowSelectedRectangularRegion() const;
	bool shouldShowParentsOfSelectedRectangularRegion() const;

	std::string getRectangularRegionsFolder() const;

	std::string filepathOfRectangularRegion(const std::string& name) const;

	void setSelectedRectangularRegion(RectangularRegion* rectangularRegion);

	void showSelectedRectangularRegionOverlay(Image* frame, Image* frameOverlays);

	void showParentRectangularRegionsOverlays(Image* frame, Image* frameOverlays);

	bool rectangularRegionIsInValidState(RectangularRegion* rectangularRegion) const;

	RectangularRegion* parentRectangularRegionOf(RectangularRegion* rectangularRegion) const;

	std::map<std::string, RectangularRegion*> mapFromNameToRectangularRegion;

	TravailleurContext* travailleurContext = nullptr;

	std::unique_ptr<ImGuiCheckbox> allowFrameOverlays;

	std::unique_ptr<ImGuiCheckbox> showSelectedRectangularRegion;
	std::unique_ptr<ImGuiCheckbox> showParentsOfSelectedRectangularRegion;

	std::unique_ptr<ImGuiSingleSelectionList> rectangularRegionsNames;
};

#endif