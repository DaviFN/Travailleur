#include "common_functions.h"

#include <cstdio>
#include <filesystem>
#include <string>
#include <vector>
#include <iostream>

#include "TravailleurFramework\Common\travailleur_definitions\travailleur_definitions.h"

// for debugging purposes
#include <windows.h>


std::wstring s2ws(const std::string& s)
{
    int len;
    int slength = (int)s.length() + 1;
    len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0); 
    wchar_t* buf = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
    std::wstring r(buf);
    delete[] buf;
    return r;
}

void dbgMsg(std::string msg)
{
    //char msgBody[2048];
    //strcpy(msgBody, msg.c_str());
    MessageBoxA(NULL,msg.c_str(), "dbgMsg",MB_OK);
}


void toCharString(char* charString, unsigned int integer)
{
    unsigned int i = 0;
    if(integer == 0)
    {
        charString[0] = '0';
        charString[1] = 0;
        return;
    }
    unsigned int digits = digitsInInteger(integer);
    while(integer != 0)
    {
        char rightMostDigit = integer % 10;
        charString[digits - i - 1] = rightMostDigit + '0';
        i++;
        integer /= 10;
    }
    charString[i] = 0;
};

unsigned int digitsInInteger(unsigned int integer)
{
    int digits = 0;
    while(integer != 0)
    {
        integer /= 10;
        digits++;
    }
    return digits;
};

// color related functions
// color is being represented as 00RRGGBB (32 bit value, integer)

bool isColor(int color)
{
    if((color & 0xff00000) >> 24 == 0)
    {
        return true;
    }
    return false;
}

int getRed(int color)
{
    return (color & 0x00ff0000) >> 16;
}

int getGreen(int color)
{
    return (color & 0x0000ff00) >> 8;
}

int getBlue(int color)
{
    return (color & 0x000000ff);
}

FILE* travailleur_fopen_w(std::string filepath)
{
    correctSlashes(filepath);

    filepath = assure_travailleur_path(filepath);
    assure_subdirs_exist(filepath);

    FILE* f = fopen(filepath.c_str(), "w");
    return f; 
}

FILE* travailleur_fopen_wb(std::string filepath)
{
    correctSlashes(filepath);

    filepath = assure_travailleur_path(filepath);
    assure_subdirs_exist(filepath);

    FILE* f = fopen(filepath.c_str(), "wb");
    return f; 
}

FILE* travailleur_fopen_r(std::string filepath)
{
    correctSlashes(filepath);

    filepath = assure_travailleur_path(filepath);
    assure_subdirs_exist(filepath);

    FILE* f = fopen(filepath.c_str(), "r");
    return f; 
}

FILE* travailleur_fopen_rb(std::string filepath)
{
    correctSlashes(filepath);

    filepath = assure_travailleur_path(filepath);
    assure_subdirs_exist(filepath);

    FILE* f = fopen(filepath.c_str(), "rb");
    return f; 
}

FILE* travailleur_fopen_aplus(std::string filepath)
{
    correctSlashes(filepath);

    filepath = assure_travailleur_path(filepath);
    assure_subdirs_exist(filepath);

    FILE* f = fopen(filepath.c_str(), "a+");
    return f;   
}

bool loadDouble(double* double_value, char* path)
{
    char actualPath[2048];
    sprintf(actualPath, "%s\\%s", travailleurDataPath.c_str(), path);
    //printf("trying to load int from: %s\n", actualPath);
    FILE* f = fopen(actualPath, "rb");
    if(f != NULL)
    {
        fread(double_value, sizeof(double), 1, f);
        fclose(f);
        return true;
    }
    //printf("f == NULL!\n");
    return false;
}

bool saveDouble(char* path, double double_value)
{
    char actualPath[2048];
    sprintf(actualPath, "%s\\%s", travailleurDataPath.c_str(), path);
    assure_subdirs_exist(actualPath);
    FILE* f = fopen(actualPath, "wb");
    if(f != NULL)
    {
        fwrite(&double_value, sizeof(double), 1, f);
        fclose(f);
        return true;
    }
    return false;
}

