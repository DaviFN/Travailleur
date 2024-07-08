#include "SimplePersistenceUtils.h"

#include <cstdio>
#include <fstream>

namespace SimplePersistenceUtils {

	inline std::string assembleFilepath(const std::string& persistenceFilename, const std::string& directory)
	{
		return persistenceFilename != "" ? directory + "\\" + persistenceFilename : persistenceFilename;
	}

	template<typename type>
	SaveOperationResult saveDirectlyStreamableType(const std::string& persistenceFilename, const type& directlySavable, const std::string& directory)
	{
		try {
			std::ofstream fStream;
			const std::string filepath = assembleFilepath(persistenceFilename, directory);
			fStream.open(filepath, std::ios_base::binary | std::ios_base::trunc);
			if (fStream.good())
			{
				fStream.write((char*)&directlySavable, sizeof(type));
				return SaveOperationResult::SavedSuccessfully;
			}
		}
		catch (...)
		{
			return SaveOperationResult::Error;
		}
		return SaveOperationResult::Error;
	}

	template<typename type>
	LoadOperationResult loadDirectlyStreamableType(const std::string& persistenceFilename, type& directlyLoadable, const std::string& directory)
	{
		try {
			std::ifstream fStream;
			const std::string filepath = assembleFilepath(persistenceFilename, directory);
			fStream.open(filepath, std::ios_base::binary);
			if (fStream.good())
			{
				fStream.read((char*)&directlyLoadable, sizeof(type));
				return LoadOperationResult::LoadedSuccessfully;
			}
		}
		catch (...)
		{
			return LoadOperationResult::Error;
		}
		return LoadOperationResult::Error;
	}

	template<typename type>
	SaveOperationResult saveVectorOfDirectlyStreamableType(const std::string& persistenceFilename, const std::vector<type>& vectorOfDirectlySavable, const std::string& directory)
	{
		try {
			std::ofstream fStream;
			const std::string filepath = assembleFilepath(persistenceFilename, directory);
			fStream.open(filepath, std::ios_base::binary | std::ios_base::trunc);
			if (fStream.good())
			{
				const size_t size = vectorOfDirectlySavable.size();
				fStream.write((char*)&size, sizeof(size_t));
				for (const type& directlySavable : vectorOfDirectlySavable) {
					fStream.write((char*)&directlySavable, sizeof(type));
				}
				return SaveOperationResult::SavedSuccessfully;
			}
		}
		catch (...)
		{
			return SaveOperationResult::Error;
		}
		return SaveOperationResult::Error;
	}

	template<typename type>
	LoadOperationResult loadVectorOfDirectlyStreamableType(const std::string& persistenceFilename, std::vector<type>& vectorOfDirectlySavable, const std::string& directory)
	{
		vectorOfDirectlySavable.clear();
		try {
			std::ifstream fStream;
			const std::string filepath = assembleFilepath(persistenceFilename, directory);
			fStream.open(filepath, std::ios_base::binary);
			if (fStream.good())
			{
				size_t howManyItems = 0;
				fStream.read((char*)&howManyItems, sizeof(size_t));
				vectorOfDirectlySavable.reserve(howManyItems);
				for (size_t i = 0; i < howManyItems; ++i) {
					type directlyLoadable = 0;
					fStream.read((char*)&directlyLoadable, sizeof(type));
					vectorOfDirectlySavable.push_back(directlyLoadable);
				}
				return LoadOperationResult::LoadedSuccessfully;
			}
		}
		catch (...)
		{
			return LoadOperationResult::Error;
		}
		return LoadOperationResult::Error;
	}

	template<typename type1, typename type2>
	SaveOperationResult saveMapFromDirectlyStreamableTypeToDirectlyStreamableType(const std::string& persistenceFilename, const std::map<type1, type2>& map, const std::string& directory)
	{
		try {
			std::ofstream fStream;
			const std::string filepath = assembleFilepath(persistenceFilename, directory);
			fStream.open(filepath, std::ios_base::binary | std::ios_base::trunc);
			if (fStream.good())
			{
				fStream << map.size();
				for (const auto& mapItem : map) {
					fStream.write((char*)&mapItem.first, sizeof(type1));
					fStream.write((char*)&mapItem.second, sizeof(type2));
				}
				return SaveOperationResult::SavedSuccessfully;
			}
		}
		catch (...)
		{
			return SaveOperationResult::Error;
		}
		return SaveOperationResult::Error;
	}

