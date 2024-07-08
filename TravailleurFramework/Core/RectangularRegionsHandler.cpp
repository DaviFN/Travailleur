#include "RectangularRegionsHandler.h"

#include "assert.h"
#include "imgui.h"

#include "Common/TravailleurUtils.h"
#include "ImGuiWrappers/ImGuiTextOutput.h"
#include "TravailleurContext.h"

RectangularRegionsHandler::RectangularRegionsHandler(TravailleurContext* travailleurContext):
	travailleurContext(travailleurContext)
{
	allowFrameOverlays = std::make_unique<ImGuiCheckbox>("Allow frame overlays", true, TravailleurUtils::getConfigFilenameFor("allow-rectangular-regions-handler-show-overlays", travailleurContext->getName()));

	showSelectedRectangularRegion = std::make_unique<ImGuiCheckbox>("Show selected rectangular region", false, TravailleurUtils::getConfigFilenameFor("show-selected-rectangular-region", travailleurContext->getName()));
	showParentsOfSelectedRectangularRegion = std::make_unique<ImGuiCheckbox>("Show parents of selected rectangular region", false, TravailleurUtils::getConfigFilenameFor("show-parents-of-selected-rectangular-region", travailleurContext->getName()));
	rectangularRegionsNames = std::make_unique<ImGuiSingleSelectionList>("rectangular-regions-names");
}

RectangularRegionsHandler::~RectangularRegionsHandler()
{
	saveSelectedRectangularRegion();

	for (auto& mapItem : mapFromNameToRectangularRegion) {
		delete mapItem.second;
	}
}

void RectangularRegionsHandler::saveSelectedRectangularRegion() const
{
	RectangularRegion* selectedRectangularRegion = getSelectedRectangularRegion();
	if (selectedRectangularRegion != nullptr) {
		const std::string selectedRectangularRegionName = selectedRectangularRegion->getName();
		SimplePersistenceUtils::saveString("selected-rectangular-region-name", selectedRectangularRegionName, TravailleurUtils::getConfigPath(travailleurContext->getName()));
	}
}

void RectangularRegionsHandler::updateSelectedRectangularRegionBasedOnSave()
{
	std::string selectedRectangularRegionName;
	SimplePersistenceUtils::loadString("selected-rectangular-region-name", selectedRectangularRegionName, TravailleurUtils::getConfigPath(travailleurContext->getName()));
	RectangularRegion* savedSelectedRectangularRegion = getRectangularRegion(selectedRectangularRegionName);
	if (savedSelectedRectangularRegion != nullptr) {
		setSelectedRectangularRegion(savedSelectedRectangularRegion);
	}
}

std::string RectangularRegionsHandler::getRectangularRegionsFolder() const
{
	return TravailleurUtils::getRectangularRegionsPath(travailleurContext->getName());
}

std::string RectangularRegionsHandler::filepathOfRectangularRegion(const std::string& name) const
{
	return getRectangularRegionsFolder() + "\\" + name;
}

void RectangularRegionsHandler::updateRectangularRegionsNames()
{
	std::vector<std::string> names;
	for (auto& mapItem : mapFromNameToRectangularRegion) {
		names.push_back(mapItem.first);
	}
	rectangularRegionsNames->setOptions(names);
}

void RectangularRegionsHandler::updateOtherRectangularRegionsForEverySpecificRectangularRegion() const
{
	std::vector<std::string> allRectangularRegionsNames;
	for (auto& mapItem : mapFromNameToRectangularRegion) {
		const std::string& rectangularRegionName = mapItem.first;
		allRectangularRegionsNames.push_back(rectangularRegionName);
	}

	for (auto& mapItem : mapFromNameToRectangularRegion) {
		const std::string& rectangularRegionName = mapItem.first;
		RectangularRegion* rectangularRegion = mapItem.second;
		std::vector<std::string> otherRectangularRegionsNames = allRectangularRegionsNames;

		auto it = std::find(otherRectangularRegionsNames.begin(), otherRectangularRegionsNames.end(), rectangularRegionName);
		if (it != otherRectangularRegionsNames.end()) {
			otherRectangularRegionsNames.erase(it);
		}

		rectangularRegion->informNamesOfOtherRectangularRegions(otherRectangularRegionsNames);
	}
}

void RectangularRegionsHandler::registerRectangularRegion(const std::string& name)
{
	assert(getRectangularRegion(name) == nullptr);
	assert(mapFromNameToRectangularRegion.find(name) == mapFromNameToRectangularRegion.end());

	auto rectangularRegion = new RectangularRegion(name, travailleurContext);
	mapFromNameToRectangularRegion[name] = rectangularRegion;

	updateRectangularRegionsNames();
	updateSelectedRectangularRegionBasedOnSave();
	updateOtherRectangularRegionsForEverySpecificRectangularRegion();
}

