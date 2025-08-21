/*

2024 - Davi Feliciano Nonnenmacher

Travailler is a desktop automation framework that provides functionalities such as image pattern recognition, OCR (optical character recognition), image preprocessing with various abstractions from OpenCV library, video input from media source (e.g. Webcam), video input from the desktop itself, mouse outputs and a few keyboard outputs. It is intended to be easily configurable via graphical user interface, allowing to quickly automate a sequence of actions in the desktop, possibly based on visual input from the screen or any other video media source.

Its name comes from the French "worker", which is a somewhat good description of the intention of the project (automate a task on the desktop, and then leave it to be done by the "worker").

*/

#include "TravailleurContext.h"

#ifdef VLD_BUILD
#define VLD_FORCE_ENABLE
#include <vld.h>
#endif

#include "gl3w.h"

#define SDL_MAIN_HANDLED

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <SDL3/SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#else
#include <GL/gl.h>
#endif

#include "assert.h"

#include "Common/TravailleurUtils.h"
#include "ImageOCRUtils.h"

double TravailleurContext::currentFontScaleFactor = 0.0;

// Main code
TravailleurContext::TravailleurContext(const std::string& name, const std::vector<TravailleurApp*>& travailleurApps) :
    name(name)
{
    initialize(travailleurApps);
}

TravailleurContext::~TravailleurContext()
{
    deinitialize();
}

void TravailleurContext::setCrashIfAccessHotkeyUnpressed(const bool crashIfAccessHotkeyUnpressed)
{
    this->crashIfAccessHotkeyUnpressed = crashIfAccessHotkeyUnpressed;
}

bool TravailleurContext::accessHotkeyPressed() const
{
    std::set<KeyboardAndMouseDefs::Key> hotkey;
    hotkey.insert(KeyboardAndMouseDefs::Key::Ctrl);
    hotkey.insert(KeyboardAndMouseDefs::Key::Shift);
    hotkey.insert(KeyboardAndMouseDefs::Key::R);

    return getHotkeyHandler()->hotkeyPressed(hotkey);
}

void TravailleurContext::startContext()
{
    if (crashIfAccessHotkeyUnpressed && !accessHotkeyPressed()) {
         // crash
        int* nullPointer = 0;
        *nullPointer = 0;
    }

    printf("initializing travailleur context... please, wait...\n");

#ifdef _WIN32
    // This function is only available on Windows to handle DPI awareness
    // It should be called before any UI elements or windows are created.
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);
#endif

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        printf("Error: %s\n", SDL_GetError());
        return;
    }

    printf("SDL initiated\n");

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
    // GL 3.2 Core + GLSL 150
    const char* glsl_version = "#version 150";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_MAXIMIZED);
    SDL_Window* window = SDL_CreateWindow("Travailleur (SDL3+OpenGL3)", 1280, 720, window_flags);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);
    ImFont* interfaceFont = io.Fonts->AddFontDefault();

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    printf("about to enter main loop\n");

    // Main loop
    bool done = false;
    while (!done)
    {

        //printf("on main loop\n");

        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT)
                done = true;
            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        applyStyleColors();

        interfaceFont->Scale = interfaceConfig->getFontScaleFactor();
        ImGui::PushFont(interfaceFont);

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

        loopCallback();

        ImGui::PopFont();

        // Rendering
        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DestroyContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void TravailleurContext::applyStyleColors() const
{
    const InterfaceConfig::StyleColors styleColors = interfaceConfig->getStyleColors();
    switch (styleColors) {
        case InterfaceConfig::StyleColors::Light:
            ImGui::StyleColorsLight();
            break;
        case InterfaceConfig::StyleColors::Classic:
            ImGui::StyleColorsClassic();
            break;
        case InterfaceConfig::StyleColors::Dark:
            ImGui::StyleColorsDark();
            break;
        default:
            ImGui::StyleColorsLight();
    }
}

std::string TravailleurContext::getName() const
{
    return name;
}

