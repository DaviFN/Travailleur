#include "CStringVector.h"

#include <string>
#include <vector>

CStringVector::CStringVector()
{
    size = 0;
    buffer = nullptr;
}

CStringVector::CStringVector(const std::vector<std::string>& stringVector)
{
    buffer = nullptr;
    setBuffer(stringVector);
}

void CStringVector::print()
{
    if(buffer != nullptr)
    {
        for(int i = 0 ; i < size ; ++i)
        {
            printf("%s\n", buffer[i]);
        }
    }
}

const char* CStringVector::operator[](const int index) const
{
    if(index < size)
    {
        return buffer[index];
    }
    return nullptr;
}

void CStringVector::setBuffer(const std::vector<std::string>& stringVector)
{
    clear();

    size = stringVector.size();
    if(size > 0)
    {
        buffer = new char*[size];
        if(buffer != nullptr)
        {
            for(int i = 0 ; i < size ; ++i)
            {
                buffer[i] = new char[stringVector[i].length() + 1];
                if(buffer[i] != nullptr)
                {
                    strcpy(buffer[i], stringVector[i].c_str());
                }
            }
        }
    }
}

CStringVector::~CStringVector()
{
    clear();
}

void CStringVector::clear()
{
    if(buffer == nullptr)
    {
        return;
    }
    for(int i = 0 ; i < size ; ++i)
    {
        delete buffer[i];
    }
    delete buffer;
    size = 0;
}

int CStringVector::getSize() const
{
    return size;
}

char** CStringVector::getBuffer()
{
    return buffer;
}

std::vector<std::string> CStringVector::getAsVectorOfStrings() const
{
    std::vector<std::string> vectorOfStrings;
    for (int i = 0; i < size; i++) {
        vectorOfStrings.push_back(buffer[i]);
    }
    return vectorOfStrings;
}

int CStringVector::getIndexOf(const char* cString) const
{
    for(int i = 0 ; i < size ; i++)
    {
        if(strcmp(buffer[i], cString) == 0)
        {
            return i;
        }
    }
    return -1;
}

bool CStringVector::hasOption(const char* cString) const
{
    for (int i = 0; i < size; i++)
    {
        if (strcmp(buffer[i], cString) == 0)
        {
            return true;
        }
    }
    return false;
}