bool RectangularRegionsHandler::shouldShowSelectedRectangularRegion() const
{
	return showSelectedRectangularRegion->getValue();
}

bool RectangularRegionsHandler::shouldShowParentsOfSelectedRectangularRegion() const
{
	return showParentsOfSelectedRectangularRegion->getValue();
}

RectangularRegion* RectangularRegionsHandler::getRectangularRegion(const std::string& name) const
{
	auto it = mapFromNameToRectangularRegion.find(name);
	if (it == mapFromNameToRectangularRegion.end()) {
		return nullptr;
	}
	return it->second;
}

RectangularRegion* RectangularRegionsHandler::getSelectedRectangularRegion() const
{
	if (!rectangularRegionsNames->hasSelectedOption()) {
		return nullptr;
	}
	return getRectangularRegion(rectangularRegionsNames->getSelectedOption());
}

void RectangularRegionsHandler::setSelectedRectangularRegion(RectangularRegion* rectangularRegion)
{
	if (rectangularRegion == nullptr) return;
	const std::string rectangularRegionName = rectangularRegion->getName();
	rectangularRegionsNames->setSelectedOption(rectangularRegionName);
}

void RectangularRegionsHandler::showGui()
{
	ImGui::Begin("RectangularRegionsHandler");

	allowFrameOverlays->showGui();

	showSelectedRectangularRegion->showGui();
	showParentsOfSelectedRectangularRegion->showGui();

	rectangularRegionsNames->showGui();
	RectangularRegion* selectedRectangularRegion = getSelectedRectangularRegion();
	if(selectedRectangularRegion != nullptr)
	{
		selectedRectangularRegion->showGui();
	}
	else {
		ImGuiTextOutput("no selected rectangular region");
	}

	if (!rectangularRegionIsInValidState(selectedRectangularRegion)) {
		ImGuiTextOutput("warning: selected rectangular region is in an invalid state");
	}

	ImGui::End();
}

void RectangularRegionsHandler::showFrameOverlays(Image* frame, Image* frameOverlays)
{
	if (!allowFrameOverlays->getValue()) {
		return;
	}
	
	if (!shouldShowSelectedRectangularRegion() && !shouldShowParentsOfSelectedRectangularRegion()) {
		return;
	}

	RectangularRegion* selectedRectangularRegion = getSelectedRectangularRegion();
	if (selectedRectangularRegion != nullptr) {
		const bool inValidState = rectangularRegionIsInValidState(selectedRectangularRegion);
		if (inValidState) {
			if (shouldShowSelectedRectangularRegion()) {
				showSelectedRectangularRegionOverlay(frame, frameOverlays);
			}
			if (shouldShowParentsOfSelectedRectangularRegion()) {
				showParentRectangularRegionsOverlays(frame, frameOverlays);
			}
		}
	}
}

void RectangularRegionsHandler::showRectangularRegionOverlay(Image* frame, Image* frameOverlays, RectangularRegion* rectangularRegion, const Pixel& color)
{
	if (rectangularRegion != nullptr) {
		const Point2D<int> topLeft = determineTopLeftConsideringParents(frame, rectangularRegion);
		frameOverlays->highlightRectangularRegion(topLeft, rectangularRegion->getSize(), 2, color);
	}
}

void RectangularRegionsHandler::showSelectedRectangularRegionOverlay(Image* frame, Image* frameOverlays)
{
	RectangularRegion* selectedRectangularRegion = getSelectedRectangularRegion();
	if (selectedRectangularRegion != nullptr) {
		const Pixel color = selectedRectangularRegion->isRelativeToParent() ? Pixel(230, 203, 55) : Pixel(255, 0, 0);
		showRectangularRegionOverlay(frame, frameOverlays, selectedRectangularRegion, color);
	}
}

void RectangularRegionsHandler::showParentRectangularRegionsOverlays(Image* frame, Image* frameOverlays)
{
	RectangularRegion* selectedRectangularRegion = getSelectedRectangularRegion();
	if (selectedRectangularRegion != nullptr) {
		if (selectedRectangularRegion->isRelativeToParent()) {
			const Pixel color = Pixel(135, 18, 3);
			RectangularRegion* currentRectangularRegion = selectedRectangularRegion;
			while (true) {
				currentRectangularRegion = parentRectangularRegionOf(currentRectangularRegion);
				if (currentRectangularRegion == nullptr) {
					break;
				}
				showRectangularRegionOverlay(frame, frameOverlays, currentRectangularRegion, color);
			}
		}
	}
}

