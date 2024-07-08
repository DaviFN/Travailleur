#ifndef TreeLikeStructure_H
#define TreeLikeStructure_H

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

#include "TravailleurFramework\Common\common_functions\common_functions.h"

#include "TravailleurFramework\3rdparty\GUI\ImGui\dear-imgui\imgui.h"

class TreeLikeStructure
{
public:
    TreeLikeStructure(std::string treeName, std::vector<std::string> items);
    TreeLikeStructure(std::string treeName, std::string relativeParentDirectory, std::string extension);
    ~TreeLikeStructure();
    std::string getSelectedItem();
    int getSelectedItemIndex();
    void setSelectedItemIndex(int index);
    void setSelectedItemName(const std::string& absolutePath);
    void showGUI();
    void showGUIex();
private:
    #define ELEMENT_TYPE_FOLDER 0
    #define ELEMENT_TYPE_FILE 1
    #define ELEMENT_TYPE_ROOT 2
    struct Element
    {
        int type;
        std::vector<struct Element*> children;
        struct Element* parent;
        std::string absolutePath;
        std::string elementName;
        std::string relativePath;

        int fileIndex;

        Element* getFolderByName(const std::string& folderName);
        void insertFolder(const std::string& folderName);
        Element* insertFile(const std::string& fileName, int index);
        Element* enterFolder(const std::string& folderToEnter);
        bool containsFile(const std::string& fileName);
    };
    int elementsInTree;
    struct Element* treeRoot;
    void deleteElement(Element* element);

    Element* selectedFile;

    void generateTreeStructure(std::vector<std::string>& items);

    std::string name;
    void showFolderGUI(Element* folder);
    void showFileGUI(Element* file);

    int currentId;

    Element* searchItemByIndex(int index);
    Element* findItemInFolderByIndex(Element* folder, int index);
    Element* searchItemByAbsolutePath(const std::string& absolutePath);
    Element* findItemInFolderByAbsolutePath(Element* folder, const std::string& absolutePath);
};

#endif