#ifndef common_functions_H
#define common_functions_H

#include <string>
#include <vector>
#include <thread>

#include "TravailleurFramework\Common\travailleur_definitions\travailleur_definitions.h"

void dbgMsg(std::string msg);

void toCharString(char* charString, unsigned int integer);
unsigned int digitsInInteger(unsigned int integer);

bool isColor(int color);
int getRed(int color);
int getGreen(int color);
int getBlue(int color);

FILE* travailleur_fopen_w(std::string filepath);
FILE* travailleur_fopen_wb(std::string filepath);
FILE* travailleur_fopen_r(std::string filepath);
FILE* travailleur_fopen_rb(std::string filepath);
FILE* travailleur_fopen_aplus(std::string filepath);

bool loadDouble(double* double_value, char* path);
bool saveDouble(char* path, double double_value);
bool loadInt(int* integer, char* path);
bool saveInt(char* path, int integer);

bool fileExists(std::string path);

void crash();

void assure_subdirs_exist(std::string filePath);
void assure_directory_exists(std::string filePath);

std::vector<std::string> list_filepaths(const std::string& directory, const std::string& extension);

void correctSlashes(char* str);
void correctSlashes(std::string& str);
void reverseSlashes(std::string& str);

void waitFor(int milisseconds);

void restrictPointToScreen(point* pt);
int max_between(int x, int y);
int min_between(int x, int y);
int difference_between(int x, int y);
double max_between_doubles(double x, double y);
double min_between_doubles(double x, double y);
double difference_between_doubles(double x, double y);

int toInteger(std::string str);
int toIntegerIgnoringRightNonsense(std::string str);
int toIntegerIgnoringLeftNonsense(std::string str);

int tenToPower(int power);
std::string padZerosToNumber(int number, int padQuantity);
std::string getNextScreenshotPath(std::string mainDirectory);

void assureSlashAtEnd(std::string& path);
void assureSlashAtEnd(char* path);

bool directoryExists(std::string directoryPath);

// similar function already existed
//void padZerosToLeft(char* outputString, char* originalString, int numberOfCharacters);

int getLastFileCounter(std::string mainPath, int numberOfDigits, std::string extension);
int getLastFolderCounter(std::string mainPath, int numberOfDigits);

bool keyIsPressed(int vKey);

// removes digits from a number (either from the left or from the right)
int removeDigitsFromLeft(int number, int quantityOfDigitsToRemove);
int removeDigitsFromRight(int number, int quantityOfDigitsToRemove);

// remove duplicates of a vector
template<class T>
void sortAndRemoveDuplicates(std::vector<T>& vec)
{
    std::sort(vec.begin(), vec.end());
    vec.erase(std::unique(vec.begin(), vec.end()), vec.end());
}

void initRNG();
int getRandomNumberBetweenZeroAnd(int maxNumber);

std::string readFile(std::string filePath);
std::string getLineFromPosition(const std::string& str, std::size_t pos);
std::vector<std::string> separateStringIntoLines(const std::string& str);

double roundHalfToEven(double doubleValue);

bool is_travailleur_path(const std::string& path);
bool is_normal_path(const std::string& path);
std::string assure_travailleur_path(const std::string& path);
std::string assure_normal_path(const std::string& path);

std::vector<std::string> list_files_in_directory(const std::string& directory, const std::string& extension);

std::string stringToHex(const std::string& str);
std::string byteToHex(unsigned char byte);

bool stringVectorContainsString(const std::vector<std::string> strVector, const std::string& str);

bool loadStringVectorFromFile(std::vector<std::string>& strVector, const std::string& fileName);
bool saveStringVectorToFile(const std::string& fileName, const std::vector<std::string>& strVector);

void saveStringToFilePointer(const std::string& str, FILE* f);
void loadStringFromFilePointer(std::string& str, FILE* f);

void printStringVector(const std::vector<std::string>& strVector);


#endif