	template<typename type1, typename type2>
	LoadOperationResult loadMapFromDirectlyStreamableTypeToDirectlyStreamableType(const std::string& persistenceFilename, std::map<type1, type2>& map, const std::string& directory)
	{
		map.clear();
		try {
			std::ifstream fStream;
			const std::string filepath = assembleFilepath(persistenceFilename, directory);
			fStream.open(filepath, std::ios_base::binary);
			if (fStream.good())
			{
				size_t howManyItems;
				fStream >> howManyItems;
				for (size_t i = 0; i < howManyItems; ++i) {
					type1 key;
					fStream.read((char*)&key, sizeof(type1));
					type2 value;
					fStream.read((char*)&value, sizeof(type2));
					map[key] = value;
				}
				return LoadOperationResult::LoadedSuccessfully;
			}
		}
		catch (...)
		{
			return LoadOperationResult::Error;
		}
		return LoadOperationResult::Error;
	}

	void saveStringToFile(FILE* f, const std::string& string)
	{
		const size_t strSize = string.size();
		fwrite(&strSize, sizeof(size_t), 1, f);
		if (string.size() > 0) {
			fwrite(string.c_str(), sizeof(char), strSize, f);
		}
	}

	void loadStringFromFile(FILE* f, std::string& string)
	{
		size_t strSize;
		fread(&strSize, sizeof(size_t), 1, f);
		if (strSize > 0) {
			char* cStrBuffer = new char[strSize + 1];
			fread(cStrBuffer, sizeof(char), strSize, f);
			cStrBuffer[strSize] = 0;
			string = cStrBuffer;
			delete[] cStrBuffer;
		}
		else {
			string = "";
		}
	}

	SaveOperationResult saveString(const std::string& persistenceFilename, const std::string& string, const std::string& directory)
	{
		const std::string filepath = assembleFilepath(persistenceFilename, directory);
		FILE* f = fopen(filepath.c_str(), "wb");
		if (f != nullptr) {
			saveStringToFile(f, string);
			fclose(f);
			return SaveOperationResult::SavedSuccessfully;
		}
		return SaveOperationResult::Error;
	}

	LoadOperationResult loadString(const std::string& persistenceFilename, std::string& string, const std::string& directory)
	{
		const std::string filepath = assembleFilepath(persistenceFilename, directory);
		FILE* f = fopen(filepath.c_str(), "rb");
		if (f != nullptr) {
			loadStringFromFile(f, string);
			fclose(f);
			return LoadOperationResult::LoadedSuccessfully;
		}
		return LoadOperationResult::Error;
	}

	SaveOperationResult saveBool(const std::string& persistenceFilename, const bool boolean, const std::string& directory)
	{
		return saveDirectlyStreamableType(persistenceFilename, boolean, directory);
	}

	LoadOperationResult loadBool(const std::string& persistenceFilename, bool& boolean, const std::string& directory)
	{
		return loadDirectlyStreamableType(persistenceFilename, boolean, directory);
	}

	SaveOperationResult saveInt64(const std::string& persistenceFilename, const int64_t integer, const std::string& directory)
	{
		return saveDirectlyStreamableType(persistenceFilename, integer, directory);
	}

	LoadOperationResult loadInt64(const std::string& persistenceFilename, int64_t& integer, const std::string& directory)
	{
		return loadDirectlyStreamableType(persistenceFilename, integer, directory);
	}

	SaveOperationResult saveInt32(const std::string& persistenceFilename, const int32_t integer, const std::string& directory)
	{
		return saveDirectlyStreamableType(persistenceFilename, integer, directory);
	}

