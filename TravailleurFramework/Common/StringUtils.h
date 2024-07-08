#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <set>
#include <string>
#include <vector>

namespace StringUtils {
	void saveToBinaryFile(const std::string& string, FILE* f);
	
	std::string loadFromBinaryFile(FILE* f);

	bool contains(const std::string& string, const std::string& substring);

	std::string toLower(const std::string& string);

	std::string toUpper(const std::string& string);

	std::vector<std::string> split(const std::string& string, const char delimiter);

	std::vector<std::string> splitIntoLines(const std::string& string);

	std::string firstLine(const std::string& string);

	std::string getBooleanDescription(const bool boolean);

	float toNonzeroFloat(const std::string& string);

	void substituteAllInstancesOfCharacterBy(std::string& string, const char charToSubstitute, const char newChar);

	std::string removeAllInstancesOfCharacter(const std::string& string, const char character);

	std::string removeSpaces(const std::string& string);

	std::set<char> setOfDigitCharacters();

	bool onlyContainsCharacters(const std::string& string, const std::set<char>& characters);

	bool doesNotContainCharacters(const std::string& string, const std::set<char>& characters);

	void keepOnlyCharacters(std::string& string, const std::set<char>& characters);

	void removeCharacters(std::string& string, const std::set<char>& characters);

	void removeDigits(std::string& string);

	void keepRemovingFirstCharactersWhileTheyAreWithinSet(std::string& string, const std::set<char>& characters);

	void keepRemovingFirstCharactersWhileTheyAreNotWithinSet(std::string& string, const std::set<char>& characters);

	void keepRemovingLastCharactersWhileTheyAreWithinSet(std::string& string, const std::set<char>& characters);

	void keepRemovingLastCharactersWhileTheyAreNotWithinSet(std::string& string, const std::set<char>& characters);

	void keepOnlyFirstCharactersThatAreWithinSet(std::string& string, const std::set<char>& characters);

	void keepOnlyFirstCharactersThatAreNotWithinSet(std::string& string, const std::set<char>& characters);

	void keepOnlyLastCharactersThatAreWithinSet(std::string& string, const std::set<char>& characters);

	void keepOnlyLastCharactersThatAreNotWithinSet(std::string& string, const std::set<char>& characters);

	void fillLeftWithCharactersUntilSizeIsAtLeast(std::string& string, const char characterToFill, const size_t size);

	void fillRightWithCharactersUntilSizeIsAtLeast(std::string& string, const char characterToFill, const size_t size);
}

#endif