#ifndef ImGuiCombo_H
#define ImGuiCombo_H

#include <string>
#include <vector>

#include "Common/CStringVector.h"

class ImGuiCombo
{
public:
    ImGuiCombo(const std::string& name, const std::string persistenceFilepath = "");
    ImGuiCombo(const std::string& name, const std::vector<std::string>& options, const std::string persistenceFilepath = "");
    ~ImGuiCombo();

    void setIndexToFirstOptionIfItExists();

    void setName(const std::string& name);
    std::string getName() const;

    void setOptions(const std::vector<std::string>& options);
    std::vector<std::string> getOptions() const;

    bool addOption(const std::string& option);

    bool hasOption(const std::string& option);

    int getSelectedIndex() const;
    void setSelectedIndex(const int selectedIndex);

    std::string getSelectedOption() const;
    void setSelectedOption(const std::string& option);

    size_t nOptions() const;

    bool hasSelectedOption() const;

    void selectFirstOptionIfNoneIsSelected();

    void showGui();    

    void clear();

private:
    void save() const;
    void load();

    std::string name;
    std::string persistencePath;
    CStringVector* optionsAsCStr = nullptr;
    int selectedIndex = -1;
};

#endif