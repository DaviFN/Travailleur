#ifndef RGBACOLOR_H
#define RGBACOLOR_H

#include <stdint.h>

class RGBAColor
{
    public:
    inline uint8_t getRed() const
    {
        return (color & 0xff000000) >> 24;
    }

    inline void setRed(uint8_t red) {
        uint32_t mask = red;
        mask = mask << 24;
        color = color & 0x00ffffff;
        color = color | mask;
    }

    inline uint8_t getGreen() const
    {
        return (color & 0x00ff0000) >> 16;
    }

    inline void setGreen(uint8_t green) {
        uint32_t mask = green;
        mask = mask << 16;
        color = color & 0xff00ffff;
        color = color | mask;
    }

    inline uint8_t getBlue() const
    {
        return (color & 0x0000ff00) >> 8;
    }

    inline void setBlue(uint8_t blue) {
        uint32_t mask = blue;
        mask = mask << 8;
        color = color & 0xffff00ff;
        color = color | mask;
    }

    inline uint8_t getAlpha() const
    {
        return color & 0x000000ff;
    }

    inline void setAlpha(uint8_t alpha) {
        color = color & 0xffffff00;
        color = color | alpha;
    }

    public:
    uint32_t color = 0x000000ff;
};

#endif