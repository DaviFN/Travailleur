#include "KeyboardHandler.h"

KeyboardHandler::KeyboardHandler()
{
    for(int i = 0 ; i < 256 ; i++)
    {
        keyStates[i] = KEY_STATE_UNPRESSED;
    }
}