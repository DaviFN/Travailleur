#include "Point2DPersistenceUtils.h"

#include <cstdio>

namespace Point2D2DPersistenceUtils
{
    inline std::string assembleFilepath(const std::string& persistenceFilename, const std::string& directory)
    {
        return persistenceFilename != "" ? directory + "\\" + persistenceFilename : persistenceFilename;
    }

    SaveOperationResult saveIntPoint2D(const std::string& persistenceFilename, const Point2D<int>& point, const std::string& directory)
    {
        const std::string filepath = assembleFilepath(persistenceFilename, directory);
        FILE* f = fopen(filepath.c_str(), "wb");
        if (f != nullptr) {
            fwrite(&point.x, sizeof(int), 1, f);
            fwrite(&point.y, sizeof(int), 1, f);
            fclose(f);
            return SaveOperationResult::SavedSuccessfully;
        }
        return SaveOperationResult::Error;
    }

    LoadOperationResult loadIntPoint2D(const std::string& persistenceFilename, Point2D<int>& point, const std::string& directory)
    {
        const std::string filepath = assembleFilepath(persistenceFilename, directory);
        FILE* f = fopen(filepath.c_str(), "rb");
        if (f != nullptr) {
            fread(&point.x, sizeof(int), 1, f);
            fread(&point.y, sizeof(int), 1, f);
            fclose(f);
            return LoadOperationResult::LoadedSuccessfully;
        }
        return LoadOperationResult::Error;
    }
}