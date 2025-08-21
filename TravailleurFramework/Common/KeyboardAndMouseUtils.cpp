#include "KeyboardAndMouseUtils.h"
#include <cassert>
#include <thread>
#include <SDL3/SDL.h>
#include "Common/DelayUtils.h"

#ifdef _WIN32
#include <Windows.h>
#elif defined(__linux__)
#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#endif

std::map<KeyboardAndMouseDefs::Key, int> KeyboardAndMouseUtils::keyToNative;
std::set<KeyboardAndMouseDefs::Key> KeyboardAndMouseUtils::keys;

void KeyboardAndMouseUtils::initializeIfNotAlready()
{
    assureKeyToNativeMapIsInitialized();
    assureKeysSetIsInitialized();
}

void KeyboardAndMouseUtils::assureKeysSetIsInitialized()
{
    if (keys.empty()) {
        for (const auto& mapItem : keyToNative) {
            keys.insert(mapItem.first);
        }
    }
}

void KeyboardAndMouseUtils::assureKeyToNativeMapIsInitialized()
{
    if (keyToNative.empty()) {
#ifdef _WIN32
        keyToNative[KeyboardAndMouseDefs::Key::Ctrl] = VK_CONTROL;
        keyToNative[KeyboardAndMouseDefs::Key::Shift] = VK_SHIFT;
        keyToNative[KeyboardAndMouseDefs::Key::Q] = 0x51;
        keyToNative[KeyboardAndMouseDefs::Key::W] = 0x57;
        keyToNative[KeyboardAndMouseDefs::Key::E] = 0x45;
        keyToNative[KeyboardAndMouseDefs::Key::R] = 0x52;
        keyToNative[KeyboardAndMouseDefs::Key::Y] = 0x59;
        keyToNative[KeyboardAndMouseDefs::Key::U] = 0x55;
#elif defined(__linux__)
        keyToNative[KeyboardAndMouseDefs::Key::Ctrl] = 37;
        keyToNative[KeyboardAndMouseDefs::Key::Shift] = 50;
        keyToNative[KeyboardAndMouseDefs::Key::Q] = 24;
        keyToNative[KeyboardAndMouseDefs::Key::W] = 25;
        keyToNative[KeyboardAndMouseDefs::Key::E] = 26;
        keyToNative[KeyboardAndMouseDefs::Key::R] = 27;
        keyToNative[KeyboardAndMouseDefs::Key::Y] = 29;
        keyToNative[KeyboardAndMouseDefs::Key::U] = 30;
#endif
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
    const bool* state = SDL_GetKeyboardState(nullptr);
    SDL_Scancode sc;
    switch (key) {
        case KeyboardAndMouseDefs::Key::Q: sc = SDL_SCANCODE_Q; break;
        case KeyboardAndMouseDefs::Key::W: sc = SDL_SCANCODE_W; break;
        case KeyboardAndMouseDefs::Key::E: sc = SDL_SCANCODE_E; break;
        case KeyboardAndMouseDefs::Key::R: sc = SDL_SCANCODE_R; break;
        case KeyboardAndMouseDefs::Key::Y: sc = SDL_SCANCODE_Y; break;
        case KeyboardAndMouseDefs::Key::U: sc = SDL_SCANCODE_U; break;
        case KeyboardAndMouseDefs::Key::Ctrl: sc = SDL_SCANCODE_LCTRL; break;
        case KeyboardAndMouseDefs::Key::Shift: sc = SDL_SCANCODE_LSHIFT; break;
        default: return KeyboardAndMouseDefs::KeyState::Undefined;
    }
    return state[sc] ? KeyboardAndMouseDefs::KeyState::Pressed : KeyboardAndMouseDefs::KeyState::Unpressed;
}

void KeyboardAndMouseUtils::pressKey(const KeyboardAndMouseDefs::Key key, const int pressTimeInMs)
{
    holdKey(key);
    DelayUtils::performDelayInMs(pressTimeInMs);
    releaseKey(key);
}

Point2D<int> KeyboardAndMouseUtils::getCurrentMousePosition()
{
#ifdef _WIN32
    POINT point;
    if (GetCursorPos(&point)) {
        return Point2D<int>(point.x, point.y);
    }
    return Point2D<int>(0, 0);
#elif defined(__linux__)
    Display* d = XOpenDisplay(nullptr);
    if (!d) return Point2D<int>(0, 0);
    Window root = DefaultRootWindow(d);
    Window ret_root, ret_child;
    int root_x, root_y, win_x, win_y;
    unsigned int mask;
    XQueryPointer(d, root, &ret_root, &ret_child, &root_x, &root_y, &win_x, &win_y, &mask);
    XCloseDisplay(d);
    return Point2D<int>(root_x, root_y);
#else
    return Point2D<int>(0, 0);
#endif
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
#ifdef _WIN32
    SetCursorPos(x, y);
#elif defined(__linux__)
    Display* d = XOpenDisplay(nullptr);
    if (!d) return;
    Window root = DefaultRootWindow(d);
    XWarpPointer(d, None, root, 0, 0, 0, 0, x, y);
    XFlush(d);
    XCloseDisplay(d);
#endif
}

void KeyboardAndMouseUtils::holdKey(const KeyboardAndMouseDefs::Key key)
{
#ifdef _WIN32
    INPUT input = {0};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = getNativeFromKey(key);
    SendInput(1, &input, sizeof(INPUT));
#elif defined(__linux__)
    Display* d = XOpenDisplay(nullptr);
    if (!d) return;
    XTestFakeKeyEvent(d, getNativeFromKey(key), True, 0);
    XFlush(d);
    XCloseDisplay(d);
#endif
}

void KeyboardAndMouseUtils::releaseKey(const KeyboardAndMouseDefs::Key key)
{
#ifdef _WIN32
    INPUT input = {0};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = getNativeFromKey(key);
    input.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &input, sizeof(INPUT));
#elif defined(__linux__)
    Display* d = XOpenDisplay(nullptr);
    if (!d) return;
    XTestFakeKeyEvent(d, getNativeFromKey(key), False, 0);
    XFlush(d);
    XCloseDisplay(d);
#endif
}

