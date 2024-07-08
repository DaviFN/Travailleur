#include "CStringVector.h"

#include <vector>
#include <string>
#include <cstring>
#include <iostream>

CStringVector::CStringVector()
{
    size = 0;
    buffer = NULL;
}

CStringVector::CStringVector(const std::vector<std::string>& stringVector)
{
    buffer = NULL;
    setBuffer(stringVector);
}

void CStringVector::print()
{
    if(buffer != NULL)
    {
        for(int i = 0 ; i < size ; i++)
        {
            printf("%s\n", buffer[i]);
        }
    }
}

char* CStringVector::operator[](int index)
{
    if(index < size)
    {
        return buffer[index];
    }
    return NULL;
}

void CStringVector::setBuffer(const std::vector<std::string>& stringVector)
{
    
    clear();

    size = stringVector.size();
    if(size > 0)
    {
        buffer = (char**)malloc(sizeof(char*) * size);
        if(buffer != NULL)
        {
            for(int i = 0 ; i < size ; i++)
            {
                buffer[i] = (char*)malloc(sizeof(char) * (stringVector[i].length() + 1));
                if(buffer[i] != NULL)
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
    if(buffer == NULL)
    {
        return;
    }
    for(int i = 0 ; i < size ; i++)
    {
        free(buffer[i]);
    }
    free(buffer);
    size = 0;
}

int CStringVector::getSize()
{
    return size;
}

char** CStringVector::getBuffer()
{
    return buffer;
}

int CStringVector::getIndexOf(char* cstring)
{
    for(int i = 0 ; i < size ; i++)
    {
        if(strcmp(buffer[i], cstring) == 0)
        {
            return i;
        }
    }
    return -1;
}