bool loadInt(int* integer, char* path)
{
    char actualPath[2048];
    sprintf(actualPath, "%s\\%s", travailleurDataPath.c_str(), path);
    //printf("trying to load int from: %s\n", actualPath);
    FILE* f = fopen(actualPath, "rb");
    if(f != NULL)
    {
        fread(integer, sizeof(int), 1, f);
        fclose(f);
        return true;
    }
    //printf("f == NULL!\n");
    return false;
}

bool saveInt(char* path, int integer)
{
    char actualPath[2048];
    sprintf(actualPath, "%s\\%s", travailleurDataPath.c_str(), path);
    assure_subdirs_exist(actualPath);
    FILE* f = fopen(actualPath, "wb");
    if(f != NULL)
    {
        fwrite(&integer, sizeof(int), 1, f);
        fclose(f);
        return true;
    }
    return false;
}

bool fileExists(std::string path)
{
    correctSlashes(path);
    /*
    FILE* f = fopen(filepath.c_str(), "rb");
    if(f == NULL)
    {
        return false;
    }
    fclose(f);
    return true;
    */
    //return std::filesystem::exists(filepath);

    

    //bool result = std::filesystem::is_regular_file(filepath)
    bool result = std::filesystem::is_regular_file(std::filesystem::u8path(path));

    return result;
}

void crash()
{
    printf("[INTENTIONAL CRASH]\n");
    int integer = *(int*)(0);
}

void assure_subdirs_exist(std::string filePath)
{
    if(filePath == "")
    {
        return;
    }

    correctSlashes(filePath);

    // removes the file name and the slash at the end
    int lastSlashPosition = filePath.find_last_of("\\");
    if(lastSlashPosition != std::string::npos)
    {
        filePath = filePath.substr(0, lastSlashPosition);
    }
    //std::cout << "filePath: " << filePath << std::endl;

    //obsolete
    /*

    std::string directoryStructureSoFar;
    while(true)
    {
        int firstSlashPosition = filePath.find_first_of("\\");
        std::string directoryBeingCreated = filePath.substr(0, firstSlashPosition);
        directoryStructureSoFar += directoryBeingCreated;

        if(!std::filesystem::is_directory(directoryBeingCreated) && directoryBeingCreated != "C:")
        {
            std::cout << "creating directory: " << directoryBeingCreated << std::endl;
            std::filesystem::create_directory(directoryStructureSoFar);
        }

        if(firstSlashPosition == std::string::npos)
        {
            break;
        }
        directoryStructureSoFar += "\\";
        filePath = filePath.substr(firstSlashPosition + 1, filePath.size());
    }
    */

    std::filesystem::create_directories(filePath);
    
    
}

void assure_directory_exists(std::string filePath)
{
    if(!directoryExists(filePath))
    {
        assure_subdirs_exist(filePath);
        std::filesystem::create_directory(filePath);
    }
}