	LoadOperationResult loadInt32(const std::string& persistenceFilename, int32_t& integer, const std::string& directory)
	{
		return loadDirectlyStreamableType(persistenceFilename, integer, directory);
	}

	SaveOperationResult saveInt8(const std::string& persistenceFilename, const int8_t integer, const std::string& directory)
	{
		return saveDirectlyStreamableType(persistenceFilename, integer, directory);
	}

	LoadOperationResult loadInt8(const std::string& persistenceFilename, int8_t& integer, const std::string& directory)
	{
		return loadDirectlyStreamableType(persistenceFilename, integer, directory);
	}

	SaveOperationResult saveUint64(const std::string& persistenceFilename, const uint64_t integer, const std::string& directory)
	{
		return saveDirectlyStreamableType(persistenceFilename, integer, directory);
	}

	LoadOperationResult loadUint64(const std::string& persistenceFilename, uint64_t& integer, const std::string& directory)
	{
		return loadDirectlyStreamableType(persistenceFilename, integer, directory);
	}

	SaveOperationResult saveUint32(const std::string& persistenceFilename, const uint32_t integer, const std::string& directory)
	{
		return saveDirectlyStreamableType(persistenceFilename, integer, directory);
	}

	LoadOperationResult loadUint32(const std::string& persistenceFilename, uint32_t& integer, const std::string& directory)
	{
		return loadDirectlyStreamableType(persistenceFilename, integer, directory);
	}

	SaveOperationResult saveUint8(const std::string& persistenceFilename, const uint8_t integer, const std::string& directory)
	{
		return saveDirectlyStreamableType(persistenceFilename, integer, directory);
	}

	LoadOperationResult loadUint8(const std::string& persistenceFilename, uint8_t& integer, const std::string& directory)
	{
		return loadDirectlyStreamableType(persistenceFilename, integer, directory);
	}

	SaveOperationResult saveFloat(const std::string& persistenceFilename, const float floatingPointValue, const std::string& directory)
	{
		return saveDirectlyStreamableType(persistenceFilename, floatingPointValue, directory);
	}

	LoadOperationResult loadFloat(const std::string& persistenceFilename, float& floatingPointValue, const std::string& directory)
	{
		return loadDirectlyStreamableType(persistenceFilename, floatingPointValue, directory);
	}

	SaveOperationResult saveDouble(const std::string& persistenceFilename, const double doublePrecisionValue, const std::string& directory)
	{
		return saveDirectlyStreamableType(persistenceFilename, doublePrecisionValue, directory);
	}

	LoadOperationResult loadDouble(const std::string& persistenceFilename, double& doublePrecisionValue, const std::string& directory)
	{
		return loadDirectlyStreamableType(persistenceFilename, doublePrecisionValue, directory);
	}

	SaveOperationResult saveVectorOfStrings(const std::string& persistenceFilename, const std::vector<std::string>& vectorOfStrings, const std::string& directory)
	{
		return saveVectorOfDirectlyStreamableType(persistenceFilename, vectorOfStrings, directory);
	}

	LoadOperationResult loadVectorOfStrings(const std::string& persistenceFilename, std::vector<std::string>& vectorOfStrings, const std::string& directory)
	{
		return loadVectorOfDirectlyStreamableType(persistenceFilename, vectorOfStrings, directory);
	}

	SaveOperationResult saveVectorOfBools(const std::string& persistenceFilename, const std::vector<bool>& vectorOfBools, const std::string& directory)
	{
		return saveVectorOfDirectlyStreamableType(persistenceFilename, vectorOfBools, directory);
	}

	LoadOperationResult loadVectorOfBools(const std::string& persistenceFilename, std::vector<bool>& vectorOfBools, const std::string& directory)
	{
		return LoadOperationResult::Error;
	}

	SaveOperationResult saveVectorOfInt64(const std::string& persistenceFilename, const std::vector<int64_t>& vectorOfInt64, const std::string& directory)
	{
		return saveVectorOfDirectlyStreamableType(persistenceFilename, vectorOfInt64, directory);
	}

