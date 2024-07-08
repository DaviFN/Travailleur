#include "gtest/gtest.h"

#include "Common/StringUtils.h"

#include <string>

TEST(CommonUnitTests, keepRemovingFirstCharactersWhileTheyAreWithinSetUnitTests)
{
	const std::set setOfDigitCharacters = StringUtils::setOfDigitCharacters();

	std::string testString = "1";

	StringUtils::keepRemovingFirstCharactersWhileTheyAreWithinSet(testString, setOfDigitCharacters);

	EXPECT_EQ(testString, "");

	testString = "a";

	StringUtils::keepRemovingFirstCharactersWhileTheyAreWithinSet(testString, setOfDigitCharacters);

	EXPECT_EQ(testString, "a");

	testString = "123asd123";

	StringUtils::keepRemovingFirstCharactersWhileTheyAreWithinSet(testString, setOfDigitCharacters);

	EXPECT_EQ(testString, "asd123");

	testString = "string that does not contain any digit";

	StringUtils::keepRemovingFirstCharactersWhileTheyAreWithinSet(testString, setOfDigitCharacters);

	EXPECT_EQ(testString, "string that does not contain any digit");
}

TEST(CommonUnitTests, keepRemovingFirstCharactersWhileTheyAreNotWithinSetUnitTests)
{
	const std::set setOfDigitCharacters = StringUtils::setOfDigitCharacters();

	std::string testString = "1";

	StringUtils::keepRemovingFirstCharactersWhileTheyAreNotWithinSet(testString, setOfDigitCharacters);

	EXPECT_EQ(testString, "1");

	testString = "a";

	StringUtils::keepRemovingFirstCharactersWhileTheyAreNotWithinSet(testString, setOfDigitCharacters);

	EXPECT_EQ(testString, "");

	testString = "asd123asd";

	StringUtils::keepRemovingFirstCharactersWhileTheyAreNotWithinSet(testString, setOfDigitCharacters);

	EXPECT_EQ(testString, "123asd");

	testString = "string that does not contain any digit";

	StringUtils::keepRemovingFirstCharactersWhileTheyAreNotWithinSet(testString, setOfDigitCharacters);

	EXPECT_EQ(testString, "");
}

TEST(CommonUnitTests, keepRemovingLastCharactersWhileTheyAreWithinSetUnitTests)
{
	const std::set setOfDigitCharacters = StringUtils::setOfDigitCharacters();

	std::string testString = "1";

	StringUtils::keepRemovingLastCharactersWhileTheyAreWithinSet(testString, setOfDigitCharacters);

	EXPECT_EQ(testString, "");

	testString = "a";

	StringUtils::keepRemovingLastCharactersWhileTheyAreWithinSet(testString, setOfDigitCharacters);

	EXPECT_EQ(testString, "a");

	testString = "123asd123";

	StringUtils::keepRemovingLastCharactersWhileTheyAreWithinSet(testString, setOfDigitCharacters);

	EXPECT_EQ(testString, "123asd");

	testString = "string that does not contain any digit";

	StringUtils::keepRemovingLastCharactersWhileTheyAreWithinSet(testString, setOfDigitCharacters);

	EXPECT_EQ(testString, "string that does not contain any digit");
}

TEST(CommonUnitTests, keepRemovingLastCharactersWhileTheyAreNotWithinSetUnitTests)
{
	const std::set setOfDigitCharacters = StringUtils::setOfDigitCharacters();

	std::string testString = "1";

	StringUtils::keepRemovingLastCharactersWhileTheyAreNotWithinSet(testString, setOfDigitCharacters);

	EXPECT_EQ(testString, "1");

	testString = "a";

	StringUtils::keepRemovingLastCharactersWhileTheyAreNotWithinSet(testString, setOfDigitCharacters);

	EXPECT_EQ(testString, "");

	testString = "asd123asd";

	StringUtils::keepRemovingLastCharactersWhileTheyAreNotWithinSet(testString, setOfDigitCharacters);

	EXPECT_EQ(testString, "asd123");

	testString = "string that does not contain any digit";

	StringUtils::keepRemovingLastCharactersWhileTheyAreNotWithinSet(testString, setOfDigitCharacters);

	EXPECT_EQ(testString, "");
}

