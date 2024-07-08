#include "DummyApp1.h"

#include <thread>

std::string DummyApp1::getName() const
{
    return "DummyApp1";
}

void DummyApp1::setup()
{
    registerRectangularRegion("rectangularRegion1");
    registerRectangularRegion("rectangularRegion2");
    registerRectangularRegion("rectangularRegion3");
    registerRectangularRegion("rectangularRegion4");
    registerRectangularRegion("rectangularRegion5");
    registerRectangularRegion("rectangularRegion6");
    registerRectangularRegion("rectangularRegion7");
    registerRectangularRegion("rectangularRegion8");
    registerRectangularRegion("rectangularRegion9");

    registerImagePattern("imagePattern1");
    registerImagePattern("imagePattern2");
    registerImagePattern("imagePattern3");
    registerImagePattern("imagePattern4");
    registerImagePattern("imagePattern5");
    registerImagePattern("imagePattern6");
    registerImagePattern("imagePattern7");
    registerImagePattern("imagePattern8");
    registerImagePattern("imagePattern9");

    registerImagePreprocessing("imagePreprocessing1");
    registerImagePreprocessing("imagePreprocessing2");
    registerImagePreprocessing("imagePreprocessing3");
    registerImagePreprocessing("imagePreprocessing4");
    registerImagePreprocessing("imagePreprocessing5");
    registerImagePreprocessing("imagePreprocessing6");
    registerImagePreprocessing("imagePreprocessing7");
    registerImagePreprocessing("imagePreprocessing8");
    registerImagePreprocessing("imagePreprocessing9");

    registerImageOCR("imageOCR1");
    registerImageOCR("imageOCR2");
    registerImageOCR("imageOCR3");
    registerImageOCR("imageOCR4");
    registerImageOCR("imageOCR5");
    registerImageOCR("imageOCR6");
    registerImageOCR("imageOCR7");
    registerImageOCR("imageOCR8");
    registerImageOCR("imageOCR9");

    registerImageResource("imageResource1");
    registerImageResource("imageResource2");
    registerImageResource("imageResource3");
    registerImageResource("imageResource4");
    registerImageResource("imageResource5");
    registerImageResource("imageResource6");
    registerImageResource("imageResource7");
    registerImageResource("imageResource8");
    registerImageResource("imageResource9");

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