	LoadOperationResult loadVectorOfInt64(const std::string& persistenceFilename, std::vector<int64_t>& vectorOfInt64, const std::string& directory)
	{
		return loadVectorOfDirectlyStreamableType(persistenceFilename, vectorOfInt64, directory);
	}

	SaveOperationResult saveVectorOfInt32(const std::string& persistenceFilename, const std::vector<int32_t>& vectorOfInt32, const std::string& directory)
	{
		return saveVectorOfDirectlyStreamableType(persistenceFilename, vectorOfInt32, directory);
	}

	LoadOperationResult loadVectorOfInt32(const std::string& persistenceFilename, std::vector<int32_t>& vectorOfInt32, const std::string& directory)
	{
		return loadVectorOfDirectlyStreamableType(persistenceFilename, vectorOfInt32, directory);
	}

	SaveOperationResult saveVectorOfInt8(const std::string& persistenceFilename, const std::vector<int8_t>& vectorOfInt8, const std::string& directory)
	{
		return saveVectorOfDirectlyStreamableType(persistenceFilename, vectorOfInt8, directory);
	}

	LoadOperationResult loadVectorOfInt8(const std::string& persistenceFilename, std::vector<int8_t>& vectorOfInt8, const std::string& directory)
	{
		return loadVectorOfDirectlyStreamableType(persistenceFilename, vectorOfInt8, directory);
	}

	SaveOperationResult saveVectorOfUint64(const std::string& persistenceFilename, const std::vector<uint64_t>& vectorOfUint64, const std::string& directory)
	{
		return saveVectorOfDirectlyStreamableType(persistenceFilename, vectorOfUint64, directory);
	}

	LoadOperationResult loadVectorOfUint64(const std::string& persistenceFilename, std::vector<uint64_t>& vectorOfUint64, const std::string& directory)
	{
		return loadVectorOfDirectlyStreamableType(persistenceFilename, vectorOfUint64, directory);
	}

	SaveOperationResult saveVectorOfUint32(const std::string& persistenceFilename, const std::vector<uint32_t>& vectorOfUint32, const std::string& directory)
	{
		return saveVectorOfDirectlyStreamableType(persistenceFilename, vectorOfUint32, directory);
	}

	LoadOperationResult loadVectorOfUint32(const std::string& persistenceFilename, std::vector<uint32_t>& vectorOfUint32, const std::string& directory)
	{
		return loadVectorOfDirectlyStreamableType(persistenceFilename, vectorOfUint32, directory);
	}

	SaveOperationResult saveVectorOfUint8(const std::string& persistenceFilename, const std::vector<uint8_t>& vectorOfUint8, const std::string& directory)
	{
		return saveVectorOfDirectlyStreamableType(persistenceFilename, vectorOfUint8, directory);
	}

	LoadOperationResult loadVectorOfUint8(const std::string& persistenceFilename, std::vector<uint8_t>& vectorOfUint8, const std::string& directory)
	{
		return loadVectorOfDirectlyStreamableType(persistenceFilename, vectorOfUint8, directory);
	}

	SaveOperationResult saveVectorOfFloats(const std::string& persistenceFilename, const std::vector<float>& vectorOfFloats, const std::string& directory)
	{
		return saveVectorOfDirectlyStreamableType(persistenceFilename, vectorOfFloats, directory);
	}

	LoadOperationResult loadVectorOfFloats(const std::string& persistenceFilename, std::vector<float>& vectorOfFloats, const std::string& directory)
	{
		return loadVectorOfDirectlyStreamableType(persistenceFilename, vectorOfFloats, directory);
	}

	SaveOperationResult saveVectorOfDoubles(const std::string& persistenceFilename, const std::vector<double>& vectorOfDoubles, const std::string& directory)
	{
		return saveVectorOfDirectlyStreamableType(persistenceFilename, vectorOfDoubles, directory);
	}

	LoadOperationResult loadVectorOfDoubles(const std::string& persistenceFilename, std::vector<double>& vectorOfDoubles, const std::string& directory)
	{
		return loadVectorOfDirectlyStreamableType(persistenceFilename, vectorOfDoubles, directory);
	}

}