// lists all file paths from a particular directory in the system, relative to this directory
std::vector<std::string> list_filepaths(const std::string& directory, const std::string& extension)
{

    //std::vector<std::string> test;
    //return test;

    //printf("directory: %s\n", directory.c_str());
    //printf("extension: %s\n", extension.c_str());

    //std::cout << "listing filepaths for: " << directory << std::endl;

    //std::string currentDirectory = std::filesystem::current_path().string();

    //int currentPathSize = std::filesystem::current_path().string().size();
    //std::cout << currentDirectory << std::endl;
    //std::cout << "list_files_in_directory CALLED" << std::endl;
    
    std::vector<std::string> filesInDirectory;

    //std::string actualDirectory = travailleurDataPath + "\\" + directory;
    std::string test;
    std::string actualDirectory = assure_travailleur_path(directory);

    //printf("actualDirectory: %s\n", actualDirectory.c_str());

    if(!std::filesystem::is_directory(actualDirectory))
    {
        return filesInDirectory;
    }

    //std::cout << "debug 1" << std::endl;

    for (const auto& entry : std::filesystem::recursive_directory_iterator(actualDirectory))
    {
        if(entry.is_directory())
        {
            //list_filepaths(entry.path().string(), extension);
        }
        else
        {
            //std::cout << "debug 2" << std::endl;
            std::string file = entry.path().string();
            
            //int lastSlashPosition = file.find_last_of(currentDirectory);
            //(lastSlashPosition != std::string::npos)
            //{
                //std::cout << "it's " << lastSlashPosition << std::endl;
                //file = file.substr(lastSlashPosition + 1, file.size());
                //std::cout << "file is now" << file << std::endl;
            //}
            /*
            std::cout << "debug 3" << std::endl;
            printf("currentPathSize: %d\n", currentPathSize);
            printf("file.size(): %d\n", file.size());
            std::cout << "file: " << file << std::endl;
            file = file.substr(currentPathSize + 1, -1);
            */
            
            //std::cout << "debug 4" << std::endl;
            //std::cout << "list_files_in_directory debug 1" << std::endl;
            //std::cout << file << std::endl;
            

            if(extension != "")
            {
                /*
                int extensionPosition = file.rfind(extension);
                if(extensionPosition == std::string::npos)
                {
                    continue;
                }
                file = file.substr(0, extensionPosition);
                */
                if(file.size() >= extension.size())
                {
                    std::string supposedExtension = file.substr(file.size() - extension.size(), extension.size());
                    //std::cout << "debug 5" << std::endl;
                    //std::cout << supposedExtension << std::endl;
                    if(supposedExtension != extension)
                    {
                        continue;
                    }
                    //if(substr(file.size() - extension.size()), extension.size())
                }
            }
            //std::cout << "list_files_in_directory debug 2" << std::endl;

            if(extension != "")
            {
                file = file.substr(0, file.size() - extension.size());
            }

            file = file.substr(actualDirectory.size() + 1, file.size());

            //std::cout << "debug 6" << std::endl;
            filesInDirectory.push_back(file);
            //std::cout << "pushing back file: " << file << std::endl;

            //std::cout << file << std::endl;
        }
    }
    
    //std::cout << "list_files_in_directory RETURNED" << std::endl;

    return filesInDirectory;
}

void correctSlashes(char* str)
{
    int i = 0;
    while(str[i] != 0)
    {
        if(str[i] == '/')
        {
            str[i] = '\\';
        }
        i++;
    }
}

void correctSlashes(std::string& str)
{
    std::replace(str.begin(), str.end(), '/', '\\'); // replace all '/' to '\'
}

void reverseSlashes(std::string& str)
{
    std::replace(str.begin(), str.end(), '\\', '/'); // replace all '\' to '/'
}

void waitFor(int milisseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(milisseconds));
}

void restrictPointToScreen(point* pt)
{
    if(pt->x >= GAME_SCREEN_WIDTH)
    {
        pt->x = GAME_SCREEN_WIDTH - 1;
    }
    if(pt->y >= GAME_SCREEN_HEIGHT)
    {
        pt->y = GAME_SCREEN_HEIGHT - 1;
    }
    if(pt->x < 0)
    {
        pt->x = 0;
    }
    if(pt->y < 0)
    {
        pt->y = 0;
    }
}

int max_between(int x, int y)
{
    if(x >= y)
    {
        return x;
    }
    else
    {
        return y;
    }
}

int min_between(int x, int y)
{
    if(x <= y)
    {
        return x;
    }
    else
    {
        return y;
    }
}

int difference_between(int x, int y)
{
    int difference = x - y;
    if(difference < 0)
    {
        difference = -difference;
    }
    return difference;
}

double max_between_doubles(double x, double y)
{
    if(x >= y)
    {
        return x;
    }
    else
    {
        return y;
    }
}

double min_between_doubles(double x, double y)
{
    if(x <= y)
    {
        return x;
    }
    else
    {
        return y;
    }
}

double difference_between_doubles(double x, double y)
{
    double difference = x -  y;
    if(difference < 0)
    {
        difference = -difference;
    }
    return difference;
}

int toInteger(std::string str)
{
    int result = 0;
    int strSize = str.size();
    for(int i = 0 ; i < strSize ; i++)
    {
        if(str[i] >= '0' && str[i] <= '9')
        {
            result = result*10 + str[i] - '0';
        }
    }
    return result;
}

