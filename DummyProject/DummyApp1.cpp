#include "DummyApp1.h"

#include <thread>

std::string DummyApp1::getName() const
{
    return "DummyApp1";
}

void DummyApp1::setup()
{
    for (uint8_t i = 1; i <= 9; ++i) {
        registerRectangularRegion("rectangularRegion" + std::to_string(i));
        registerImagePattern("imagePattern" + std::to_string(i));
        registerImagePreprocessing("imagePreprocessing" + std::to_string(i));
        registerImageOCR("imageOCR" + std::to_string(i));
        registerImageResource("imageResource" + std::to_string(i));
        registerPointInput("pointerInput" + std::to_string(i));
    }
    addAppFramesDisableOption("DummyApp1 dummy frame");
}

void DummyApp1::processFrame()
{
    static int counter = 0;
    ++counter;
    printf("on DummyApp1::processFrame (counter: %d)\n", counter);

    const bool controlPressed = GetAsyncKeyState(VK_SHIFT) & 0x8000;
    if (controlPressed) {
        const bool hasPattern = hasImagePattern("imagePattern1", "rectangularRegion1");
        printf("hasPattern: %d\n", (int)hasPattern);
    }

    const bool shiftPressed = GetAsyncKeyState(VK_SHIFT) & 0x8000;
    if (shiftPressed) {
        const std::string ocrResult = performImageOcr("imageOCR1", "rectangularRegion2");
        printf("ocrResult: %s\n", ocrResult.c_str());
    }

    if (!shiftPressed) {
        showImageResource("imageResource1");
    }
}

void DummyApp1::showPreRunGui()
{
}

void DummyApp1::showRunningGui()
{
    if (frameIsEnabledByUser("DummyApp1 dummy frame")) {
        showDummyFrame1Gui();
    }
}

void DummyApp1::showDummyFrame1Gui()
{
    ImGui::Begin("DummyApp1 dummy frame 1");

    ImGuiTextOutput("this is simple text output");

    ImGui::End();
}