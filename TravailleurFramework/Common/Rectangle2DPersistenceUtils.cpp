#include "Rectangle2DPersistenceUtils.h"

#include <cstdio>

namespace Rectangle2DPersistenceUtils
{
    inline std::string assembleFilepath(const std::string& persistenceFilename, const std::string& directory)
    {
        return persistenceFilename != "" ? directory + "\\" + persistenceFilename : persistenceFilename;
    }

    SaveOperationResult saveIntRectangle2D(const std::string& persistenceFilename, const Rectangle2D<int>& rectangle, const std::string& directory)
    {
        const std::string filepath = assembleFilepath(persistenceFilename, directory);
        FILE* f = fopen(filepath.c_str(), "wb");
        if (f != nullptr) {
            fwrite(&rectangle.topLeft.x, sizeof(int), 1, f);
            fwrite(&rectangle.topLeft.y, sizeof(int), 1, f);
            fwrite(&rectangle.size.x, sizeof(int), 1, f);
            fwrite(&rectangle.size.y, sizeof(int), 1, f);
            fclose(f);
            return SaveOperationResult::SavedSuccessfully;
        }
        return SaveOperationResult::Error;
    }

    LoadOperationResult loadIntRectangle2D(const std::string& persistenceFilename, Rectangle2D<int>& rectangle, const std::string& directory)
    {
        const std::string filepath = assembleFilepath(persistenceFilename, directory);
        FILE* f = fopen(filepath.c_str(), "rb");
        if (f != nullptr) {
            fread(&rectangle.topLeft.x, sizeof(int), 1, f);
            fread(&rectangle.topLeft.y, sizeof(int), 1, f);
            fread(&rectangle.size.x, sizeof(int), 1, f);
            fread(&rectangle.size.y, sizeof(int), 1, f);
            fclose(f);
            return LoadOperationResult::LoadedSuccessfully;
        }
        return LoadOperationResult::Error;
    }
}