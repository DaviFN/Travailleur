#ifndef KeyboardAndMouseDefs_H
#define KeyboardAndMouseDefs_H

namespace KeyboardAndMouseDefs {
	enum class Key {
		Ctrl,
		Shift,
		Q,
		W,
		E,
		R,
		Y,
		U
	};

	enum class KeyState {
		Undefined,
		Unpressed,
		Pressed
	};
}

#endif