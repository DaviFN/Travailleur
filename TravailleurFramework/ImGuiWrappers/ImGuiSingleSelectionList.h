#ifndef ImGuiSingleSelectionList_H
#define ImGuiSingleSelectionList_H

#include <string>
#include <vector>

#include "Common/CStringVector.h"

class ImGuiSingleSelectionList
{
public:
    ImGuiSingleSelectionList(const std::string& name);
    ImGuiSingleSelectionList(const std::string& name, const std::vector<std::string>& options);
    ~ImGuiSingleSelectionList();

    void setSelectionToNone();
    void setIndexToFirstOptionIfItExists();

    void setName(const std::string& name);
    std::string getName() const;

    void setOptions(const std::vector<std::string>& options);
    std::vector<std::string> getOptions() const;

    bool addOption(const std::string& option);

    bool hasOption(const std::string& option);

    bool hasSelectedOption() const;

    int getSelectedIndex() const;
    void setSelectedIndex(const int selectedIndex);

    std::string getSelectedOption() const;
    void setSelectedOption(const std::string& option);

    void showGui();

    void setHowManyItemsToShow(const size_t howManyItemsToShow);

    bool changedButNotMarkedAsDealtWith() const;

    void markAsDealtWithChange();

    int nOptions() const;

    void clear();

    bool hasOptionWithIndex(const int index) const;

    int getPreviouslySelectedIndex() const;

private:
    std::string name;
    CStringVector* optionsAsCStr = nullptr;
    int selectedIndex = -1;
    int lastSelectedIndex = -1;
    size_t howManyItemsToShow = 5;
    bool changed = false;
    bool changeDealtWith = false;
    int previouslySelectedIndex = -1;
};

#endif