void KeyboardAndMouseUtils::holdLeftMouseButton()
{
#ifdef _WIN32
    INPUT input = {0};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    SendInput(1, &input, sizeof(INPUT));
#elif defined(__linux__)
    Display* d = XOpenDisplay(nullptr);
    if (!d) return;
    XTestFakeButtonEvent(d, 1, True, 0);
    XFlush(d);
    XCloseDisplay(d);
#endif
}

void KeyboardAndMouseUtils::releaseLeftMouseButton()
{
#ifdef _WIN32
    INPUT input = {0};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(1, &input, sizeof(INPUT));
#elif defined(__linux__)
    Display* d = XOpenDisplay(nullptr);
    if (!d) return;
    XTestFakeButtonEvent(d, 1, False, 0);
    XFlush(d);
    XCloseDisplay(d);
#endif
}

void KeyboardAndMouseUtils::holdRightMouseButton()
{
#ifdef _WIN32
    INPUT input = {0};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
    SendInput(1, &input, sizeof(INPUT));
#elif defined(__linux__)
    Display* d = XOpenDisplay(nullptr);
    if (!d) return;
    XTestFakeButtonEvent(d, 3, True, 0);
    XFlush(d);
    XCloseDisplay(d);
#endif
}

void KeyboardAndMouseUtils::releaseRightMouseButton()
{
#ifdef _WIN32
    INPUT input = {0};
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
    SendInput(1, &input, sizeof(INPUT));
#elif defined(__linux__)
    Display* d = XOpenDisplay(nullptr);
    if (!d) return;
    XTestFakeButtonEvent(d, 3, False, 0);
    XFlush(d);
    XCloseDisplay(d);
#endif
}

int KeyboardAndMouseUtils::getNativeFromKey(const KeyboardAndMouseDefs::Key key)
{
    auto it = keyToNative.find(key);
    if (it != keyToNative.end()) {
        return it->second;
    }
    assert(false);
    return 0;
}

int KeyboardAndMouseUtils::getScreenWidth()
{
#ifdef _WIN32
    return GetSystemMetrics(SM_CXSCREEN);
#elif defined(__linux__)
    Display* d = XOpenDisplay(nullptr);
    if (!d) return 0;
    Screen*  s = DefaultScreenOfDisplay(d);
    int w = s->width;
    XCloseDisplay(d);
    return w;
#else
    return 0;
#endif
}

int KeyboardAndMouseUtils::getScreenHeight()
{
#ifdef _WIN32
    return GetSystemMetrics(SM_CYSCREEN);
#elif defined(__linux__)
    Display* d = XOpenDisplay(nullptr);
    if (!d) return 0;
    Screen*  s = DefaultScreenOfDisplay(d);
    int h = s->height;
    XCloseDisplay(d);
    return h;
#else
    return 0;
#endif
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
#ifdef _WIN32
    return GetSystemMetrics(SM_CXVIRTUALSCREEN);
#elif defined(__linux__)
    return getScreenWidth();
#else
    return 0;
#endif
}

int KeyboardAndMouseUtils::getVirtualScreenHeight()
{
#ifdef _WIN32
    return GetSystemMetrics(SM_CYVIRTUALSCREEN);
#elif defined(__linux__)
    return getScreenHeight();
#else
    return 0;
#endif
}

Point2D<int> KeyboardAndMouseUtils::getVirtualScreenResolution()
{
    return Point2D<int>(getVirtualScreenWidth(), getVirtualScreenHeight());
}

std::string KeyboardAndMouseUtils::getVirtualScreenResolutionAsString()
{
    return std::to_string(getVirtualScreenWidth()) + "x" + std::to_string(getVirtualScreenHeight());
}