#include "FilesystemUtils.h"

#include <filesystem>

namespace FilesystemUtils {

    void globFiles(std::vector<std::string>& files, const std::string& searchDir, const std::string& extension, bool recursively)
    {
        WIN32_FIND_DATA fd;
        const HANDLE hFind = FindFirstFile((searchDir + "\\*").c_str(), &fd);
        if (hFind != INVALID_HANDLE_VALUE) {
            do {
                if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                    if (extension == "") {
                        files.push_back(searchDir + "\\" + fd.cFileName);
                    }
                    else {
                        const size_t filePathSize = strlen(fd.cFileName);
                        if (filePathSize >= extension.size() + 1) {
                            bool fileEndsWithExtension = true;
                            if (fd.cFileName[filePathSize - extension.size() - 1] != '.') {
                                fileEndsWithExtension = false;
                            }
                            else for (size_t i = 0; i < extension.size(); ++i) {
                                char lowercaseFilepathCharacter = fd.cFileName[filePathSize - extension.size() + i];
                                lowercaseFilepathCharacter = lowercaseFilepathCharacter >= 'A' && lowercaseFilepathCharacter <= 'Z' ? lowercaseFilepathCharacter + ('a' - 'A') : lowercaseFilepathCharacter;
                                const char lowercaseExtensionCharacter = extension[i] >= 'A' && extension[i] <= 'Z' ? extension[i] + ('a' - 'A') : extension[i];
                                if (lowercaseFilepathCharacter != lowercaseExtensionCharacter) {
                                    fileEndsWithExtension = false;
                                    break;
                                }
                            }
                            if (fileEndsWithExtension) {
                                files.push_back(searchDir + "\\" + fd.cFileName);
                            }
                        }
                    }
                }
                else if (recursively && strcmp(fd.cFileName, ".") != 0 && strcmp(fd.cFileName, "..") != 0) {
                    globFiles(files, searchDir + "\\" + fd.cFileName, extension, true);
                }
            } while (FindNextFile(hFind, &fd));
            FindClose(hFind);
        }
    }

    size_t sizeOfFileInBytes(const std::string& filepath)
    {
        WIN32_FIND_DATA fd;
        HANDLE h = FindFirstFileA(filepath.c_str(), &fd);
        if (h == INVALID_HANDLE_VALUE) return 0;

        FindClose(h);

        return fd.nFileSizeLow | (__int64)fd.nFileSizeHigh << 32;
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
        CreateDirectoryA(dirPath.c_str(), NULL);
    }


    bool directoryExists(const std::string& dirPath)
    {
        return std::filesystem::is_directory(dirPath);
    }

    bool fileExists(const std::string& filepath)
    {
        const DWORD dwAttrib = GetFileAttributesA((LPCTSTR)filepath.c_str());

        return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
    }

    bool deleteFile(const std::string& filepath)
    {
        return DeleteFileA(filepath.c_str()) == TRUE;
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
        ShellExecuteA(NULL, "open", dirPath.c_str(), NULL, NULL, SW_SHOWDEFAULT);
    }

    inline std::string padStringWithZerosOnTheLeft(const std::string& string, const size_t nZeros)
    {
        return std::string(nZeros - std::min(nZeros, string.length()), '0') + string;
    }

    std::string getNextNumberFilenameBasedOnExistingFiles(const std::string& dirPath, const std::string& extension, const size_t nOfPaddingZeros)
    {
        std::vector<std::string> filesInDir;
        globFiles(filesInDir, dirPath, "png", false);
        size_t number = 1;
        while (true) {
            const std::string numberString = padStringWithZerosOnTheLeft(std::to_string(number), nOfPaddingZeros);
            const std::string filepath = dirPath + "\\" + numberString + "." + extension;
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
        sprintf(buffer, "nBytes: %zu - nKiB: %.3f - nMiB: %.3f - nGiB: %.3f - nTiB: %.3f", (int)nBytes, nKiB, nMiB, nGiB, nTiB);
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
        const char* value = std::getenv("userprofile");
        return value != nullptr ? value : "";
    }

    std::string getUserDirectory()
    {
        return getValueOfEnvironmentVariable("userprofile");
    };

    std::string parentDirectoryOfFilepath(const std::string& filepath)
    {
        const size_t lastDelimiterIndex = filepath.find_last_of("\\");
        if (lastDelimiterIndex == std::string::npos) {
            return "";
        }
        return filepath.substr(0, lastDelimiterIndex);
    }

    std::string filenameFromFilepath(const std::string& filepath)
    {
        const size_t lastDelimiterIndex = filepath.find_last_of("\\");
        if (lastDelimiterIndex == std::string::npos) {
            return filepath;
        }
        return filepath.substr(lastDelimiterIndex + 1);
    }

}