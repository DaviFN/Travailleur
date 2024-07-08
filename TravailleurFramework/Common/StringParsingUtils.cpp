#include "StringParsingUtils.h"

#include "StringUtils.h"

std::string StringParsingUtils::removeAllBeforeSubstring(const std::string& string, const std::string& substring, const bool leaveSubstring)
{
	const size_t substringLocation = string.find(substring);
	if (substringLocation == std::string::npos) {
		return string;
	}
	if (leaveSubstring) {
		return string.substr(substringLocation);
	}
	return string.substr(substringLocation + substring.size());
}

std::string StringParsingUtils::removeAllAfterSubstring(const std::string& string, const std::string& substring, const bool leaveSubstring)
{
	const size_t substringLocation = string.find(substring);
	if (substringLocation == std::string::npos) {
		return string;
	}
	if (leaveSubstring) {
		return string.substr(0, substringLocation + substring.size());
	}
	return string.substr(0, substringLocation);
}