std::string TravailleurContext::getPath() const
{
    return TravailleurUtils::getContextPath(getName());
}

void TravailleurContext::load()
{
    SimplePersistenceUtils::loadBool("shouldShowInterfaceConfigGui", shouldShowInterfaceConfigGui, TravailleurUtils::getConfigPath(name));
    SimplePersistenceUtils::loadBool("shouldShowCaptureHandlerGui", shouldShowCaptureHandlerGui, TravailleurUtils::getConfigPath(name));
    SimplePersistenceUtils::loadBool("shouldShowPointInputHandlerGui", shouldShowPointInputHandlerGui, TravailleurUtils::getConfigPath(name));
    SimplePersistenceUtils::loadBool("shouldShowRectangularRegionsHandlerGui", shouldShowRectangularRegionsHandlerGui, TravailleurUtils::getConfigPath(name));
    SimplePersistenceUtils::loadBool("shouldShowImagePatternsHandlerGui", shouldShowImagePatternsHandlerGui, TravailleurUtils::getConfigPath(name));
    SimplePersistenceUtils::loadBool("shouldShowImagePreprocessingsHandlerGui", shouldShowImagePreprocessingsHandlerGui, TravailleurUtils::getConfigPath(name));
    SimplePersistenceUtils::loadBool("shouldShowImageOCRsHandlerGui", shouldShowImageOCRsHandlerGui, TravailleurUtils::getConfigPath(name));
    SimplePersistenceUtils::loadBool("shouldShowImageResourcesHandlerGui", shouldShowImageResourcesHandlerGui, TravailleurUtils::getConfigPath(name));
    SimplePersistenceUtils::loadBool("shouldShowTravailleurAppsHandlerGui", shouldShowTravailleurAppsHandlerGui, TravailleurUtils::getConfigPath(name));
    SimplePersistenceUtils::loadBool("shouldShowMainAppWindowGui", shouldShowMainAppWindowGui, TravailleurUtils::getConfigPath(name));
    SimplePersistenceUtils::loadBool("shouldShowTravailleurConsoleGui", shouldShowTravailleurConsoleGui, TravailleurUtils::getConfigPath(name));
}

void TravailleurContext::save() const
{
    SimplePersistenceUtils::saveBool("shouldShowInterfaceConfigGui", shouldShowInterfaceConfigGui, TravailleurUtils::getConfigPath(name));
    SimplePersistenceUtils::saveBool("shouldShowCaptureHandlerGui", shouldShowCaptureHandlerGui, TravailleurUtils::getConfigPath(name));
    SimplePersistenceUtils::saveBool("shouldShowPointInputHandlerGui", shouldShowPointInputHandlerGui, TravailleurUtils::getConfigPath(name));
    SimplePersistenceUtils::saveBool("shouldShowRectangularRegionsHandlerGui", shouldShowRectangularRegionsHandlerGui, TravailleurUtils::getConfigPath(name));
    SimplePersistenceUtils::saveBool("shouldShowImagePatternsHandlerGui", shouldShowImagePatternsHandlerGui, TravailleurUtils::getConfigPath(name));
    SimplePersistenceUtils::saveBool("shouldShowImagePreprocessingsHandlerGui", shouldShowImagePreprocessingsHandlerGui, TravailleurUtils::getConfigPath(name));
    SimplePersistenceUtils::saveBool("shouldShowImageOCRsHandlerGui", shouldShowImageOCRsHandlerGui, TravailleurUtils::getConfigPath(name));
    SimplePersistenceUtils::saveBool("shouldShowImageResourcesHandlerGui", shouldShowImageResourcesHandlerGui, TravailleurUtils::getConfigPath(name));
    SimplePersistenceUtils::saveBool("shouldShowTravailleurAppsHandlerGui", shouldShowTravailleurAppsHandlerGui, TravailleurUtils::getConfigPath(name));
    SimplePersistenceUtils::saveBool("shouldShowMainAppWindowGui", shouldShowMainAppWindowGui, TravailleurUtils::getConfigPath(name));
    SimplePersistenceUtils::saveBool("shouldShowTravailleurConsoleGui", shouldShowTravailleurConsoleGui, TravailleurUtils::getConfigPath(name));
}

