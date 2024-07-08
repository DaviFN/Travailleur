#include "StringUtils.h"

#include <algorithm>
#include <iterator>

inline bool digit_or_comma(char ch) {
	return (ch >= '0' && ch <= '9') || ch == ',';
}

std::string make_nonzero_float_compatible(const std::string& input) {
	std::string result;
	std::copy_if(input.begin(), input.end(),
		std::back_inserter(result),
		digit_or_comma);
	return result;
}

namespace StringUtils {
	void saveToBinaryFile(const std::string& string, FILE* f)
	{
		const size_t stringSize = string.size();
		fwrite(&stringSize, sizeof(size_t), 1, f);
		if (stringSize > 0) {
			fwrite(string.c_str(), sizeof(char), stringSize, f);
		}
	}

	std::string loadFromBinaryFile(FILE* f)
	{
		size_t stringSize = 0;
		fread(&stringSize, sizeof(size_t), 1, f);
		if (stringSize == 0) return "";
		char* buffer = new char[stringSize + 1];
		buffer[stringSize] = 0;
		fread(buffer, sizeof(char), stringSize, f);
		const std::string string(buffer);
		delete[] buffer;
		return string;
	}

	bool contains(const std::string& string, const std::string& substring)
	{
		return string.find(substring) != std::string::npos;
	}

	std::string toLower(const std::string& string)
	{
		char* tempCString = new char[string.size() + 1];
		tempCString[string.size()] = 0;
		
		for (size_t i = 0; i < string.size(); ++i) {
			tempCString[i] = std::tolower(string[i]);
		}

		const std::string result(tempCString);

		delete[] tempCString;

		return result;
	}

	std::string toUpper(const std::string& string)
	{
		char* tempCString = new char[string.size() + 1];
		tempCString[string.size()] = 0;

		for (size_t i = 0; i < string.size(); ++i) {
			tempCString[i] = std::toupper(string[i]);
		}

		const std::string result(tempCString);

		delete[] tempCString;

		return result;
	}

	std::vector<std::string> split(const std::string& string, const char delimiter)
	{
		std::vector<std::string> result;

		const size_t strSize = string.size();
		size_t i = 0;
		while (i < strSize) {
			size_t j = i;
			while (j < strSize && string[j] != delimiter) {
				++j;
			}

			const size_t argumentSize = j - i;
			if (argumentSize > 0) {
				char* argument = new char[argumentSize + 1];
				argument[argumentSize] = 0;

				for (size_t k = 0; k < argumentSize; ++k) {
					argument[k] = string[i + k];
				}

				result.push_back(argument);

				delete[] argument;
			}

			i = j + 1;
		}

		return result;
	}

	std::vector<std::string> splitIntoLines(const std::string& string)
	{
		return split(string, '\n');
	}

	std::string firstLine(const std::string& string)
	{
		const std::vector<std::string> lines = splitIntoLines(string);
		if (lines.empty()) return "";
		return lines[0];
	}

	std::string getBooleanDescription(const bool boolean)
	{
		return boolean ? "true" : "false";
	}

	float toNonzeroFloat(const std::string& string)
	{
		const std::string floatCompatibleString = make_nonzero_float_compatible(string);
		float value = 0.0;
		try {
			value = std::stof(floatCompatibleString);
		}
		catch (...) {
			return value;
		}
		return value;
	}

	void substituteAllInstancesOfCharacterBy(std::string& string, const char charToSubstitute, const char newChar)
	{
		for (size_t i = 0; i < string.size(); ++i) {
			if (string[i] == charToSubstitute) {
				string[i] = newChar;
			}
		}
	}

	std::string removeAllInstancesOfCharacter(const std::string& string, const char character)
	{
		std::string result = string;
		std::erase(result, character);
		return result;
	}

	std::string removeSpaces(const std::string& string)
	{
		return removeAllInstancesOfCharacter(string, ' ');
	}

