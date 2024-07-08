#include "HotkeyHandler.h"

#include <Windows.h>

void HotkeyHandler::updateKeysStates()
{
	previousKeyToState = keyToState;

	for (const auto& key : KeyboardAndMouseUtils::getKeys()) {
		keyToState[key] = KeyboardAndMouseUtils::keyIsPressed(key) ? KeyboardAndMouseDefs::KeyState::Pressed : KeyboardAndMouseDefs::KeyState::Unpressed;
	}
}

KeyboardAndMouseDefs::KeyState HotkeyHandler::getKeyState(const KeyboardAndMouseDefs::Key key) const
{
	auto it = keyToState.find(key);
	if (it != keyToState.end()) {
		return it->second;
	}
	return KeyboardAndMouseDefs::KeyState::Undefined;
}

KeyboardAndMouseDefs::KeyState HotkeyHandler::getPreviousKeyState(const KeyboardAndMouseDefs::Key key) const
{
	auto it = previousKeyToState.find(key);
	if (it != previousKeyToState.end()) {
		return it->second;
	}
	return KeyboardAndMouseDefs::KeyState::Undefined;
}

bool HotkeyHandler::hotkeyPressed(const KeyboardAndMouseDefs::Key key, const bool onlyConsiderPressedIfPreviouslyUnpressed) const
{
	std::set<KeyboardAndMouseDefs::Key> hotkey;
	hotkey.insert(key);
	return hotkeyPressed(hotkey, onlyConsiderPressedIfPreviouslyUnpressed);
}

bool HotkeyHandler::hotkeyPressed(const std::set<KeyboardAndMouseDefs::Key>& hotkeyKeys, const bool onlyConsiderPressedIfPreviouslyUnpressed) const
{
	if (onlyConsiderPressedIfPreviouslyUnpressed && hotkeyWasPreviouslyPressed(hotkeyKeys)) {
		return false;
	}
	for (const KeyboardAndMouseDefs::Key& key : hotkeyKeys) {
		const KeyboardAndMouseDefs::KeyState keyState = getKeyState(key);
		if (keyState != KeyboardAndMouseDefs::KeyState::Pressed) {
			return false;
		}
	}
	return true;
}

bool HotkeyHandler::hotkeyWasPreviouslyPressed(const std::set<KeyboardAndMouseDefs::Key>& hotkeyKeys) const
{
	for (const KeyboardAndMouseDefs::Key& key : hotkeyKeys) {
		const KeyboardAndMouseDefs::KeyState previousKeyState = getPreviousKeyState(key);
		if (previousKeyState != KeyboardAndMouseDefs::KeyState::Pressed) {
			return false;
		}
	}
	return true;
}