void TravailleurContext::showMainMenuGui()
{
    if (ImGui::BeginMainMenuBar())
    {
        showFrameworkFramesInMainMenu();
        travailleurAppsHandler->showAppFramesInMainMenu();
        ImGui::EndMainMenuBar();
    }
}

void TravailleurContext::showFrameworkFramesInMainMenu()
{
    if (ImGui::BeginMenu("Framework frames"))
    {
        ImGui::Checkbox("InterfaceConfig", &shouldShowInterfaceConfigGui);
        ImGui::Checkbox("CaptureHandler", &shouldShowCaptureHandlerGui);
        ImGui::Checkbox("PointInputHandler", &shouldShowPointInputHandlerGui);
        ImGui::Checkbox("RectangularRegionsHandler", &shouldShowRectangularRegionsHandlerGui);
        ImGui::Checkbox("ImagePatternsHandler", &shouldShowImagePatternsHandlerGui);
        ImGui::Checkbox("ImagePreprocessingsHandler", &shouldShowImagePreprocessingsHandlerGui);
        ImGui::Checkbox("ImageOCRsHandler", &shouldShowImageOCRsHandlerGui);
        ImGui::Checkbox("ImageResourcesHandler", &shouldShowImageResourcesHandlerGui);
        ImGui::Checkbox("TravailleurAppsHandler", &shouldShowTravailleurAppsHandlerGui);
        ImGui::Checkbox("MainAppWindow", &shouldShowMainAppWindowGui);
        ImGui::Checkbox("TravailleurConsole", &shouldShowTravailleurConsoleGui);
        ImGui::EndMenu();
    }
}

void TravailleurContext::initialize(const std::vector<TravailleurApp*> travailleurApps)
{
    load();

    assert(name != "");
    TravailleurUtils::assureWorkingDirectoriesExist(name);

    // temporarily disabled
    ImageOCRUtils::addInitialTesseractEngines();

    interfaceConfig = std::make_unique<InterfaceConfig>(this);
    captureHandler = std::make_unique<CaptureHandler>(this);
    pointInputHandler = std::make_unique<PointInputHandler>(this);
    rectangularRegionsHandler = std::make_unique<RectangularRegionsHandler>(this);
    imagePatternsHandler = std::make_unique<ImagePatternsHandler>(this);
    imagePreprocessingsHandler = std::make_unique<ImagePreprocessingsHandler>(this);
    imageOCRsHandler = std::make_unique<ImageOCRsHandler>(this);
    imageResourcesHandler = std::make_unique<ImageResourcesHandler>(this);

    travailleurAppsHandler = std::make_unique<TravailleurAppsHandler>(this, travailleurApps);

    mainAppWindow = std::make_unique<MainAppWindow>(this);

    travailleurConsole = std::make_unique<TravailleurConsole>(this);

    hotkeyHandler = std::make_unique<HotkeyHandler>();

    rectangularRegionsHandler->afterAppSetup();
    imagePatternsHandler->afterAppSetup();
    imagePreprocessingsHandler->afterAppSetup();
    imageOCRsHandler->afterAppSetup();
    imageResourcesHandler->afterAppSetup();

    contextInitialized = true;
}

void TravailleurContext::deinitialize()
{
    save();

    ImageOCRUtils::removeTesseractEngines();
}

void TravailleurContext::loopCallback()
{
    update();
    showGuis();
}