int toIntegerIgnoringRightNonsense(std::string str)
{
    int result = 0;
    int strSize = str.size();
    std::string preProcessedString;
    bool foundADigitAlready = false;
    for(int i = 0 ; i < strSize ; i++)
    {
        if(str[i] >= '0' && str[i] <= '9')
        {
            foundADigitAlready = true;
            preProcessedString.append(str.substr(i, 1));
        }
        if(foundADigitAlready)
        {
            if(!(str[i] >= '0' && str[i] <= '9') && str[i] != '.' && str[i] != ',')
            {
                break;
            }
        }
    }
    int preProcessedStringSize = preProcessedString.size();
    for(int i = 0 ; i < preProcessedStringSize ; i++)
    {
        if(preProcessedString[i] >= '0' && preProcessedString[i] <= '9')
        {
            result = result*10 + preProcessedString[i] - '0';
        }
    }
    return result;
}

int toIntegerIgnoringLeftNonsense(std::string str)
{
    int result = 0;
    int strSize = str.size();
    std::string preProcessedString;
    bool foundADigitAlready = false;
    for(int i = strSize-1 ; i >= 0 ; i--)
    {
        if(str[i] >= '0' && str[i] <= '9')
        {
            foundADigitAlready = true;
            preProcessedString.append(str.substr(i, 1));
        }
        if(foundADigitAlready)
        {
            if(!(str[i] >= '0' && str[i] <= '9') && str[i] != '.' && str[i] != ',')
            {
                break;
            }
        }
    }
    int preProcessedStringSize = preProcessedString.size();
    for(int i = preProcessedStringSize-1 ; i >= 0 ; i--)
    {
        if(preProcessedString[i] >= '0' && preProcessedString[i] <= '9')
        {
            result = result*10 + preProcessedString[i] - '0';
        }
    }
    return result;
}

bool existsFile(std::string filepath)
{
    correctSlashes(filepath);
    /*
    FILE* f = fopen(filepath.c_str(), "rb");
    if(f == NULL)
    {
        return false;
    }
    fclose(f);
    return true;
    */
    //return std::filesystem::exists(filepath);

    

    //bool result = std::filesystem::is_regular_file(filepath)
    bool result = std::filesystem::is_regular_file(std::filesystem::u8path(filepath));

    return result;
}

int tenToPower(int power)
{
    if(power < 0)
    {
        return -1;
    }
    int result = 1;
    for(int i = 0 ; i < power ; i++)
    {
        result *= 10;
    }
    return result;
}

std::string padZerosToNumber(int number, int padQuantity)
{
    int digitsInNumber = digitsInInteger(number);
    if(digitsInNumber > padQuantity || padQuantity < 1 || number < 0)
    {
        return "INVALID STRING - PAD QUANTITY LOGIC ERROR";
    }
    if(number == 0) // idk why (did not check) but this is a special case (otherwise we get padQuantity+1 zeroes)
    {
        char numberString[2048];
        int i;
        for(i = 0 ; i < padQuantity ; i++)
        {
            numberString[i] = '0';
        }
        numberString[i] = 0;
        std::string resultingString = numberString;
        return resultingString;
    }
    char numberString[2048];
    sprintf(numberString, "%d", number);
    std::string resultingString;
    int numberOfLeadingZeros = padQuantity - digitsInNumber;
    for(int i = 0 ; i < numberOfLeadingZeros ; i++)
    {
        resultingString = resultingString + '0';
    }
    resultingString = resultingString + numberString;
    return resultingString;
}

std::string getNextScreenshotPath(std::string mainDirectory)
{
    if(mainDirectory.size() == 0)
    {
        return "INVALID STRING - LOGIC ERROR - getNextScreenshotPath";
    }
    int number = 1;
    char filename[2048];
    std::string numberPath;
    std::string mainDirectoryPath = travailleurDataPath + "/screenshots/" + mainDirectory;
    //printf("numberPath =  %s\n", numberPath.c_str());
    //printf("mainDirectoryPath =  %s\n", mainDirectoryPath.c_str());
    while(true)
    {
        std::string numberPath = mainDirectoryPath + padZerosToNumber(number, 4) + ".bmp";
        correctSlashes(numberPath);
        //printf("checking whether exists:  %s\n", numberPath.c_str());
        //printf("function returns: %d", existsFile(numberPath));
        if(!existsFile(numberPath))
        {
            //printf("does not exist:  %s\n", numberPath.c_str());
            //printf("getNextScreenshotPath returning %s\n", numberPath.c_str());
            std::string pathToReturn = mainDirectory + padZerosToNumber(number, 4);
            return pathToReturn;
        }
        number++;
    }
    return "NEVER SUPPOSED TO GET HERE - COSMIC RADIATION OR SOMETHING";
}

