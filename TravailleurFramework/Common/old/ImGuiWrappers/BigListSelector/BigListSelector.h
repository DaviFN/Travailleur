#ifndef BIG_LIST_SELECTOR
#define BIG_LIST_SELECTOR

#include <string>
#include <vector>

#include "TravailleurFramework\Common\CStringVector\CStringVector.h"
#include "TravailleurFramework\3rdparty\GUI\ImGui\dear-imgui\imgui.h"

#include "TravailleurFramework\Common\ImGuiWrappers\generic-imgui-wrappers\generic_imgui_wrappers.h"

class BigListSelector
{
    std::string name;

    std::vector<std::string> itemNames;

    int selectedItemIndex = -1;
    int selectedPageItemIndex = -1;

    int previousSelectedItemIndex = -1;

    bool isTreated = false;

    int howManyItemsToShow = -1;
    int numberOfPages = -1;
    int selectedPageIndex = -1;

    void selectFirstPage();
    void selectLastPage();

    std::vector<CStringVector*> pageItems;
    void destroyPageItems();

    bool lastPageDefault = true;

    void setHowManyItemsToShow(int howManyItemsToShow);

    void goToNextPage();
    void goToPreviousPage();

    void updateSelectedPageItemIndex();
    void updateSelectedItemIndex();

    public:
    BigListSelector(std::string name, std::vector<std::string> itemNames, int howManyItemsToShow);
    ~BigListSelector();

    int getSelectedItemIndex();
    std::string getSelectedItem();
    void setSelectedItem(int itemIndex);
    void setSelectedItemToFirst();
    void setSelectedItemToLast();

    bool needsTreatment();
    void informTreated();

    void showGui();

    int getListSize();

};

#endif