#include "BigListSelector.h"

BigListSelector::BigListSelector(std::string name, std::vector<std::string> itemNames, int howManyItemsToShow)
{

    this->name = name;
    this->itemNames = itemNames;

    if(howManyItemsToShow > 0)
    {
        this->setHowManyItemsToShow(howManyItemsToShow);
    }
    else
    {
        this->setHowManyItemsToShow(100);
    }

}

BigListSelector::~BigListSelector()
{
    this->destroyPageItems();
}

int BigListSelector::getSelectedItemIndex()
{
    return this->selectedItemIndex;
}

std::string BigListSelector::getSelectedItem()
{
    if(this->selectedItemIndex == -1)
    {
        return "[NO SELECTED ITEM]";
    }
    return itemNames[selectedItemIndex];
}

void BigListSelector::setSelectedItem(int itemIndex)
{
    if(itemIndex < 0 || itemIndex >= this->itemNames.size())
    {
        printf("BigListSelector %s: could not set selected item to %d (listSize: %d)\n", this->name.c_str(), itemIndex, this->getListSize());
        return;
    }
    this->selectedItemIndex = itemIndex;
}

void BigListSelector::setSelectedItemToFirst()
{
    if(itemNames.empty())
    {
        printf("BigListSelector %s: could not set selected item to first because there are no items\n", this->name.c_str());
        return;
    }
    this->selectedItemIndex = 0;
}

void BigListSelector::setSelectedItemToLast()
{
    if(itemNames.empty())
    {
        printf("BigListSelector %s: could not set selected item to last because there are no items\n", this->name.c_str());
        return;
    }
    this->selectedItemIndex = itemNames.size() - 1;
}

bool BigListSelector::needsTreatment()
{
    return this->isTreated;
}
void BigListSelector::informTreated()
{
    this->isTreated = true;
}

int BigListSelector::getListSize()
{
    int listSize = this->itemNames.size();
    return listSize;
}

void BigListSelector::setHowManyItemsToShow(int howManyItemsToShow)
{
    if(howManyItemsToShow <= 0)
    {
        return;
    }

    this->howManyItemsToShow = howManyItemsToShow;

    int itemNamesSize = this->itemNames.size();
    
    // the number of pages is going to be the floor of itemNames.size() divided by howManyItemsToShow
    this->numberOfPages = itemNamesSize / howManyItemsToShow;
    if(itemNamesSize % howManyItemsToShow != 0)
    {
        // one more page if not divisible
        this->numberOfPages++;
    }






    this->destroyPageItems();

    for(int i = 0 ; i < this->numberOfPages ; i++)
    {
        std::vector<std::string> itemsForPage;

        int indexOfFirstItemOfPage = i * howManyItemsToShow;
        int indexOfLastItemOfPage = indexOfFirstItemOfPage + howManyItemsToShow - 1;
        if(indexOfLastItemOfPage >= itemNamesSize)
        {
            indexOfLastItemOfPage = itemNamesSize - 1;
        }

        for(int j = indexOfFirstItemOfPage ; j <= indexOfLastItemOfPage ; j++)
        {
            std::string itemNameToPush = itemNames[j];
            itemNameToPush += " (";
            itemNameToPush += std::to_string(j+1);
            itemNameToPush += ")";
            itemsForPage.push_back(itemNameToPush);
        }

        pageItems.push_back(new CStringVector(itemsForPage));

    }







    if(this->lastPageDefault)
    {
        this->selectLastPage();
    }
    else
    {
        this->selectFirstPage();
    }

    updateSelectedPageItemIndex();

}

void BigListSelector::destroyPageItems()
{
    int pageItemsSize = this->pageItems.size();
    for(int i = 0 ; i < pageItemsSize ; i++)
    {
        if(pageItems[i] != NULL)
        {
            delete pageItems[i];
        }
    }
    pageItems.clear();
}

void BigListSelector::selectFirstPage()
{
    if(itemNames.empty())
    {
        return;
    }
    this->selectedPageIndex = 0;

    updateSelectedPageItemIndex();

}

void BigListSelector::selectLastPage()
{
    if(itemNames.empty())
    {
        return;
    }
    if(this->numberOfPages <= 0)
    {
        return;
    }
    this->selectedPageIndex = this->numberOfPages - 1;

    updateSelectedPageItemIndex();

}

void BigListSelector::goToNextPage()
{
    if(this->selectedPageIndex < this->numberOfPages - 1)
    {
        this->selectedPageIndex++;
    }

    updateSelectedPageItemIndex();

}

