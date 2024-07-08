#ifndef KEYBOARD_HANDLER_H
#define KEYBOARD_HANDLER_H

#define KEY_STATE_UNPRESSED 0
#define KEY_STATE_PRESSED_UNHANDLED 1
#define KEY_STATE_PRESSED_HANDLED 2

// this is os-specific; works on Windows OS
#include "windows.h"

class KeyboardHandler
{
    int keyStates[256];
    public:
    KeyboardHandler();
    void update();
};

#endif