#include "StringVectorShower.h"

StringVectorShower::StringVectorShower(const std::string& name)
{
    this->name = name;
    cStringVector = new CStringVector();
}

StringVectorShower::StringVectorShower(const std::string& name, int howManyItemsToShow)
{
    this->name = name;
    this->howManyItemsToShow = howManyItemsToShow;
    cStringVector = new CStringVector();
}

StringVectorShower::~StringVectorShower()
{
    if(cStringVector != NULL)
    {
        delete cStringVector;
    }
}

void StringVectorShower::setStringVector(const std::vector<std::string>& strVector)
{
    //printf("StringVectorShower::setStringVector called\n");
    //for(int i = 0 ; i < strVector.size() ; i++)
    //{
    //    printf("strVector[%d]: %s\n", i, strVector[i].c_str());
    //}

    if(cStringVector != NULL)
    {
        delete cStringVector;
    }
    cStringVector = new CStringVector(strVector);
}

void StringVectorShower::showGui()
{
    // List box
    //const char* items[] = { "Apple", "Banana", "Cherry", "Kiwi", "Mango", "Orange", "Pineapple", "Strawberry", "Watermelon" };
    int dummyCurrentIndex = -1;
    //ImGui::ListBox("listbox\n(single select)", &dummyCurrentIndex, items, IM_ARRAYSIZE(items), 4);
    char aux[2048];
    sprintf(aux, "StringVectorShower: %s (%d items)", this->name.c_str(), this->cStringVector->getSize());
    ImGui::ListBox(aux, &dummyCurrentIndex, this->cStringVector->getBuffer(), this->cStringVector->getSize(), this->howManyItemsToShow);
    sprintf(aux, "%s (%d items)", this->name.c_str(), this->cStringVector->getSize());
    ImGui::TextWrapped(aux);
}