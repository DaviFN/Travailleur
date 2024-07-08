#ifndef KeyboardAndMouseUtils_H
#define KeyboardAndMouseUtils_H

#include "KeyboardAndMouseDefs.h"

#include <set>
#include <map>

#include "Common/Point2D.h"

class KeyboardAndMouseUtils {
public:
	static bool keyIsPressed(const KeyboardAndMouseDefs::Key key);
	static void pressKey(const KeyboardAndMouseDefs::Key key, const int pressTimeInMs);
	static Point2D<int> getCurrentMousePosition();
	static void pressLeftMouseButton(const int pressTimeInMs);
	static void pressRightMouseButton(const int pressTimeInMs);
	static void moveMouse(const Point2D<int>& point);
	static void moveMouse(const int x, const int y);
	static const std::set<KeyboardAndMouseDefs::Key>& getKeys();

	static int getScreenWidth();
	static int getScreenHeight();
	static Point2D<int> getScreenResolution();
	static std::string getScreenResolutionAsString();

	static int getVirtualScreenWidth();
	static int getVirtualScreenHeight();
	static Point2D<int> getVirtualScreenResolution();
	static std::string getVirtualScreenResolutionAsString();

private:
	static void initializeIfNotAlready();
	static void assureKeysSetIsInitialized();
	static void assureKeyToWinVkMapIsInitialized();

	static KeyboardAndMouseDefs::KeyState getKeyState(const KeyboardAndMouseDefs::Key key);

	static void holdKey(const KeyboardAndMouseDefs::Key key);
	static void releaseKey(const KeyboardAndMouseDefs::Key key);

	static void holdLeftMouseButton();
	static void releaseLeftMouseButton();
	static void holdRightMouseButton();
	static void releaseRightMouseButton();
	
	static int getVkFromKey(const KeyboardAndMouseDefs::Key key);
	static bool winVKIsPressed(const int vk);

	static std::set<KeyboardAndMouseDefs::Key> keys;
	static std::map<KeyboardAndMouseDefs::Key, int> keyToWinVk;
};

#endif