#include "TravailleurUtils.h"

#include "Common/FilesystemUtils.h"

namespace TravailleurUtils {

    std::string getWorkPath()
    {
        return FilesystemUtils::getUserDirectory() + "\\" + "travailleur-workpath";
    }

    void openWorkFolderInExplorer()
    {
        FilesystemUtils::openDirectoryInExplorer(getWorkPath());
    }

    std::string getContextPath(const std::string& contextName)
    {
        return getWorkPath() + "\\" + contextName;
    }

    std::string getConfigPath(const std::string& contextName)
    {
        return getContextPath(contextName) + "\\config";
    }

    std::string getConfigFilenameFor(const std::string& filepath, const std::string& contextName)
    {
        return getConfigPath(contextName) + "\\" + filepath;
    }

    std::string getScreenshotsPath(const std::string& contextName)
    {
        return getContextPath(contextName) + "\\screenshots";
    }

    std::string getVideoRecordingsPath(const std::string& contextName)
    {
        return getContextPath(contextName) + "\\video-recordings";
    }

    std::string getPointInputsPath(const std::string& contextName)
    {
        return getContextPath(contextName) + "\\point-inputs";
    }

    std::string getRectangularRegionsPath(const std::string& contextName)
    {
        return getContextPath(contextName) + "\\rectangular-regions";
    }

    std::string getImagePatternsPath(const std::string& contextName)
    {
        return getContextPath(contextName) + "\\image-patterns";
    }

    std::string getImagePreprocessingsPath(const std::string& contextName)
    {
        return getContextPath(contextName) + "\\image-preprocessings";
    }

    std::string getImageOCRsPath(const std::string& contextName)
    {
        return getContextPath(contextName) + "\\image-ocrs";
    }

    std::string getImageResourcesPath(const std::string& contextName)
    {
        return getContextPath(contextName) + "\\image-resources";
    }

    std::string getAppsPath(const std::string& contextName)
    {
        return getContextPath(contextName) + "\\apps";
    }

    std::string getConsolePath(const std::string& contextName)
    {
        return getContextPath(contextName) + "\\console";
    }

    void assureWorkingDirectoriesExist(const std::string& contextName)
    {
        FilesystemUtils::assureDirectoryExists(getWorkPath());
        FilesystemUtils::assureDirectoryExists(getContextPath(contextName));
        FilesystemUtils::assureDirectoryExists(getConfigPath(contextName));
        FilesystemUtils::assureDirectoryExists(getScreenshotsPath(contextName));
        FilesystemUtils::assureDirectoryExists(getVideoRecordingsPath(contextName));
        FilesystemUtils::assureDirectoryExists(getImagePatternsPath(contextName));
        FilesystemUtils::assureDirectoryExists(getImagePreprocessingsPath(contextName));
        FilesystemUtils::assureDirectoryExists(getImageOCRsPath(contextName));
        FilesystemUtils::assureDirectoryExists(getImageResourcesPath(contextName));
        FilesystemUtils::assureDirectoryExists(getAppsPath(contextName));
        FilesystemUtils::assureDirectoryExists(getConsolePath(contextName));
    }

    void showErrorMessage(const std::string& message)
    {
        MessageBoxA(NULL, message.c_str(), "Error", MB_OK | MB_ICONERROR);
    }
}