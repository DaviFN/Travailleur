#include "TravailleurUtils.h"
#include "Common/FilesystemUtils.h"
#include <SDL3/SDL.h>

namespace TravailleurUtils {

#ifdef _WIN32
    const char PATH_SEP = '\\';
#else
    const char PATH_SEP = '/';
#endif

    std::string joinPath(const std::string& base, const std::string& addition) {
        if (base.empty()) return addition;
        if (addition.empty()) return base;
        if (base.back() == PATH_SEP)
            return base + addition;
        else
            return base + PATH_SEP + addition;
    }

    std::string getWorkPath() {
        return joinPath(FilesystemUtils::getUserDirectory(), "travailleur-workpath");
    }

    void openWorkFolderInExplorer() {
        FilesystemUtils::openDirectoryInExplorer(getWorkPath());
    }

    std::string getContextPath(const std::string& contextName) {
        return joinPath(getWorkPath(), contextName);
    }

    std::string getConfigPath(const std::string& contextName) {
        return joinPath(getContextPath(contextName), "config");
    }

    std::string getConfigFilenameFor(const std::string& filepath, const std::string& contextName) {
        return joinPath(getConfigPath(contextName), filepath);
    }

    std::string getScreenshotsPath(const std::string& contextName) {
        return joinPath(getContextPath(contextName), "screenshots");
    }

    std::string getVideoRecordingsPath(const std::string& contextName) {
        return joinPath(getContextPath(contextName), "video-recordings");
    }

    std::string getPointInputsPath(const std::string& contextName) {
        return joinPath(getContextPath(contextName), "point-inputs");
    }

    std::string getRectangularRegionsPath(const std::string& contextName) {
        return joinPath(getContextPath(contextName), "rectangular-regions");
    }

    std::string getImagePatternsPath(const std::string& contextName) {
        return joinPath(getContextPath(contextName), "image-patterns");
    }

    std::string getImagePreprocessingsPath(const std::string& contextName) {
        return joinPath(getContextPath(contextName), "image-preprocessings");
    }

    std::string getImageOCRsPath(const std::string& contextName) {
        return joinPath(getContextPath(contextName), "image-ocrs");
    }

    std::string getImageResourcesPath(const std::string& contextName) {
        return joinPath(getContextPath(contextName), "image-resources");
    }

    std::string getAppsPath(const std::string& contextName) {
        return joinPath(getContextPath(contextName), "apps");
    }

    std::string getConsolePath(const std::string& contextName) {
        return joinPath(getContextPath(contextName), "console");
    }

    void assureWorkingDirectoriesExist(const std::string& contextName) {
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

    void showErrorMessage(const std::string& message) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", message.c_str(), nullptr);
    }
}