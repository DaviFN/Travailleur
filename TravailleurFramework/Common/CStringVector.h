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
    const char* operator[](const int index) const;
    int getSize() const;
    char** getBuffer();
    std::vector<std::string> getAsVectorOfStrings() const;
    int getIndexOf(const char* cString) const;
    bool hasOption(const char* cString) const;
private:
    int size;
    char** buffer;
};

#endif