	std::set<char> setOfDigitCharacters()
	{
		std::set<char> set;
		for (char character = '0'; character <= '9'; ++character) {
			set.insert(character);
		}
		return set;
	}

	bool onlyContainsCharacters(const std::string& string, const std::set<char>& characters)
	{
		for (const char& character : string) {
			if (!characters.contains(character)) return false;
		}
		return true;
	}

	bool doesNotContainCharacters(const std::string& string, const std::set<char>& characters)
	{
		return !onlyContainsCharacters(string, characters);
	}

	void keepOnlyCharacters(std::string& string, const std::set<char>& characters)
	{
		char* buffer = new char[string.size() + 1];
		for (size_t i = 0; i < string.size(); ++i) {
			buffer[i] = 0;
		}
		size_t currentIndexInBuffer = 0;
		for (size_t i = 0; i < string.size(); ++i) {
			const char currentCharacter = string[i];
			if (characters.contains(currentCharacter)) {
				buffer[currentIndexInBuffer] = currentCharacter;
				++currentIndexInBuffer;
			}
		}
		string = *buffer;
		delete[] buffer;
	}

	void removeCharacters(std::string& string, const std::set<char>& characters)
	{
		char* buffer = new char[string.size() + 1];
		for (size_t i = 0; i < string.size(); ++i) {
			buffer[i] = 0;
		}
		size_t currentIndexInBuffer = 0;
		for (size_t i = 0; i < string.size(); ++i) {
			const char currentCharacter = string[i];
			if (!characters.contains(currentCharacter)) {
				buffer[currentIndexInBuffer] = currentCharacter;
				++currentIndexInBuffer;
			}
		}
		string = *buffer;
		delete[] buffer;
	}

	void removeDigits(std::string& string)
	{
		removeCharacters(string, setOfDigitCharacters());
	}

	int getPositionOfFirstCharacterThatIsInSet(std::string& string, const std::set<char>& characters)
	{
		int positionOfFirstCharacterThatIsInSet = -1;

		for (size_t i = 0; i < string.size(); ++i) {
			if (characters.contains(string[i])) {
				positionOfFirstCharacterThatIsInSet = i;
				break;
			}
		}

		return positionOfFirstCharacterThatIsInSet;
	}

	int getPositionOfFirstCharacterThatIsNotInSet(std::string& string, const std::set<char>& characters)
	{
		int positionOfFirstCharacterThatIsNotInSet = -1;

		for (size_t i = 0; i < string.size(); ++i) {
			if (!characters.contains(string[i])) {
				positionOfFirstCharacterThatIsNotInSet = i;
				break;
			}
		}

		return positionOfFirstCharacterThatIsNotInSet;
	}

	int getPositionOfLastCharacterThatIsInSet(std::string& string, const std::set<char>& characters)
	{
		int positionOfLastCharacterThatIsInSet = -1;

		for (int i = string.size() - 1; i >= 0; --i) {
			if (characters.contains(string[i])) {
				positionOfLastCharacterThatIsInSet = i;
				break;
			}
		}

		return positionOfLastCharacterThatIsInSet;
	}

	int getPositionOfLastCharacterThatIsNotInSet(std::string& string, const std::set<char>& characters)
	{
		int positionOfLastCharacterThatIsNotInSet = -1;

		for (int i = string.size() - 1; i >= 0; --i) {
			if (!characters.contains(string[i])) {
				positionOfLastCharacterThatIsNotInSet = i;
				break;
			}
		}

		return positionOfLastCharacterThatIsNotInSet;
	}

	void keepRemovingFirstCharactersWhileTheyAreWithinSet(std::string& string, const std::set<char>& characters)
	{
		const int positionOfFirstCharacterThatIsNotInSet = getPositionOfFirstCharacterThatIsNotInSet(string, characters);

		if (positionOfFirstCharacterThatIsNotInSet != -1) {
			string = string.substr(positionOfFirstCharacterThatIsNotInSet);
		}
		else {
			string = "";
		}
	}