void assureSlashAtEnd(std::string& path)
{
    int pathSize = path.size();
    if(pathSize == 0)
    {
        path.append("/");
        return;
    }
    if(path[pathSize-1] != '/' && path[pathSize-1] != '\\')
    {
        path.append("/");
        return;
    }
}

void assureSlashAtEnd(char* path)
{
    int pathSize = strlen(path);
    if(pathSize == 0)
    {
        path[0] = '/';
        path[1] = 0;
        return;
    }
    if(path[pathSize-1] != '/' && path[pathSize-1] != '\\')
    {
        path[pathSize] = '/';
        path[pathSize + 1] = 0;
        return;
    }
}

bool directoryExists(std::string directoryPath)
{
    bool result = std::filesystem::is_directory(directoryPath);
    return result;
}

/*
void padZerosToLeft(char* outputString, char* originalString, int numberOfCharacters)
{
    if(outputString == NULL || originalString == NULL || numberOfCharacters < 1)
    {
        return;
    }
    int originalStringSize = strlen(originalString);

    if(originalStringSize >= numberOfCharacters)
    {
        return;
    }

    int howManyZerosToPad = numberOfCharacters - originalStringSize;
    int i;
    for(i = 0 ; i < howManyZerosToPad ; i++)
    {
        outputString[i] = '0';
    }
    outputString[i] = 0;

    strcat(outputString, originalString);

}
*/

// gets last file counter, verifies until doesn't exist, pads with zeros (to get numberOfDigits)

int getLastFileCounter(std::string mainPath, int numberOfDigits, std::string extension)
{
    if(numberOfDigits < 1)
    {
        printf("getLastFileCounter error: numberOfDigits < 1 (it is %d)\n", numberOfDigits);
        return 1;
    }

    int fileCounter = 1;
    int maximumFileCounter = 1;
    for(int i = 0 ; i < numberOfDigits ; i++)
    {
        maximumFileCounter *= 10;
    }
    maximumFileCounter -= 1;

    while(fileCounter <= maximumFileCounter)
    {
        std::string paddedString = padZerosToNumber(fileCounter, numberOfDigits);
        std::string fileCounterFileName = mainPath + "\\" + paddedString + extension;

        if(!fileExists(fileCounterFileName))
        {
            return fileCounter;
        }

        fileCounter++;
    }
    
    return 1;

}

// gets last folder counter, verifies until doesn't exist, pads with zeros (to get numberOfDigits)
int getLastFolderCounter(std::string mainPath, int numberOfDigits)
{
    if(numberOfDigits < 1)
    {
        printf("getLastFolderCounter error: numberOfDigits < 1 (it is %d)\n", numberOfDigits);
        return 1;
    }

    int folderCounter = 1;
    int maximumFolderCounter = 1;
    for(int i = 0 ; i < numberOfDigits ; i++)
    {
        maximumFolderCounter *= 10;
    }
    maximumFolderCounter -= 1;

    while(folderCounter <= maximumFolderCounter)
    {
        std::string paddedString = padZerosToNumber(folderCounter, numberOfDigits);
        std::string folderCounterFileName = mainPath + "\\" + paddedString;

        if(!directoryExists(folderCounterFileName))
        {
            return folderCounter;
        }

        folderCounter++;
    }
    
    return 1;

}

#include <windows.h>
bool keyIsPressed(int vKey)
{
    bool result = false;
    if(GetAsyncKeyState(vKey) & 0x8000)
    {
        result = true;
    }
    return result;
}

