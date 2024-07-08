#ifndef NAIVERGBACOLOR_H
#define NAIVERGBACOLOR_H

#include <stdint.h>

class NaiveRGBAColor
{
    public:
    uint8_t getRed() const
    {
        return red;
    }

    void setRed(uint8_t red)
    {
        this->red = red;
    }

    uint8_t getGreen() const
    {
        return green;
    }

    void setGreen(uint8_t green)
    {
        this->green = green;
    }

    uint8_t getBlue() const
    {
        return blue;
    }

    void setBlue(uint8_t blue)
    {
        this->blue = blue;
    }

    uint8_t getAlpha() const
    {
        return alpha;
    }

    void setAlpha(uint8_t alpha)
    {
        this->alpha = alpha;
    }

    private:
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
    uint8_t alpha = 255;
};

#endif