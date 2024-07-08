#ifndef STRING_VECTOR_SHOWER_H
#define STRING_VECTOR_SHOWER_H

#include "TravailleurFramework\3rdparty\GUI\ImGui\dear-imgui\imgui.h"
#include "TravailleurFramework\Common\CStringVector\CStringVector.h"

class StringVectorShower
{
    std::string name;
    CStringVector* cStringVector = NULL;
    int howManyItemsToShow = 5;
    public:
    StringVectorShower(const std::string& name);
    StringVectorShower(const std::string& name, int howManyItemsToShow);
    ~StringVectorShower();
    void setStringVector(const std::vector<std::string>& strVector);
    void showGui();
};

#endif