TEST(CommonUnitTests, keepOnlyFirstCharactersThatAreWithinSetUnitTests)
{
	const std::set setOfDigitCharacters = StringUtils::setOfDigitCharacters();

	std::string testString = "1";

	StringUtils::keepOnlyFirstCharactersThatAreWithinSet(testString, setOfDigitCharacters);

	EXPECT_EQ(testString, "1");

	testString = "a";

	StringUtils::keepOnlyFirstCharactersThatAreWithinSet(testString, setOfDigitCharacters);

	EXPECT_EQ(testString, "");

	testString = "123asd456";

	StringUtils::keepOnlyFirstCharactersThatAreWithinSet(testString, setOfDigitCharacters);

	EXPECT_EQ(testString, "123");

	testString = "asd123zxc";

	StringUtils::keepOnlyFirstCharactersThatAreWithinSet(testString, setOfDigitCharacters);

	EXPECT_EQ(testString, "");
}

TEST(CommonUnitTests, keepOnlyFirstCharactersThatAreNotWithinSetUnitTests)
{
	const std::set setOfDigitCharacters = StringUtils::setOfDigitCharacters();

	std::string testString = "1";

	StringUtils::keepOnlyFirstCharactersThatAreNotWithinSet(testString, setOfDigitCharacters);

	EXPECT_EQ(testString, "");

	testString = "a";

	StringUtils::keepOnlyFirstCharactersThatAreNotWithinSet(testString, setOfDigitCharacters);

	EXPECT_EQ(testString, "a");

	testString = "123asd456";

	StringUtils::keepOnlyFirstCharactersThatAreNotWithinSet(testString, setOfDigitCharacters);

	EXPECT_EQ(testString, "");

	testString = "asd123zxc";

	StringUtils::keepOnlyFirstCharactersThatAreNotWithinSet(testString, setOfDigitCharacters);

	EXPECT_EQ(testString, "asd");
}

TEST(CommonUnitTests, keepOnlyLastCharactersThatAreWithinSetUnitTests)
{
	const std::set setOfDigitCharacters = StringUtils::setOfDigitCharacters();

	std::string testString = "1";

	StringUtils::keepOnlyLastCharactersThatAreWithinSet(testString, setOfDigitCharacters);

	EXPECT_EQ(testString, "1");

	testString = "a";

	StringUtils::keepOnlyLastCharactersThatAreWithinSet(testString, setOfDigitCharacters);

	EXPECT_EQ(testString, "");

	testString = "123asd456";

	StringUtils::keepOnlyLastCharactersThatAreWithinSet(testString, setOfDigitCharacters);

	EXPECT_EQ(testString, "456");

	testString = "asd456zxc";

	StringUtils::keepOnlyLastCharactersThatAreWithinSet(testString, setOfDigitCharacters);

	EXPECT_EQ(testString, "");
}

TEST(CommonUnitTests, keepOnlyLastCharactersThatAreNotWithinSetUnitTests)
{
	const std::set setOfDigitCharacters = StringUtils::setOfDigitCharacters();

	std::string testString = "1";

	StringUtils::keepOnlyLastCharactersThatAreNotWithinSet(testString, setOfDigitCharacters);

	EXPECT_EQ(testString, "");

	testString = "a";

	StringUtils::keepOnlyLastCharactersThatAreNotWithinSet(testString, setOfDigitCharacters);

	EXPECT_EQ(testString, "a");

	testString = "123asd456";

	StringUtils::keepOnlyLastCharactersThatAreNotWithinSet(testString, setOfDigitCharacters);

	EXPECT_EQ(testString, "");

	testString = "asd456zxc";

	StringUtils::keepOnlyLastCharactersThatAreNotWithinSet(testString, setOfDigitCharacters);

	EXPECT_EQ(testString, "zxc");
}