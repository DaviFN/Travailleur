#ifndef HotkeyHandler_H
#define HotkeyHandler_H

#include <map>
#include <set>

#include "KeyboardAndMouseUtils.h"

class HotkeyHandler {
public:
	void updateKeysStates();

	bool hotkeyPressed(const KeyboardAndMouseDefs::Key key, const bool onlyConsiderPressedIfPreviouslyUnpressed = true) const;

	bool hotkeyPressed(const std::set<KeyboardAndMouseDefs::Key>& hotkeyKeys, const bool onlyConsiderPressedIfPreviouslyUnpressed = true) const;

private:
	KeyboardAndMouseDefs::KeyState getKeyState(const KeyboardAndMouseDefs::Key key) const;

	KeyboardAndMouseDefs::KeyState getPreviousKeyState(const KeyboardAndMouseDefs::Key key) const;

	bool hotkeyWasPreviouslyPressed(const std::set<KeyboardAndMouseDefs::Key>& hotkeyKeys) const;

	std::map<KeyboardAndMouseDefs::Key, KeyboardAndMouseDefs::KeyState> keyToState;

	std::map<KeyboardAndMouseDefs::Key, KeyboardAndMouseDefs::KeyState> previousKeyToState;
};

#endif