int removeDigitsFromLeft(int number, int quantityOfDigitsToRemove)
{
    if(quantityOfDigitsToRemove <= 0)
    {
        return number;
    }

    /* // this works but I thought it was better to treat the number as a string

    int numberOfDigits = digitsInInteger(number);

    for(int i = 0 ; i < quantityOfDigitsToRemove ; i++)
    {
        if(number <= 0)
        {
            return number;
        }

        int tenToPowerOfNumberOfDigits = tenToPower(numberOfDigits);

        int firstDigit = number / tenToPowerOfNumberOfDigits;

        number -= firstDigit * tenToPowerOfNumberOfDigits;

    }

    */
    char integerString[2048];
    sprintf(integerString, "%d", number);

    int howManyDigits = strlen(integerString);

    if(quantityOfDigitsToRemove >= howManyDigits)
    {
        return 0;
    }

    char* integerStringWithDigitsRemoved = integerString + sizeof(char) * quantityOfDigitsToRemove;

    int numberWithDigitsRemoved = toInteger(integerStringWithDigitsRemoved);

    return numberWithDigitsRemoved;

}

int removeDigitsFromRight(int number, int quantityOfDigitsToRemove)
{
    if(quantityOfDigitsToRemove < 0)
    {
        return number;
    }

    for(int i = 0 ; i < quantityOfDigitsToRemove ; i++)
    {
        if(number <= 0)
        {
            return number;
        }

        number /= 10;

    }

    return number;

}


void initRNG()
{
    srand(time(NULL));
}

int getRandomNumberBetweenZeroAnd(int maxNumber)
{
    if(maxNumber < 0)
    {
        printf("getRandomNumberBetweenZeroAnd error: maxNumber: %d, returning 0\n", maxNumber);
        return 0;
    }
    int randomizedNumber = rand()%(maxNumber+1);
    return randomizedNumber;
}

std::string readFile(std::string filePath)
{
    std::ifstream inFile;

    std::string actualFilePath = travailleurDataPath + "\\" + filePath;

    //FILE* f = fopen(actualFilePath.c_str(), "rb");
    //if(f != NULL)
    //{
    //    printf("could be open\n");
    //    fclose(f);
    //}
    //else
    //{
    //    printf("could NOT be open\n");
    //}



    if(!existsFile(actualFilePath))
    {
        //printf("cannot readFile cuz %s doesn't exist\n", actualFilePath.c_str());
        
        char aux[2048];
        sprintf(aux, "error: readFile could not read file %s (does not exist)", actualFilePath.c_str());
        dbgMsg(aux);

        return "";
    }
    //else
    //{
    //    printf("%s (exists)\n", actualFilePath.c_str());
    //    dbgMsg("pause");
    //}


    //inFile.open(actualFilePath); //open the input file
    inFile.open(std::filesystem::u8path(actualFilePath));

    std::stringstream strStream;
    strStream << inFile.rdbuf(); //read the file
    return strStream.str(); //str holds the content of the file
}

std::string getLineFromPosition(const std::string& str, std::size_t pos)
{
    std::size_t strSize = str.size();
    std::size_t maxPos = strSize - 1;

    if(pos > maxPos || str[pos] == '\n')
    {
        //printf("getLineFromPosition error: pos > maxPos || str[pos] == '\\n'\n");
        //printf("pos: %d, strSize: %d, maxPos: %d\n", pos, strSize, maxPos);
        return "";
    }

    

    std::size_t initialPos = pos;
    while(initialPos > 0 && str[initialPos-1] != '\n')
    {
        //printf("deb 1, initialPos: %d\n", initialPos);
        initialPos--;
    }

    std::size_t finalPos = pos;

    while(finalPos < maxPos && str[finalPos+1] != '\n')
    {
        //printf("deb 2, finalPos: %d\n", finalPos);
        finalPos++;
    }

    std::size_t lineLength = finalPos - initialPos + 1;

    std::string lineExtracted = str.substr(initialPos, lineLength);

    //printf("initialPos: %d, finalPos: %d, lineLength: %d\n", initialPos, finalPos, lineLength);

    //std::cout << "line extracted: " + lineExtracted << std::endl;

    //printf("lineExtracted[0]: %c\n", lineExtracted[0]);
    //printf("lineExtracted[1]: %c\n", lineExtracted[1]);

    return lineExtracted;

}