void TravailleurContext::update()
{
    hotkeyHandler->updateKeysStates();

    captureHandler->update();
    
    if (!travailleurAppsHandler->isExecutingApp()) {
        if (shouldShowPointInputHandlerGui) {
            pointInputHandler->showFrameOverlays(captureHandler->getFrame(), captureHandler->getFrameOverlays());
        }
        if (shouldShowRectangularRegionsHandlerGui) {
            rectangularRegionsHandler->showFrameOverlays(captureHandler->getFrame(), captureHandler->getFrameOverlays());
        }
        if (shouldShowImagePatternsHandlerGui) {
            imagePatternsHandler->showFrameOverlays(captureHandler->getFrame(), captureHandler->getFrameOverlays());
        }
        if (shouldShowImagePreprocessingsHandlerGui) {
            imagePreprocessingsHandler->showFrameOverlays(captureHandler->getFrame(), captureHandler->getFrameOverlays());
        }
        if (shouldShowImageOCRsHandlerGui) {
            imageOCRsHandler->showFrameOverlays(captureHandler->getFrame(), captureHandler->getFrameOverlays());
        }
        if (shouldShowImageResourcesHandlerGui) {
            imageResourcesHandler->showFrameOverlays(captureHandler->getFrame(), captureHandler->getFrameOverlays());
        }
    
        imagePatternsHandler->performActionsThatDependOnFrame(captureHandler->getFrame(), captureHandler->getFrameOverlays());
        imagePreprocessingsHandler->performActionsThatDependOnFrame(captureHandler->getFrame(), captureHandler->getFrameOverlays());
        imageOCRsHandler->performActionsThatDependOnFrame(captureHandler->getFrame(), captureHandler->getFrameOverlays());
    }

    travailleurAppsHandler->update();
    travailleurAppsHandler->feedNewFrameIfNeeded(captureHandler->getFrame());
}

void TravailleurContext::showGuis()
{
    //printf("on showGuis\n");

    showMainMenuGui();

    if (shouldShowInterfaceConfigGui) {
        interfaceConfig->showGui();
    }

    if (shouldShowCaptureHandlerGui) {
        captureHandler->showGui();
    }

    if (!travailleurAppsHandler->isExecutingApp()) {
        if (shouldShowPointInputHandlerGui) {
            pointInputHandler->showGui();
        }

        if (shouldShowRectangularRegionsHandlerGui) {
            rectangularRegionsHandler->showGui();
        }

        if (shouldShowImagePatternsHandlerGui) {
            imagePatternsHandler->showGui();
        }

        if (shouldShowImagePreprocessingsHandlerGui) {
            imagePreprocessingsHandler->showGui();
        }

        if (shouldShowImageOCRsHandlerGui) {
            imageOCRsHandler->showGui();
        }

        if (shouldShowImageResourcesHandlerGui) {
            imageResourcesHandler->showGui();
        }
    }

    if (shouldShowTravailleurAppsHandlerGui) {
        travailleurAppsHandler->showGui();
    }

    if (shouldShowMainAppWindowGui) {
        mainAppWindow->showGui();
    }

    if (shouldShowTravailleurConsoleGui) {
        travailleurConsole->showGui();
    }

    travailleurAppsHandler->showGuiForAppInExecution();
}

void TravailleurContext::registerPointInput(const std::string& name)
{
    assert(!contextInitialized);
    pointInputHandler->registerPointInput(name);
}

PointInput* TravailleurContext::getPointInput(const std::string& name)
{
    return pointInputHandler->getPointInput(name);
}

void TravailleurContext::registerRectangularRegion(const std::string& name)
{
    assert(!contextInitialized);
    rectangularRegionsHandler->registerRectangularRegion(name);
}

RectangularRegion* TravailleurContext::getRectangularRegion(const std::string& name)
{
    return rectangularRegionsHandler->getRectangularRegion(name);
}

std::vector<std::string> TravailleurContext::rectangularRegionsNames() const
{
    return rectangularRegionsHandler->getRectangularRegionsNames();
}

void TravailleurContext::registerImagePattern(const std::string& name)
{
    assert(!contextInitialized);
    imagePatternsHandler->registerImagePattern(name);
}

