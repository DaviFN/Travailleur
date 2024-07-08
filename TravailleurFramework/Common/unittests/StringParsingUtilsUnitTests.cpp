#include "gtest/gtest.h"

#include "Common/StringParsingUtils.h"

#include <string>

TEST(CommonUnitTests, removeAllBeforeSubstringUnitTests)
{
	const std::string result1 = StringParsingUtils::removeAllBeforeSubstring("asdZXCqwe", "ZXC", true);
	EXPECT_EQ(result1, "ZXCqwe");
	const std::string result2 = StringParsingUtils::removeAllBeforeSubstring("asdZXCqwe", "ZXC", false);
	EXPECT_EQ(result2, "qwe");

	const std::string result3 = StringParsingUtils::removeAllBeforeSubstring("asdZXCqwe", "fgh", true);
	EXPECT_EQ(result3, "asdZXCqwe");
	const std::string result4 = StringParsingUtils::removeAllBeforeSubstring("asdZXCqwe", "fgh", false);
	EXPECT_EQ(result4, "asdZXCqwe");
	const std::string result5 = StringParsingUtils::removeAllBeforeSubstring("asdZXCqwe", "qwe", true);
	EXPECT_EQ(result5, "qwe");
	const std::string result6 = StringParsingUtils::removeAllBeforeSubstring("asdZXCqwe", "qwe", false);
	EXPECT_EQ(result6, "");
}

TEST(CommonUnitTests, removeAllAfterSubstringUnitTests)
{
	const std::string result1 = StringParsingUtils::removeAllAfterSubstring("asdZXCqwe", "ZXC", true);
	EXPECT_EQ(result1, "asdZXC");
	const std::string result2 = StringParsingUtils::removeAllAfterSubstring("asdZXCqwe", "ZXC", false);
	EXPECT_EQ(result2, "asd");

	const std::string result3 = StringParsingUtils::removeAllAfterSubstring("asdZXCqwe", "fgh", true);
	EXPECT_EQ(result3, "asdZXCqwe");
	const std::string result4 = StringParsingUtils::removeAllAfterSubstring("asdZXCqwe", "fgh", false);
	EXPECT_EQ(result4, "asdZXCqwe");
	const std::string result5 = StringParsingUtils::removeAllAfterSubstring("asdZXCqwe", "asd", true);
	EXPECT_EQ(result5, "asd");
	const std::string result6 = StringParsingUtils::removeAllAfterSubstring("asdZXCqwe", "asd", false);
	EXPECT_EQ(result6, "");
}