std::vector<std::string> separateStringIntoLines(const std::string& str)
{
    std::size_t maxPos = str.size() - 1;
    std::size_t initialPos = 0;

    std::vector<std::string> result;

    int lin = 0;

    //while(initialPos <= maxPos && str[initialPos] == '\n')
    //{
    //    //printf("LOGIC: initialPos: %d, finalPos: %d, maxPos: %d\n", initialPos, finalPos, maxPos);
    //    lin++;
    //    result.push_back("");
    //    printf("pushed lin %d: EMPTY\n", lin);
    //    initialPos++;
    //}

    while(initialPos <= maxPos)
    {
        //printf("initialPos: %d, maxPos: %d\n", initialPos, maxPos);

        size_t finalPos = initialPos;


        while(finalPos <= maxPos && str[finalPos] != '\n')
        {
            finalPos++;
        }
        size_t lineLength = finalPos - initialPos;
        
        result.push_back(str.substr(initialPos, lineLength));
        //printf("pushed lin %d: %s\n", lin, result.back().c_str());
        //printf("initialPos: %d, finalPos: %d\n", initialPos, finalPos);
        lin++;

        //initialPos = finalPos + 1;

        initialPos = finalPos + 1;
        while(initialPos <= maxPos && str[initialPos] == '\n')
        {
            //printf("LOGIC: initialPos: %d, finalPos: %d, maxPos: %d\n", initialPos, finalPos, maxPos);
            lin++;
            result.push_back("");
            //printf("pushed lin %d: EMPTY\n", lin);
            initialPos++;
        }

    }

    if(initialPos == maxPos + 1 && maxPos >= 0 && str[maxPos] == '\n')
    {
        lin++;
        result.push_back("");
        //printf("pushed lin %d: EMPTY\n", lin);
    }


    //dbgMsg("got lines");

    return result;
}

double roundHalfToEven(double f)
{
    const double r = round(f); // Result is round-half-away-from-zero
    const double d = r - f; // Difference

    // Result is not half, RHAFZ result same as RHTE
    if ((d != 0.5f) && (d != -0.5f))
    {
        return r;
    }

    // Check if RHAFZ result is even, then RHAFZ result same as RHTE
    if (fmod(r, 2.0f) == 0.0f)
    {
        return r;
    }

    // Switch to even value
    return f - d;
}

bool is_travailleur_path(const std::string& path)
{
    //printf("is_travailleur_path called for %s\n", path.c_str());
    std::size_t travailleurDataPathPos;
    std::string travailleurPathInitialString = travailleurDataPath + "\\";
    travailleurDataPathPos = path.find(travailleurPathInitialString);
    bool result = false;
    if(travailleurDataPathPos != std::string::npos && travailleurDataPathPos == 0)
    {
        result = true;
    }
    return result;
}
bool is_normal_path(const std::string& path)
{
    bool result = !is_travailleur_path(path);
    return result;
}
std::string assure_travailleur_path(const std::string& path)
{
    //printf("assure_travailleur_path called for %s\n", path.c_str());
    if(!is_travailleur_path(path))
    {
        //printf("is not a travailleur path...");
        std::string result = travailleurDataPath + "\\" + path;
        //printf("returning %s\n", result.c_str());
        return result;
    }
    //printf("is already a travailleur path...");
    return path;
}
std::string assure_normal_path(const std::string& path)
{
    if(!is_normal_path(path))
    {
        printf("assure_normal_path: !is_normal_path(path)\n");
        std::size_t travailleurDataPathInitialStringPos;
        std::string travailleurPathInitialString = travailleurDataPath + "\\";
        std::string result = path.substr(travailleurPathInitialString.size());
        return result;
    }
    printf("assure_normal_path: returning path (is normal path)\n");
    return path;
}

