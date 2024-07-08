#ifndef SimplePersistenceUtils_H
#define SimplePersistenceUtils_H

#include <cstdio>
#include <map>
#include <stdint.h>
#include <string>
#include <vector>

enum class LoadOperationResult
{
    LoadedSuccessfully,
    Error
};

enum class SaveOperationResult
{
    SavedSuccessfully,
    Error
};

namespace SimplePersistenceUtils
{
    SaveOperationResult saveString(const std::string& persistenceFilename, const std::string& string, const std::string& directory = "");
    LoadOperationResult loadString(const std::string& persistenceFilename, std::string& string, const std::string& directory = "");

    SaveOperationResult saveBool(const std::string& persistenceFilename, const bool boolean, const std::string& directory = "");
    LoadOperationResult loadBool(const std::string& persistenceFilename, bool& boolean, const std::string& directory = "");

    SaveOperationResult saveInt64(const std::string& persistenceFilename, const int64_t integer, const std::string& directory = "");
    LoadOperationResult loadInt64(const std::string& persistenceFilename, int64_t& integer, const std::string& directory = "");

    SaveOperationResult saveInt32(const std::string& persistenceFilename, const int32_t integer, const std::string& directory = "");
    LoadOperationResult loadInt32(const std::string& persistenceFilename, int32_t& integer, const std::string& directory = "");

    SaveOperationResult saveInt8(const std::string& persistenceFilename, const int8_t integer, const std::string& directory = "");
    LoadOperationResult loadInt8(const std::string& persistenceFilename, int8_t& integer, const std::string& directory = "");

    SaveOperationResult saveUint64(const std::string& persistenceFilename, const uint64_t integer, const std::string& directory = "");
    LoadOperationResult loadUint64(const std::string& persistenceFilename, uint64_t& integer, const std::string& directory = "");

    SaveOperationResult saveUint32(const std::string& persistenceFilename, const uint32_t integer, const std::string& directory = "");
    LoadOperationResult loadUint32(const std::string& persistenceFilename, uint32_t& integer, const std::string& directory = "");

    SaveOperationResult saveUint8(const std::string& persistenceFilename, const uint8_t integer, const std::string& directory = "");
    LoadOperationResult loadUint8(const std::string& persistenceFilename, uint8_t& integer, const std::string& directory = "");

    SaveOperationResult saveFloat(const std::string& persistenceFilename, const float floatingPointValue, const std::string& directory = "");
    LoadOperationResult loadFloat(const std::string& persistenceFilename, float& floatingPointValue, const std::string& directory = "");

    SaveOperationResult saveDouble(const std::string& persistenceFilename, const double doublePrecisionValue, const std::string& directory = "");
    LoadOperationResult loadDouble(const std::string& persistenceFilename, double& doublePrecisionValue, const std::string& directory = "");

    SaveOperationResult saveVectorOfStrings(const std::string& persistenceFilename, const std::vector<std::string>& vectorOfStrings, const std::string& directory = "");
    LoadOperationResult loadVectorOfStrings(const std::string& persistenceFilename, std::vector<std::string>& vectorOfStrings, const std::string& directory = "");

    SaveOperationResult saveVectorOfBools(const std::string& persistenceFilename, const std::vector<bool>& vectorOfBools, const std::string& directory = "");
    LoadOperationResult loadVectorOfBools(const std::string& persistenceFilename, std::vector<bool>& vectorOfBools, const std::string& directory = "");

    SaveOperationResult saveVectorOfInt64(const std::string& persistenceFilename, const std::vector<int64_t>& vectorOfInt64, const std::string& directory = "");
    LoadOperationResult loadVectorOfInt64(const std::string& persistenceFilename, std::vector<int64_t>& vectorOfInt64, const std::string& directory = "");

    SaveOperationResult saveVectorOfInt32(const std::string& persistenceFilename, const std::vector<int32_t>& vectorOfInt32, const std::string& directory = "");
    LoadOperationResult loadVectorOfInt32(const std::string& persistenceFilename, std::vector<int32_t>& vectorOfInt32, const std::string& directory = "");

    SaveOperationResult saveVectorOfInt8(const std::string& persistenceFilename, const std::vector<int8_t>& vectorOfInt8, const std::string& directory = "");
    LoadOperationResult loadVectorOfInt8(const std::string& persistenceFilename, std::vector<int8_t>& vectorOfInt8, const std::string& directory = "");

    SaveOperationResult saveVectorOfUint64(const std::string& persistenceFilename, const std::vector<uint64_t>& vectorOfUint64, const std::string& directory = "");
    LoadOperationResult loadVectorOfUint64(const std::string& persistenceFilename, std::vector<uint64_t>& vectorOfUint64, const std::string& directory = "");

    SaveOperationResult saveVectorOfUint32(const std::string& persistenceFilename, const std::vector<uint32_t>& vectorOfUint32, const std::string& directory = "");
    LoadOperationResult loadVectorOfUint32(const std::string& persistenceFilename, std::vector<uint32_t>& vectorOfUint32, const std::string& directory = "");

    SaveOperationResult saveVectorOfUint8(const std::string& persistenceFilename, const std::vector<uint8_t>& vectorOfUint8, const std::string& directory = "");
    LoadOperationResult loadVectorOfUint8(const std::string& persistenceFilename, std::vector<uint8_t>& vectorOfUint8, const std::string& directory = "");

    SaveOperationResult saveVectorOfFloats(const std::string& persistenceFilename, const std::vector<float>& vectorOfFloats, const std::string& directory = "");
    LoadOperationResult loadVectorOfFloats(const std::string& persistenceFilename, std::vector<float>& vectorOfFloats, const std::string& directory = "");

    SaveOperationResult saveVectorOfDoubles(const std::string& persistenceFilename, const std::vector<double>& vectorOfDoubles, const std::string& directory = "");
    LoadOperationResult loadVectorOfDoubles(const std::string& persistenceFilename, std::vector<double>& vectorOfDoubles, const std::string& directory = "");

    SaveOperationResult saveMapFromStringToString(const std::string& persistenceFilename, const std::map<std::string, std::string>& map, const std::string& directory = "");
    LoadOperationResult loadMapFromStringToString(const std::string& persistenceFilename, std::map<std::string, std::string>& map, const std::string& directory = "");

    template<typename T>
    void saveVector(FILE* f, const std::vector<T>& vector)
    {
        if (f != nullptr) {
            const size_t vectorSize = vector.size();
            fwrite(&vectorSize, sizeof(size_t), 1, f);
            for (const T& vectorItem : vector) {
                fwrite(&vectorItem, sizeof(T), 1, f);
            }
        }
    }

    template<typename T>
    bool loadVector(FILE* f, std::vector<T>& vector)
    {
        vector.clear();
        if (f == nullptr) return false;
        size_t vectorSize = 0;
        fread(&vectorSize, sizeof(size_t), 1, f);
        for (size_t i = 0; i < vectorSize; ++i) {
            T vectorItem;
            fread(&vectorItem, sizeof(T), 1, f);
            vector.push_back(vectorItem);
        }
        return true;
    }
}

#endif