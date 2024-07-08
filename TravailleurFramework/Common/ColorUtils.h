#ifndef ColorUtils_H
#define ColorUtils_H

namespace ColorUtils {
    bool RGBtoHSV(const int r, const int g, const int b, int& h, int& s, int& v);
    bool HSVtoRGB(const int h, const int s, const int v, int& r, int& g, int& b);
}

#endif