bool RectangularRegionsHandler::rectangularRegionIsInValidState(RectangularRegion* rectangularRegion) const
{
	return !parentsOfRectangularRegionFormCycle(rectangularRegion);
}

bool RectangularRegionsHandler::parentsOfRectangularRegionFormCycle(RectangularRegion* rectangularRegion) const
{
	std::set<RectangularRegion*> alreadyVisitedParents;
	RectangularRegion* currentRectangularRegion = rectangularRegion;
	while (true) {
		currentRectangularRegion = parentRectangularRegionOf(currentRectangularRegion);
		if (currentRectangularRegion == nullptr) {
			break;
		}
		if (std::find(alreadyVisitedParents.begin(), alreadyVisitedParents.end(), currentRectangularRegion) != alreadyVisitedParents.end()) {
			return true;
		}
		alreadyVisitedParents.insert(currentRectangularRegion);
	}
	return false;
}

RectangularRegion* RectangularRegionsHandler::parentRectangularRegionOf(RectangularRegion* rectangularRegion) const
{
	if (!rectangularRegion->isRelativeToParent()) {
		return nullptr;
	}

	const std::string parentRectangularRegionName = rectangularRegion->nameOfParent();

	// I don't know why this was not working; used linear search instead
	//const auto it = std::find(mapFromNameToRectangularRegion.begin(), mapFromNameToRectangularRegion.end(), parentRectangularRegionName);
	//if (it != mapFromNameToRectangularRegion.end()) {
	//	RectangularRegion* parent = it->second;
	//	return parent;
	//}

	for (auto& mapItem : mapFromNameToRectangularRegion) {
		if (mapItem.first == parentRectangularRegionName) {
			return mapItem.second;
		}
	}

	return nullptr;
}

Point2D<int> RectangularRegionsHandler::determineTopLeftConsideringParents(Image* frame, RectangularRegion* rectangularRegion) const
{
	if (!parentsOfRectangularRegionFormCycle(rectangularRegion)) {
		if (rectangularRegion->isRelativeToParent()) {
			RectangularRegion* parent = parentRectangularRegionOf(rectangularRegion);
			assert(parent != nullptr);
			Point2D<int> topLeftConsideringParents = determineTopLeftConsideringParents(frame, parent);
			if (topLeftConsideringParents == TravailleurContext::invalidPoint()) return topLeftConsideringParents;
			topLeftConsideringParents += rectangularRegion->getTopLeft();
			if (rectangularRegion->isRelativeToImagePatternDetection()) {
				ImagePattern* imagePattern = travailleurContext->getImagePattern(rectangularRegion->nameOfImagePatternToBeRelativeTo());
				assert(imagePattern != nullptr);
				std::vector<Point2D<int>> occurrences;
				Rectangle2D<int> searchRegion;
				searchRegion.topLeft = determineTopLeftConsideringParents(frame, parent);
				if (topLeftConsideringParents == TravailleurContext::invalidPoint()) return topLeftConsideringParents;
				searchRegion.size = parent->getSize();
				imagePattern->performSearchForFirstOccurrence(occurrences, frame, searchRegion);
				if (occurrences.size() == 0) {
					return TravailleurContext::invalidPoint();
				}
				const Point2D<int> occurrence = occurrences[0];
				topLeftConsideringParents += occurrence;
				topLeftConsideringParents -= searchRegion.getTopLeft();
			}
			return topLeftConsideringParents;
		}
	}
	return rectangularRegion->getTopLeft();
}

bool RectangularRegionsHandler::rectangularRegionIsValidOnFrame(Image* frame, RectangularRegion* rectangularRegion) const
{
	return determineTopLeftConsideringParents(frame, rectangularRegion) != TravailleurContext::invalidPoint();
}

void RectangularRegionsHandler::afterAppSetup()
{
	informImagePatternNamesToEveryRectangularRegion();
}

void RectangularRegionsHandler::informImagePatternNamesToEveryRectangularRegion()
{
	const std::vector<std::string> imagePatternNames = travailleurContext->imagePatternNames();
	for (auto& mapItem : mapFromNameToRectangularRegion) {
		RectangularRegion* rectangularRegion = mapItem.second;
		rectangularRegion->informImagePatternNames(imagePatternNames);
	}
}

std::vector<std::string> RectangularRegionsHandler::getRectangularRegionsNames() const
{
	std::vector<std::string> names;
	for (auto& mapItem : mapFromNameToRectangularRegion) {
		names.push_back(mapItem.first);
	}
	return names;
}