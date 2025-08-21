#ifndef TRAVAILLEURUTILS_H
#define TRAVAILLEURUTILS_H

#include <string>

#include "FilesystemUtils.h"

namespace TravailleurUtils {
    std::string getWorkPath();

    void openWorkFolderInExplorer();

    std::string getContextPath(const std::string& contextName);

    std::string getConfigPath(const std::string& contextName);

    std::string getConfigFilenameFor(const std::string& filepath, const std::string& contextName);

    std::string getScreenshotsPath(const std::string& contextName);

    std::string getVideoRecordingsPath(const std::string& contextName);

    std::string getPointInputsPath(const std::string& contextName);

    std::string getRectangularRegionsPath(const std::string& contextName);

    std::string getImagePatternsPath(const std::string& contextName);

    std::string getImagePreprocessingsPath(const std::string& contextName);

    std::string getImageOCRsPath(const std::string& contextName);

    std::string getImageResourcesPath(const std::string& contextName);

    std::string getAppsPath(const std::string& contextName);

    std::string getConsolePath(const std::string& contextName);

    void assureWorkingDirectoriesExist(const std::string& contextName);

    void showErrorMessage(const std::string& message);
}

#endif