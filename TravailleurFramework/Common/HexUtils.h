#ifndef HEXUTILS_H
#define HEXUTILS_H

#include <stdint.h>

namespace HexUtils {
    void putHexByteRepresentationInCStrWithoutNullTerminator(char* cStr, const uint8_t byte);
}

#endif