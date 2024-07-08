#ifndef STRINGPARSINGUTILS_H
#define STRINGPARSINGUTILS_H

#include <string>

namespace StringParsingUtils {
	std::string removeAllBeforeSubstring(const std::string& string, const std::string& substring, const bool leaveSubstring = true);
	std::string removeAllAfterSubstring(const std::string& string, const std::string& substring, const bool leaveSubstring = true);
}

#endif