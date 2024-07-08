#include "HexUtils.h"

namespace HexUtils {
	char singleHexDigitFromInteger(const uint8_t integer)
	{
		if (integer >= 0 && integer <= 9) {
			return '0' + integer;
		}
		if (integer >= 10 && integer <= 15) {
			return 'a' + integer - 10;
		}
		return '?';
	}

	void putHexByteRepresentationInCStrWithoutNullTerminator(char* cStr, const uint8_t byte)
	{
		cStr[0] = singleHexDigitFromInteger(byte / 16);
		cStr[1] = singleHexDigitFromInteger(byte % 16);
	}
}