std::vector<std::string> list_files_in_directory(const std::string& directory, const std::string& extension)
{
    std::cout << "list_files_in_directory CALLED" << std::endl;
    //printf("directory: %s\n", directory.c_str());
    //printf("extension: %s\n", extension.c_str());
    
    std::string actualDirectory = assure_travailleur_path(directory);

    if(!directoryExists(actualDirectory))
    {
        std::vector<std::string> result;
        return result;
    }

    std::vector<std::string> filesInDirectory;

    //legacy
    //for (const auto & entry : std::filesystem::directory_iterator(travailleurDataPath + "\\" + directory))


    for (const auto & entry : std::filesystem::directory_iterator(actualDirectory))
    {
        std::string file = entry.path().string();

        //printf("file: %s\n", file.c_str());

        int lastSlashPosition = file.find_last_of("\\");
        if(lastSlashPosition != std::string::npos)
        {
            file = file.substr(lastSlashPosition + 1, file.size());
        }

        //std::cout << "list_files_in_directory debug 1" << std::endl;

        if(extension != "")
        {
            int extensionPosition = file.rfind(extension);
            if(extensionPosition == std::string::npos)
            {
                continue;
            }
            file = file.substr(0, extensionPosition);
        }
        //std::cout << "list_files_in_directory debug 2" << std::endl;

        filesInDirectory.push_back(file);
        //std::cout << "pushing back file: " << file << std::endl;
        //std::cout << file << std::endl;
    }
    
    //std::cout << "list_files_in_directory RETURNED" << std::endl;
    

    return filesInDirectory;
}

std::string stringToHex(const std::string& str)
{
    std::string result;
    int strSize = str.size();
    for(int i = 0 ; i < strSize ; i++)
    {
        result += byteToHex(str[i]);
    }
    return result;
}

std::string byteToHex(unsigned char byte)
{
    unsigned char leftMostHexDigit = byte % 16;
    unsigned char rightMostHexDigit = byte / 16;
    std::string result;
    if(rightMostHexDigit >= 0 && rightMostHexDigit <= 9)
    {
        result += '0' + rightMostHexDigit;
    }
    else
    {
        result += 'A' + rightMostHexDigit - 10;
    }
    if(leftMostHexDigit >= 0 && leftMostHexDigit <= 9)
    {
        result += '0' + leftMostHexDigit;
    }
    else
    {
        result += 'A' + leftMostHexDigit - 10;
    }
    return result;
}

bool stringVectorContainsString(const std::vector<std::string> strVector, const std::string& str)
{
    int strVectorSize = strVector.size();
    for(int i = 0 ; i < strVectorSize ; i++)
    {
        if(strVector[i] == str)
        {
            return true;
        }
    }
    return false;
}

bool saveStringVectorToFile(const std::string& fileName, const std::vector<std::string>& strVector)
{
    FILE* f = travailleur_fopen_wb(fileName);

    if(f == NULL)
    {
        return false;
    }

    int strVectorSize = strVector.size();
    fwrite(&strVectorSize, sizeof(int), 1, f);

    for(int i = 0 ; i < strVectorSize ; i++)
    {
        saveStringToFilePointer(strVector[i], f);
    }

    fclose(f);
    return true;

}

bool loadStringVectorFromFile(std::vector<std::string>& strVector, const std::string& fileName)
{
    FILE* f = travailleur_fopen_rb(fileName);

    if(f == NULL)
    {
        printf("couldn't load string vector from file: %s (couldn't open file)\n", fileName.c_str());
        printf("file was: %s\n", fileName.c_str());
        strVector.clear();
        return false;
    }

    int strVectorSize = 0;
    fread(&strVectorSize, sizeof(int), 1, f);

    for(int i = 0 ; i < strVectorSize ; i++)
    {
        std::string newString;
        loadStringFromFilePointer(newString, f);
        strVector.push_back(newString);
    }

    fclose(f);

    return true;

}

void loadStringFromFilePointer(std::string& str, FILE* f)
{
    int strSize = 0;
    fread(&strSize, sizeof(int), 1, f);
    char* aux = new char[strSize + 1];
    fread(aux, sizeof(char), strSize, f);
    aux[strSize] = 0;
    str = aux;
    delete aux;
}

void saveStringToFilePointer(const std::string& str, FILE* f)
{
    int strSize = str.size();
    fwrite(&strSize, sizeof(int), 1, f);
    fwrite(str.c_str(), sizeof(char), strSize, f);
}

void printStringVector(const std::vector<std::string>& strVector)
{
    printf("vector size: %d\n", strVector.size());
    for(int i = 0 ; i < strVector.size() ; i++)
    {
        printf("%s\n", strVector[i].c_str());
    }
}