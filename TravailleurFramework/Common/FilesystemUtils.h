#ifndef FILESYSTEMUTILS_H
#define FILESYSTEMUTILS_H

#include <filesystem>
#include <string>
#include <vector>
#define NOMINMAX
#include <windows.h>

namespace FilesystemUtils {

    void globFiles(std::vector<std::string>& files, const std::string& searchDir, const std::string& extension = "", bool recursively = true);

    size_t sizeOfFileInBytes(const std::string& filepath);

    size_t sizeOfFilesInBytes(const std::vector<std::string>& filepaths);

    void createDirectory(const std::string& dirPath);

    bool directoryExists(const std::string& dirPath);

    bool fileExists(const std::string& filepath);

    bool deleteFile(const std::string& filepath);

    bool deleteFolder(const std::string& folderpath);

    void assureDirectoryExists(const std::string& dirPath);

    void openDirectoryInExplorer(const std::string& dirPath);

    std::string getNextNumberFilenameBasedOnExistingFiles(const std::string& dirPath, const std::string& extension, const size_t nOfPaddingZeros = 4);

    void bytesToReadableSizesString(std::string& output, const size_t nBytes);

    bool string_ends_with(const char* str, const char* suffix);

    void convert_character_to_lowercase_if_not_already(char& character);

    bool c_strings_are_equal_noncasesensitive(const char* str1, const char* str2);

    bool string_ends_with_noncasesensitive(const char* str, const char* suffix);

    bool filepathHasExtension(const std::string& filepath, const std::string& extensionWithoutDot);

    std::string getValueOfEnvironmentVariable(const std::string& environmentVariable);

    std::string getUserDirectory();

    std::string parentDirectoryOfFilepath(const std::string& filepath);

    std::string filenameFromFilepath(const std::string& filepath);

}

#endif