ImagePattern* TravailleurContext::getImagePattern(const std::string& name)
{
    return imagePatternsHandler->getImagePattern(name);
}

std::vector<std::string> TravailleurContext::imagePatternNames() const
{
    return imagePatternsHandler->getImagePatternsNames();
}

void TravailleurContext::registerImagePreprocessing(const std::string& name)
{
    assert(!contextInitialized);
    imagePreprocessingsHandler->registerImagePreprocessing(name);
}

ImagePreprocessing* TravailleurContext::getImagePreprocessing(const std::string& name)
{
    return imagePreprocessingsHandler->getImagePreprocessing(name);
}

std::vector<std::string> TravailleurContext::imagePreprocessingsNames() const
{
    return imagePreprocessingsHandler->getImagePreprocessingsNames();
}

void TravailleurContext::registerImageOCR(const std::string& name)
{
    assert(!contextInitialized);
    imageOCRsHandler->registerImageOCR(name);
}

ImageOCR* TravailleurContext::getImageOCR(const std::string& name)
{
    return imageOCRsHandler->getImageOCR(name);
}

std::vector<std::string> TravailleurContext::imageOCRsNames() const
{
    return imageOCRsHandler->getImageOCRsNames();
}

void TravailleurContext::registerImageResource(const std::string& name)
{
    assert(!contextInitialized);
    imageResourcesHandler->registerImageResource(name);
}

ImageResource* TravailleurContext::getImageResource(const std::string& name)
{
    return imageResourcesHandler->getImageResource(name);
}

InterfaceConfig* TravailleurContext::getInterfaceConfig() const
{
    return interfaceConfig.get();
}

CaptureHandler* TravailleurContext::getCaptureHandler() const
{
    return captureHandler.get();
}

RectangularRegionsHandler* TravailleurContext::getRectangularRegionsHandler() const
{
    return rectangularRegionsHandler.get();
}

ImagePatternsHandler* TravailleurContext::getImagePatternsHandler() const
{
    return imagePatternsHandler.get();
}

ImagePreprocessingsHandler* TravailleurContext::getImagePreprocessingsHandler() const
{
    return imagePreprocessingsHandler.get();
}

ImageOCRsHandler* TravailleurContext::getImageOCRsHandler() const
{
    return imageOCRsHandler.get();
}

ImageResourcesHandler* TravailleurContext::getImageResourcesHandler() const
{
    return imageResourcesHandler.get();
}

TravailleurAppsHandler* TravailleurContext::getTravailleurAppsHandler() const
{
    return travailleurAppsHandler.get();
}

TravailleurConsole* TravailleurContext::getTravailleurConsole() const
{
    return travailleurConsole.get();
}

HotkeyHandler* TravailleurContext::getHotkeyHandler() const
{
    return hotkeyHandler.get();
}

Point2D<int> TravailleurContext::invalidPoint()
{
    return Point2D<int>(1000000, 1000000);
}

void TravailleurContext::pressKey(const KeyboardAndMouseDefs::Key key, const int pressTimeInMs)
{
    KeyboardAndMouseUtils::pressKey(key, pressTimeInMs);
}

void TravailleurContext::pressLeftMouseButton(const int pressTimeInMs)
{
    KeyboardAndMouseUtils::pressLeftMouseButton(pressTimeInMs);
}

void TravailleurContext::pressRightMouseButton(const int pressTimeInMs)
{
    KeyboardAndMouseUtils::pressRightMouseButton(pressTimeInMs);
}

void TravailleurContext::moveMouse(const Point2D<int>& point)
{
    KeyboardAndMouseUtils::moveMouse(point);
}

void TravailleurContext::moveMouse(const std::string& pointInputName)
{
    const PointInput* pointInput = pointInputHandler->getPointInput(pointInputName);
    assert(pointInput != nullptr);
    const Point2D<int>& point = pointInput->getPoint();
    moveMouse(point);
}