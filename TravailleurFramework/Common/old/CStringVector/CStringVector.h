#ifndef CStringVector_H
#define CStringVector_H

#include <vector>
#include <string>

class CStringVector
{
public:
    CStringVector();
    CStringVector(const std::vector<std::string>& stringVector);
    ~CStringVector();
    void print();
    void clear();
    void setBuffer(const std::vector<std::string>& stringVector);
    char* operator[](int index);
    int getSize();
    char** getBuffer();
    int getIndexOf(char* cstring);
private:
    int size;
    char** buffer;
};

#endif