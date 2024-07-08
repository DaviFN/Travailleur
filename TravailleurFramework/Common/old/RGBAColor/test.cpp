#include <cstdio>
#include <cstdlib>
#include <ctime>

#include "RGBAColor.h"
#include "NaiveRGBAColor.h"

bool someErrorOccurred = false;

bool representSameColor(const RGBAColor& rgbaColor, const NaiveRGBAColor& naiveRgbaColor)
{
    return rgbaColor.getRed() == naiveRgbaColor.getRed() && rgbaColor.getGreen() == naiveRgbaColor.getGreen() && rgbaColor.getBlue() == naiveRgbaColor.getBlue() && rgbaColor.getAlpha() == naiveRgbaColor.getAlpha();
}

void printRgbaColor(const RGBAColor& rgbaColor) {
    printf("R: %d, G: %d, B: %d, A: %d", rgbaColor.getRed(), rgbaColor.getGreen(), rgbaColor.getBlue(), rgbaColor.getAlpha());
}

void printNaiveRgbaColor(const NaiveRGBAColor& naiveRgbaColor) {
    printf("R: %d, G: %d, B: %d, A: %d", naiveRgbaColor.getRed(), naiveRgbaColor.getGreen(), naiveRgbaColor.getBlue(), naiveRgbaColor.getAlpha());
}

void compareAndWarnIfDifferent(const RGBAColor& rgbaColor, const NaiveRGBAColor& naiveRgbaColor)
{
    if(!representSameColor(rgbaColor, naiveRgbaColor)) {
        printf("don't represent same color;\nrgbaColor: ");
        printRgbaColor(rgbaColor);
        printf("\nnaiveRgbaColor: ");
        printNaiveRgbaColor(naiveRgbaColor);
        printf("\n");
        someErrorOccurred = true;
    }
}

void initializeRNGSeed()
{
    srand(time(0));
}

uint8_t randomColorComponent() {
    uint16_t randomComponent = rand() % 256;
    return randomComponent;
}

int main()
{
    RGBAColor rgbaColor;
    NaiveRGBAColor naiveRgbaColor;

    compareAndWarnIfDifferent(rgbaColor, naiveRgbaColor);

    initializeRNGSeed();

    printf("testing getting and setters for some random colors...");

    size_t iteration = 0;
    size_t nOfTestSamples = 100000000;
    for(size_t i = 0 ; i < nOfTestSamples ; ++i) {
        iteration++;
        if(iteration % 10000000 == 0) {
            printf("iteration: %d\n", iteration);
            printf("(%lf %%)\n", ((double)iteration / nOfTestSamples) * 100);
        }

        uint8_t red = randomColorComponent();
        uint8_t green = randomColorComponent();
        uint8_t blue = randomColorComponent();
        uint8_t alpha = randomColorComponent();

        rgbaColor.setRed(red);
        rgbaColor.setGreen(green);
        rgbaColor.setBlue(blue);
        rgbaColor.setAlpha(alpha);

        naiveRgbaColor.setRed(red);
        naiveRgbaColor.setGreen(green);
        naiveRgbaColor.setBlue(blue);
        naiveRgbaColor.setAlpha(alpha);

        compareAndWarnIfDifferent(rgbaColor, naiveRgbaColor);
    }

    if(someErrorOccurred) {
        printf("there is a least one error");
    }

    printf("end.");
}