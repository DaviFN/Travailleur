#include "FilesystemUtils.h"
#include <filesystem>
#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdio>

#ifdef _WIN32
    #define NOMINMAX
    #include <windows.h>
    #include <shellapi.h>
#else
    #include <sys/stat.h>
    #include <unistd.h>
#endif

namespace FilesystemUtils {

    void globFiles(std::vector<std::string>& files, const std::string& searchDir, const std::string& extension, bool recursively)
    {
        namespace fs = std::filesystem;
        fs::directory_options options = fs::directory_options::skip_permission_denied;
        if (recursively) {
            for (auto& entry : fs::recursive_directory_iterator(searchDir, options)) {
                if (entry.is_regular_file()) {
                    if (extension.empty() || filepathHasExtension(entry.path().string(), extension)) {
                        files.push_back(entry.path().string());
                    }
                }
            }
        } else {
            for (auto& entry : fs::directory_iterator(searchDir, options)) {
                if (entry.is_regular_file()) {
                    if (extension.empty() || filepathHasExtension(entry.path().string(), extension)) {
                        files.push_back(entry.path().string());
                    }
                }
            }
        }
    }

    size_t sizeOfFileInBytes(const std::string& filepath)
    {
        namespace fs = std::filesystem;
        std::error_code ec;
        auto size = fs::file_size(filepath, ec);
        return ec ? 0 : static_cast<size_t>(size);
    }

    size_t sizeOfFilesInBytes(const std::vector<std::string>& filepaths)
    {
        size_t size = 0;
        for (const std::string& filepath : filepaths) {
            size += sizeOfFileInBytes(filepath);
        }
        return size;
    }

    void createDirectory(const std::string& dirPath)
    {
        std::filesystem::create_directory(dirPath);
    }

    bool directoryExists(const std::string& dirPath)
    {
        return std::filesystem::is_directory(dirPath);
    }

    bool fileExists(const std::string& filepath)
    {
        return std::filesystem::is_regular_file(filepath);
    }

    bool deleteFile(const std::string& filepath)
    {
        return std::filesystem::remove(filepath);
    }

    bool deleteFolder(const std::string& folderpath)
    {
        return std::filesystem::remove_all(folderpath) > 0;
    }

    void assureDirectoryExists(const std::string& dirPath)
    {
        std::filesystem::create_directories(dirPath);
    }

    void openDirectoryInExplorer(const std::string& dirPath) {
    #ifdef _WIN32
        ShellExecuteA(NULL, "open", dirPath.c_str(), NULL, NULL, SW_SHOWDEFAULT);
    #else
        std::string command = "xdg-open \"" + dirPath + "\"";
        std::system(command.c_str());
    #endif
    }

    std::string padStringWithZerosOnTheLeft(const std::string& string, const size_t nZeros)
    {
        return std::string(nZeros - std::min(nZeros, string.length()), '0') + string;
    }

    std::string getNextNumberFilenameBasedOnExistingFiles(const std::string& dirPath, const std::string& extension, const size_t nOfPaddingZeros)
    {
        std::vector<std::string> filesInDir;
        globFiles(filesInDir, dirPath, extension, false);
        size_t number = 1;
        while (true) {
            const std::string numberString = padStringWithZerosOnTheLeft(std::to_string(number), nOfPaddingZeros);
            const std::string filepath =
    #ifdef _WIN32
                dirPath + "\\" + numberString + "." + extension;
    #else
                dirPath + "/" + numberString + "." + extension;
    #endif
            if (!fileExists(filepath)) return numberString;
            ++number;
        }
        return "";
    }

    void bytesToReadableSizesString(std::string& output, const size_t nBytes)
    {
        const float nKiB = (double)nBytes / 1024.0;
        const float nMiB = (double)nKiB / 1024.0;
        const float nGiB = (double)nMiB / 1024.0;
        const float nTiB = (double)nGiB / 1024.0;
        char buffer[4096];
        sprintf(buffer, "nBytes: %zu - nKiB: %.3f - nMiB: %.3f - nGiB: %.3f - nTiB: %.3f", nBytes, nKiB, nMiB, nGiB, nTiB);
        output = buffer;
    }

    bool string_ends_with(const char* str, const char* suffix)
    {
        int str_len = strlen(str);
        int suffix_len = strlen(suffix);
        return (str_len >= suffix_len) && (0 == strcmp(str + (str_len - suffix_len), suffix));
    }

    void convert_character_to_lowercase_if_not_already(char& character)
    {
        if (character >= 'A' && character <= 'Z') {
            character = character + ('a' - 'A');
        }
    }

    bool c_strings_are_equal_noncasesensitive(const char* str1, const char* str2)
    {
        int str_len = strlen(str1);
        int suffix_len = strlen(str2);
        if (str_len == suffix_len) {
            for (size_t i = 0; i < str_len; ++i) {
                char characterFrom1 = str1[i];
                char characterFrom2 = str2[i];
                convert_character_to_lowercase_if_not_already(characterFrom1);
                convert_character_to_lowercase_if_not_already(characterFrom2);
                if (characterFrom1 != characterFrom2) return false;
            }
            return true;
        }
        return false;
    }

    bool string_ends_with_noncasesensitive(const char* str, const char* suffix)
    {
        int str_len = strlen(str);
        int suffix_len = strlen(suffix);

        return (str_len >= suffix_len) && (c_strings_are_equal_noncasesensitive(str + (str_len - suffix_len), suffix));
    }

    bool filepathHasExtension(const std::string& filepath, const std::string& extensionWithoutDot) {
        const std::string extension = "." + extensionWithoutDot;
        return string_ends_with_noncasesensitive(filepath.c_str(), extension.c_str());
    }

    std::string getValueOfEnvironmentVariable(const std::string& environmentVariable)
    {
        const char* value = std::getenv(environmentVariable.c_str());
        return value != nullptr ? value : "";
    }

    std::string getUserDirectory()
    {
    #ifdef _WIN32
        return getValueOfEnvironmentVariable("USERPROFILE");
    #else
        return getValueOfEnvironmentVariable("HOME");
    #endif
    }

    std::string parentDirectoryOfFilepath(const std::string& filepath)
    {
    #ifdef _WIN32
        const char delimiter = '\\';
    #else
        const char delimiter = '/';
    #endif
        const size_t lastDelimiterIndex = filepath.find_last_of(delimiter);
        if (lastDelimiterIndex == std::string::npos) {
            return "";
        }
        return filepath.substr(0, lastDelimiterIndex);
    }

    std::string filenameFromFilepath(const std::string& filepath)
    {
    #ifdef _WIN32
        const char delimiter = '\\';
    #else
        const char delimiter = '/';
    #endif
        const size_t lastDelimiterIndex = filepath.find_last_of(delimiter);
        if (lastDelimiterIndex == std::string::npos) {
            return filepath;
        }
        return filepath.substr(lastDelimiterIndex + 1);
    }

}