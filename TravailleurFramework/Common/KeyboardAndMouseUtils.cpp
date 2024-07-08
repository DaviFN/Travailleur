#include "KeyboardAndMouseUtils.h"

#include <assert.h>
#include <thread>
#include <Windows.h>

#include "Common/DelayUtils.h"

std::map<KeyboardAndMouseDefs::Key, int> KeyboardAndMouseUtils::keyToWinVk;
std::set<KeyboardAndMouseDefs::Key> KeyboardAndMouseUtils::keys;

void KeyboardAndMouseUtils::initializeIfNotAlready()
{
	assureKeyToWinVkMapIsInitialized();
	assureKeysSetIsInitialized();
}

void KeyboardAndMouseUtils::assureKeysSetIsInitialized()
{
	if (keys.empty()) {
		for (const auto& mapItem : keyToWinVk) {
			keys.insert(mapItem.first);
		}
	}
}

void KeyboardAndMouseUtils::assureKeyToWinVkMapIsInitialized()
{
	if (keyToWinVk.empty()) {
		keyToWinVk[KeyboardAndMouseDefs::Key::Ctrl] = VK_CONTROL;
		keyToWinVk[KeyboardAndMouseDefs::Key::Shift] = VK_SHIFT;
		keyToWinVk[KeyboardAndMouseDefs::Key::Q] = 81;
		keyToWinVk[KeyboardAndMouseDefs::Key::W] = 87;
		keyToWinVk[KeyboardAndMouseDefs::Key::E] = 69;
		keyToWinVk[KeyboardAndMouseDefs::Key::R] = 82;
		keyToWinVk[KeyboardAndMouseDefs::Key::Y] = 89;
		keyToWinVk[KeyboardAndMouseDefs::Key::U] = 85;
	}
}

bool KeyboardAndMouseUtils::keyIsPressed(const KeyboardAndMouseDefs::Key key)
{
	return getKeyState(key) == KeyboardAndMouseDefs::KeyState::Pressed;
}

const std::set<KeyboardAndMouseDefs::Key>& KeyboardAndMouseUtils::getKeys()
{
	initializeIfNotAlready();
	return keys;
}

KeyboardAndMouseDefs::KeyState KeyboardAndMouseUtils::getKeyState(const KeyboardAndMouseDefs::Key key)
{
	initializeIfNotAlready();
	return winVKIsPressed(getVkFromKey(key)) ? KeyboardAndMouseDefs::KeyState::Pressed : KeyboardAndMouseDefs::KeyState::Unpressed;
}

void KeyboardAndMouseUtils::pressKey(const KeyboardAndMouseDefs::Key key, const int pressTimeInMs)
{
	holdKey(key);
	DelayUtils::performDelayInMs(pressTimeInMs);
	releaseKey(key);
}

Point2D<int> KeyboardAndMouseUtils::getCurrentMousePosition()
{
	POINT point;
	if (GetCursorPos(&point)) {
		return Point2D<int>(point.x, point.y);
	}
	return Point2D<int>(0, 0);
}

void KeyboardAndMouseUtils::pressLeftMouseButton(const int pressTimeInMs)
{
	holdLeftMouseButton();
	DelayUtils::performDelayInMs(pressTimeInMs);
	releaseLeftMouseButton();
}

void KeyboardAndMouseUtils::pressRightMouseButton(const int pressTimeInMs)
{
	holdRightMouseButton();
	DelayUtils::performDelayInMs(pressTimeInMs);
	releaseRightMouseButton();
}

void KeyboardAndMouseUtils::moveMouse(const Point2D<int>& point)
{
	moveMouse(point.x, point.y);
}

void KeyboardAndMouseUtils::moveMouse(const int x, const int y)
{
	INPUT input;
	input.type = INPUT_MOUSE;
	input.mi.mouseData = 0;
	input.mi.dx = MulDiv(x, 65536, getScreenWidth());
	input.mi.dy = MulDiv(y, 65536, getScreenHeight());
	input.mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
	SendInput(1, &input, sizeof(input));
}

void KeyboardAndMouseUtils::holdKey(const KeyboardAndMouseDefs::Key key)
{
	INPUT input;
	input.type = INPUT_KEYBOARD;
	input.ki.wVk = getVkFromKey(key);
	input.ki.wScan = MapVirtualKeyA(input.ki.wVk, MAPVK_VK_TO_VSC);
	input.ki.dwFlags = KEYEVENTF_SCANCODE;

	SendInput(1, &input, sizeof(INPUT));
}

void KeyboardAndMouseUtils::releaseKey(const KeyboardAndMouseDefs::Key key)
{
	INPUT input;
	input.type = INPUT_KEYBOARD;
	input.ki.wVk = getVkFromKey(key);
	input.ki.wScan = MapVirtualKeyA(input.ki.wVk, MAPVK_VK_TO_VSC);
	input.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;

	SendInput(1, &input, sizeof(INPUT));
}

void KeyboardAndMouseUtils::holdLeftMouseButton()
{
	INPUT input;
	input.type = INPUT_MOUSE;
	input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

	SendInput(1, &input, sizeof(INPUT));
}

void KeyboardAndMouseUtils::releaseLeftMouseButton()
{
	INPUT input;
	input.type = INPUT_MOUSE;
	input.mi.dwFlags = MOUSEEVENTF_LEFTUP;

	SendInput(1, &input, sizeof(INPUT));
}

void KeyboardAndMouseUtils::holdRightMouseButton()
{
	INPUT input;
	input.type = INPUT_MOUSE;
	input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;

	SendInput(1, &input, sizeof(INPUT));
}

void KeyboardAndMouseUtils::releaseRightMouseButton()
{
	INPUT input;
	input.type = INPUT_MOUSE;
	input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;

	SendInput(1, &input, sizeof(INPUT));
}

int KeyboardAndMouseUtils::getVkFromKey(const KeyboardAndMouseDefs::Key key)
{
	auto it = keyToWinVk.find(key);
	if (it != keyToWinVk.end()) {
		const int vk = it->second;
		return vk;
	}
	// there should be a corresponding key in the map; let's crash to warn there is not
	assert(false);
	return 0;
}

bool KeyboardAndMouseUtils::winVKIsPressed(const int vk)
{
	return GetAsyncKeyState(vk) & 0x8000;
}

int KeyboardAndMouseUtils::getScreenWidth()
{
	return GetSystemMetrics(SM_CXSCREEN);
}

int KeyboardAndMouseUtils::getScreenHeight()
{
	return GetSystemMetrics(SM_CYSCREEN);
}

Point2D<int> KeyboardAndMouseUtils::getScreenResolution()
{
	return Point2D<int>(getScreenWidth(), getScreenHeight());
}

std::string KeyboardAndMouseUtils::getScreenResolutionAsString()
{
	return std::to_string(getScreenWidth()) + "x" + std::to_string(getScreenHeight());
}

int KeyboardAndMouseUtils::getVirtualScreenWidth()
{
	return GetSystemMetrics(SM_CXVIRTUALSCREEN);
}

int KeyboardAndMouseUtils::getVirtualScreenHeight()
{
	return GetSystemMetrics(SM_CYVIRTUALSCREEN);
}

Point2D<int> KeyboardAndMouseUtils::getVirtualScreenResolution()
{
	return Point2D<int>(getVirtualScreenWidth(), getVirtualScreenHeight());
}

std::string KeyboardAndMouseUtils::getVirtualScreenResolutionAsString()
{
	return std::to_string(getVirtualScreenWidth()) + "x" + std::to_string(getVirtualScreenHeight());
}