	void keepRemovingFirstCharactersWhileTheyAreNotWithinSet(std::string& string, const std::set<char>& characters)
	{
		const int positionOfFirstCharacterThatIsInSet = getPositionOfFirstCharacterThatIsInSet(string, characters);

		if (positionOfFirstCharacterThatIsInSet != -1) {
			string = string.substr(positionOfFirstCharacterThatIsInSet);
		}
		else {
			string = "";
		}
	}

	void keepRemovingLastCharactersWhileTheyAreWithinSet(std::string& string, const std::set<char>& characters)
	{
		const int positionOfLastCharacterThatIsNotInSet = getPositionOfLastCharacterThatIsNotInSet(string, characters);

		if (positionOfLastCharacterThatIsNotInSet != -1) {
			string = string.substr(0, positionOfLastCharacterThatIsNotInSet + 1);
		}
		else {
			string = "";
		}
	}

	void keepRemovingLastCharactersWhileTheyAreNotWithinSet(std::string& string, const std::set<char>& characters)
	{
		const int positionOfLastCharacterThatIsInSet = getPositionOfLastCharacterThatIsInSet(string, characters);

		if (positionOfLastCharacterThatIsInSet != -1) {
			string = string.substr(0, positionOfLastCharacterThatIsInSet + 1);
		}
		else {
			string = "";
		}
	}

	void keepOnlyFirstCharactersThatAreWithinSet(std::string& string, const std::set<char>& characters)
	{
		const int positionOfFirstCharacterThatIsNotInSet = getPositionOfFirstCharacterThatIsNotInSet(string, characters);

		if (positionOfFirstCharacterThatIsNotInSet != -1) {
			string = string.substr(0, positionOfFirstCharacterThatIsNotInSet);
		}
	}

	void keepOnlyFirstCharactersThatAreNotWithinSet(std::string& string, const std::set<char>& characters)
	{
		const int positionOfFirstCharacterThatIsInSet = getPositionOfFirstCharacterThatIsInSet(string, characters);

		if (positionOfFirstCharacterThatIsInSet != -1) {
			string = string.substr(0, positionOfFirstCharacterThatIsInSet);
		}
	}

	void keepOnlyLastCharactersThatAreWithinSet(std::string& string, const std::set<char>& characters)
	{
		const int positionOfLastCharacterThatIsNotInSet = getPositionOfLastCharacterThatIsNotInSet(string, characters);

		if (positionOfLastCharacterThatIsNotInSet != -1) {
			string = string.substr(positionOfLastCharacterThatIsNotInSet + 1);
		}
	}

	void keepOnlyLastCharactersThatAreNotWithinSet(std::string& string, const std::set<char>& characters)
	{
		const int positionOfLastCharacterThatIsInSet = getPositionOfLastCharacterThatIsInSet(string, characters);

		if (positionOfLastCharacterThatIsInSet != -1) {
			string = string.substr(positionOfLastCharacterThatIsInSet + 1);
		}
	}

	void fillLeftWithCharactersUntilSizeIsAtLeast(std::string& string, const char characterToFill, const size_t size)
	{
		if (size > string.size()) {
			char* buffer = new char[size + 1];
			buffer[size] = 0;
			const size_t nCharsToFill = size - string.size();
			for (size_t i = 0; i < nCharsToFill; ++i) {
				buffer[i] = characterToFill;
			}
			for (size_t i = 0; i < string.size(); ++i) {
				buffer[nCharsToFill + i] = string[i];
			}
			string = std::string(buffer);
			delete[] buffer;
		}
	}

	void fillRightWithCharactersUntilSizeIsAtLeast(std::string& string, const char characterToFill, const size_t size)
	{
		if (size > string.size()) {
			char* buffer = new char[size + 1];
			buffer[size] = 0;
			for (size_t i = 0; i < string.size(); ++i) {
				buffer[i] = string[i];
			}
			for (size_t i = string.size(); i < size; ++i) {
				buffer[i] = characterToFill;
			}
			string = std::string(buffer);
			delete[] buffer;
		}
	}
}