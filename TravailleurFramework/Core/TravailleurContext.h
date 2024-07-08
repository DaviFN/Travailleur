#ifndef TravailleurContext_H
#define TravailleurContext_H

#include <memory>

#include "CaptureHandler.h"
#include "Common/DelayUtils.h"
#include "Common/HotkeyHandler.h"
#include "Common/Image.h"
#include "Common/Point2D.h"
#include "ImageOCRsHandler.h"
#include "ImagePatternsHandler.h"
#include "ImagePreprocessingsHandler.h"
#include "ImageResourcesHandler.h"
#include "ImGuiWrappers/include_all_headers.h"
#include "InterfaceConfig.h"
#include "MainAppWindow.h"
#include "PointInputHandler.h"
#include "RectangularRegionsHandler.h"
#include "TravailleurAppsHandler.h"
#include "TravailleurApp.h"
#include "TravailleurConsole.h"

class TravailleurContext {
    public:
    TravailleurContext(const std::string& name, const std::vector<TravailleurApp*>& travailleurApps);
    ~TravailleurContext();

    void setCrashIfAccessHotkeyUnpressed(const bool crashIfAccessHotkeyUnpressed);
    bool accessHotkeyPressed() const;
    void startContext();

    std::string getName() const;
    std::string getPath() const;

    void registerPointInput(const std::string& name);
    PointInput* getPointInput(const std::string& name);

    void registerRectangularRegion(const std::string& name);
    RectangularRegion* getRectangularRegion(const std::string& name);
    std::vector<std::string> rectangularRegionsNames() const;

    void registerImagePattern(const std::string& name);
    ImagePattern* getImagePattern(const std::string& name);
    std::vector<std::string> imagePatternNames() const;

    void registerImagePreprocessing(const std::string& name);
    ImagePreprocessing* getImagePreprocessing(const std::string& name);
    std::vector<std::string> imagePreprocessingsNames() const;

    void registerImageOCR(const std::string& name);
    ImageOCR* getImageOCR(const std::string& name);
    std::vector<std::string> imageOCRsNames() const;

    void registerImageResource(const std::string& name);
    ImageResource* getImageResource(const std::string& name);

    InterfaceConfig* getInterfaceConfig() const;
    CaptureHandler* getCaptureHandler() const;
    RectangularRegionsHandler* getRectangularRegionsHandler() const;
    ImagePatternsHandler* getImagePatternsHandler() const;
    ImagePreprocessingsHandler* getImagePreprocessingsHandler() const;
    ImageOCRsHandler* getImageOCRsHandler() const;
    ImageResourcesHandler* getImageResourcesHandler() const;
    TravailleurAppsHandler* getTravailleurAppsHandler() const;
    TravailleurConsole* getTravailleurConsole() const;
    HotkeyHandler* getHotkeyHandler() const;

    static Point2D<int> invalidPoint();

    void pressKey(const KeyboardAndMouseDefs::Key key, const int pressTimeInMs);
    void pressLeftMouseButton(const int pressTimeInMs);
    void pressRightMouseButton(const int pressTimeInMs);
    void moveMouse(const Point2D<int>& point);
    void moveMouse(const std::string& pointInputName);

    private:
    void initialize(const std::vector<TravailleurApp*> travailleurApps);
    void load();
    void deinitialize();
    void save() const;
    void loopCallback();
    void update();
    void showGuis();
    void showMainMenuGui();
    void showFrameworkFramesInMainMenu();

    void applyStyleColors() const;

    std::string name;

    bool shouldShowInterfaceConfigGui = false;
    bool shouldShowCaptureHandlerGui = false;
    bool shouldShowPointInputHandlerGui = false;
    bool shouldShowRectangularRegionsHandlerGui = false;
    bool shouldShowImagePatternsHandlerGui = false;
    bool shouldShowImagePreprocessingsHandlerGui = false;
    bool shouldShowImageOCRsHandlerGui = false;
    bool shouldShowImageResourcesHandlerGui = false;
    bool shouldShowTravailleurAppsHandlerGui = false;
    bool shouldShowMainAppWindowGui = false;
    bool shouldShowTravailleurConsoleGui = false;

    std::unique_ptr<InterfaceConfig> interfaceConfig;

    bool contextInitialized = false;
    std::unique_ptr<CaptureHandler> captureHandler;
    bool isFullScreen = false;

    std::unique_ptr<PointInputHandler> pointInputHandler;
    std::unique_ptr<RectangularRegionsHandler> rectangularRegionsHandler;
    std::unique_ptr<ImagePatternsHandler> imagePatternsHandler;
    std::unique_ptr<ImagePreprocessingsHandler> imagePreprocessingsHandler;
    std::unique_ptr<ImageOCRsHandler> imageOCRsHandler;
    std::unique_ptr<ImageResourcesHandler> imageResourcesHandler;

    std::unique_ptr<TravailleurAppsHandler> travailleurAppsHandler;

    std::unique_ptr<MainAppWindow> mainAppWindow;

    std::unique_ptr<TravailleurConsole> travailleurConsole;

    std::unique_ptr<HotkeyHandler> hotkeyHandler;

    static double currentFontScaleFactor;

    bool crashIfAccessHotkeyUnpressed = false;
};

#endif