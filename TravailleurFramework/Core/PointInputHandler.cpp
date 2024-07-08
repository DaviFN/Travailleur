#include "PointInputHandler.h"

#include "Common/TravailleurUtils.h"
#include "TravailleurContext.h"

PointInputHandler::PointInputHandler(TravailleurContext* travailleurContext):
	travailleurContext(travailleurContext)
{
	allowFrameOverlays = std::make_unique<ImGuiCheckbox>("Allow frame overlays", true, TravailleurUtils::getConfigFilenameFor("allow-point-input-handler-show-overlays", travailleurContext->getName()));

	pointInputNames = std::make_unique<ImGuiSingleSelectionList>("point-input-names");
}

PointInputHandler::~PointInputHandler()
{
	saveSelectedPointInput();

	for (auto& mapItem : mapFromNameToPointInput) {
		delete mapItem.second;
	}
}

void PointInputHandler::saveSelectedPointInput() const
{
	PointInput* selectedPointInput = getSelectedPointInput();
	if (selectedPointInput != nullptr) {
		const std::string selectedPointInputName = selectedPointInput->getName();
		SimplePersistenceUtils::saveString("selected-point-input-name", selectedPointInputName, TravailleurUtils::getConfigPath(travailleurContext->getName()));
	}
}

void PointInputHandler::updateSelectedPointInputBasedOnSave()
{
	std::string selectedPointInputName;
	SimplePersistenceUtils::loadString("selected-point-input-name", selectedPointInputName, TravailleurUtils::getConfigPath(travailleurContext->getName()));
	PointInput* savedSelectedPointInput = getPointInput(selectedPointInputName);
	if (savedSelectedPointInput != nullptr) {
		setSelectedPointInput(savedSelectedPointInput);
	}
}

void PointInputHandler::showFrameOverlays(Image* frame, Image* frameOverlays)
{
	if (!allowFrameOverlays->getValue()) {
		return;
	}

	PointInput* selectedPointInput = getSelectedPointInput();
	if (selectedPointInput != nullptr) {
		const Point2D<int>& point = selectedPointInput->getPoint();
		highlightPointInFrameOverlay(frame, frameOverlays, point);
	}
}

void PointInputHandler::highlightPointInFrameOverlay(Image* frame, Image* frameOverlays, const Point2D<int>& point)
{
	const Pixel highlightColor(230, 203, 55);
	frameOverlays->drawCross(point, 1024, 1, highlightColor, true);
}

void PointInputHandler::showGui()
{
	ImGui::Begin("PointInputHandler");

	allowFrameOverlays->showGui();

	pointInputNames->showGui();
	PointInput* selectedPointInput = getSelectedPointInput();
	if (selectedPointInput != nullptr)
	{
		selectedPointInput->showGui();
	}
	else {
		ImGuiTextOutput("no selected point input");
	}

	const std::string& screenResolutionString = KeyboardAndMouseUtils::getScreenResolutionAsString();
	ImGuiTextOutput("Screen resolution: " + screenResolutionString);

	const std::string& virtualScreenResolutionString = KeyboardAndMouseUtils::getVirtualScreenResolutionAsString();
	ImGuiTextOutput("Virtual screen resolution: " + virtualScreenResolutionString);

	const Point2D<int> currentMousePosition = KeyboardAndMouseUtils::getCurrentMousePosition();
	const std::string& currentMousePositionInformationText = std::string("Current mouse position: (") + std::to_string(currentMousePosition.x) + "," + std::to_string(currentMousePosition.y) + ")";
	ImGuiTextOutput(currentMousePositionInformationText);

	if (selectedPointInput != nullptr) {
		if (ImGui::Button("Set to current mouse position (CTRL + SHIFT + Y)") || setToCurrentMousePositionHotkeyPressed()) {
			selectedPointInput->setPoint(currentMousePosition);
		}

		if (ImGui::Button("Move mouse to point (CTRL + SHIFT + U)") || moveMouseToPointHotkeyPressed()) {
			KeyboardAndMouseUtils::moveMouse(selectedPointInput->getPoint());
		}
	}

	ImGui::End();
}

bool PointInputHandler::setToCurrentMousePositionHotkeyPressed() const
{
	std::set<KeyboardAndMouseDefs::Key> hotkey;
	hotkey.insert(KeyboardAndMouseDefs::Key::Ctrl);
	hotkey.insert(KeyboardAndMouseDefs::Key::Shift);
	hotkey.insert(KeyboardAndMouseDefs::Key::Y);

	return travailleurContext->getHotkeyHandler()->hotkeyPressed(hotkey);
}

bool PointInputHandler::moveMouseToPointHotkeyPressed() const
{
	std::set<KeyboardAndMouseDefs::Key> hotkey;
	hotkey.insert(KeyboardAndMouseDefs::Key::Ctrl);
	hotkey.insert(KeyboardAndMouseDefs::Key::Shift);
	hotkey.insert(KeyboardAndMouseDefs::Key::U);

	return travailleurContext->getHotkeyHandler()->hotkeyPressed(hotkey);
}

PointInput* PointInputHandler::getPointInput(const std::string& name) const
{
	auto it = mapFromNameToPointInput.find(name);
	if (it == mapFromNameToPointInput.end()) {
		return nullptr;
	}
	return it->second;
}

PointInput* PointInputHandler::getSelectedPointInput() const
{
	if (!pointInputNames->hasSelectedOption()) {
		return nullptr;
	}
	return getPointInput(pointInputNames->getSelectedOption());
}

void PointInputHandler::setSelectedPointInput(PointInput* pointInput)
{
	if (pointInput == nullptr) return;
	const std::string pointInputName = pointInput->getName();
	pointInputNames->setSelectedOption(pointInputName);
}

void PointInputHandler::registerPointInput(const std::string& name)
{
	assert(getPointInput(name) == nullptr);
	assert(mapFromNameToPointInput.find(name) == mapFromNameToPointInput.end());

	auto pointInput = new PointInput(name, travailleurContext);
	mapFromNameToPointInput[name] = pointInput;

	updatePointInputNames();
	updateSelectedPointInputBasedOnSave();
}

void PointInputHandler::updatePointInputNames()
{
	std::vector<std::string> names;
	for (auto& mapItem : mapFromNameToPointInput) {
		names.push_back(mapItem.first);
	}
	pointInputNames->setOptions(names);
}