void BigListSelector::goToPreviousPage()
{
    if(this->selectedPageIndex > 0)
    {
        this->selectedPageIndex--;
    }

    updateSelectedPageItemIndex();

}

// updates selectedPageItemIndex based on selectedItemIndex
void BigListSelector::updateSelectedPageItemIndex()
{
    int itemNamesSize= itemNames.size();
    int indexOfFirstItemOfPage = this->selectedPageIndex * howManyItemsToShow;
    int indexOfLastItemOfPage = indexOfFirstItemOfPage + howManyItemsToShow - 1;
    if(indexOfLastItemOfPage >= itemNamesSize)
    {
        indexOfLastItemOfPage = itemNamesSize - 1;
    }

    if(this->selectedItemIndex < indexOfFirstItemOfPage || this->selectedItemIndex > indexOfLastItemOfPage)
    {
        this->selectedPageItemIndex = -1;
        return;
    }

    this->selectedPageItemIndex = this->selectedItemIndex % this->howManyItemsToShow;
}

// updates selectedItemIndex based on selectedPageItemIndex
void BigListSelector::updateSelectedItemIndex()
{
    if(this->selectedPageItemIndex < 0)
    {
        return;
    }

    this->selectedItemIndex = this->selectedPageIndex * this->howManyItemsToShow + this->selectedPageItemIndex;

    //printf("selectedPageIndex: %d\n", selectedPageIndex);
    //printf("howManyItemsToShow: %d\n", howManyItemsToShow);
    //printf("selectedPageItemIndex: %d\n", selectedPageItemIndex);
    //printf("this->selectedItemIndex updated to %d\n", this->selectedItemIndex);

}

void BigListSelector::showGui()
{
    
    char aux[2048];

    //for(int i = 0 ; i < itemNames.size() ; i++)
    //{
    //    printf("itemNames[%d] = %s\n", i, itemNames[i].c_str());
    //}

    sprintf(aux, "BigListSelector: %s", name.c_str());

    if(ImGui::TreeNode(aux))
    {


        //for(int i = 0; i < itemNames.size() ; i++)
        //{
        //    char buf[2048];
        //    sprintf(buf, "%s (%d)", itemNames[i].c_str(), i + 1);
        //    //printf("buf: %s\n", buf);
        //    //sprintf(buf, "%d", n);
        //    //if (ImGui::Selectable(buf, selected == n))
        //    if(ImGui::Selectable(buf, selectedItemIndex == i))
        //    {
        //        selectedItemIndex = i;
        //    }
        //}

        //sprintf(aux, "page %d of %s\n", this->selectedPage, this->name.c_str());
        //ImGui::Combo(aux, &selectedItemIndex, pageItems[selectedPage]->getBuffer(), pageItems[selectedPage]->getSize());


        // List box
        //const char* items[] = { "Apple", "Banana", "Cherry", "Kiwi", "Mango", "Orange", "Pineapple", "Strawberry", "Watermelon" };
        //static int item_current = 1;
        sprintf(aux, "page %d of %s\n", this->selectedPageIndex + 1, this->name.c_str());
        
        //ImGui::ListBox(aux, &selectedItemIndex, pageItems[selectedPage]->getBuffer(), IM_ARRAYSIZE(pageItems[selectedPage]->getBuffer()), 10);
        
        // wrapped
        //ImGui::ListBox(aux, &selectedPageItemIndex, pageItems[selectedPageIndex]->getBuffer(), pageItems[selectedPageIndex]->getSize(), 10);
        
        ImGuiListBox(aux, &selectedPageItemIndex, pageItems[selectedPageIndex]->getBuffer(), pageItems[selectedPageIndex]->getSize(), 10);

        this->updateSelectedItemIndex();
        
        sprintf(aux, "next page (%s)", this->name.c_str());
        if(ImGui::Button(aux))
        {
            this->goToNextPage();
        }

        sprintf(aux, "previous page (%s)", this->name.c_str());
        if(ImGui::Button(aux))
        {
            this->goToPreviousPage();
        }

        ImGui::TreePop();
    }

    if(selectedItemIndex >= 0)
    {
        sprintf(aux, "selected: %s (%d)", itemNames[selectedItemIndex].c_str(), selectedItemIndex + 1);
        ImGui::TextWrapped(aux);
    }
    else
    {
        sprintf(aux, "selected: none");
        ImGui::TextWrapped(aux);
    }

    sprintf(aux, "page %d of %d", this->selectedPageIndex + 1, this->numberOfPages);
    ImGui::TextWrapped(aux);

    
    if(selectedItemIndex != previousSelectedItemIndex)
    {
        this->isTreated = false;
        previousSelectedItemIndex = selectedItemIndex;
    }


}