#include "TreeLikeStructure.h"

TreeLikeStructure::TreeLikeStructure(std::string treeName, std::vector<std::string> items)
{
    this->name = treeName;
    this->elementsInTree = 0;

    // sort the strings
    //std::sort(items.begin(), items.end(), [](std::string a, std::string b){return a < b;});


    generateTreeStructure(items);
}

TreeLikeStructure::TreeLikeStructure(std::string treeName, std::string relativeParentDirectory, std::string extension)
{
    this->name = treeName;
    this->elementsInTree = 0;

    std::vector<std::string> items;
    items = list_filepaths(relativeParentDirectory, extension);

    // sort the strings
    //std::sort(items.begin(), items.end(), [](std::string a, std::string b){return a < b;});

    generateTreeStructure(items);

}

TreeLikeStructure::~TreeLikeStructure()
{
    deleteElement(treeRoot);
}

std::string TreeLikeStructure::getSelectedItem()
{
    if(selectedFile == NULL)
    {
        return "NULL";
    }
    return this->selectedFile->absolutePath;
}

int TreeLikeStructure::getSelectedItemIndex()
{
    if(selectedFile == NULL)
    {
        return -1;
    }
    return this->selectedFile->fileIndex;
}

void TreeLikeStructure::setSelectedItemIndex(int index)
{
    #ifdef DEBUG_TREELIKESTRUCTURE
        LOG(DEBUG) << "setSelectedItemIndex called (index = " << index << ")";
    #endif


    Element* newSelectedElement = findItemInFolderByIndex(treeRoot, index);

    #ifdef DEBUG_TREELIKESTRUCTURE
        LOG(DEBUG) << "findItemInFolderByIndex returned " << newSelectedElement;
    #endif

    if(newSelectedElement != NULL)
    {

        //printf("setSelectedItemIndex successful!\n");
        selectedFile = newSelectedElement;
    }
    else
    {
        selectedFile = NULL;
    }
}

void TreeLikeStructure::setSelectedItemName(const std::string& absolutePath)
{
    Element* newSelectedElement = findItemInFolderByAbsolutePath(treeRoot, absolutePath);
    if(newSelectedElement != NULL)
    {
        //printf("setSelectedItemName successful!\n");
        selectedFile = newSelectedElement;
    }
    else
    {
        printf("fatal error: setSelectedItemName called but absolutePath (%s) does't belong to tree\n", absolutePath.c_str()); 
        selectedFile = NULL;
    }
}


TreeLikeStructure::Element* TreeLikeStructure::findItemInFolderByIndex(TreeLikeStructure::Element* folder, int index)
{
    int childrenSize = folder->children.size();
    for(int i = 0 ; i < childrenSize ; i++)
    {
        if(folder->children[i]->type == ELEMENT_TYPE_FILE && folder->children[i]->fileIndex == index)
        {
            return folder->children[i];
        }
        else
        {
            Element* returnValue = findItemInFolderByIndex(folder->children[i], index);
            if(returnValue != NULL)
            {
                return returnValue;
            }
        }
        
    }
    return NULL;
}
TreeLikeStructure::Element* TreeLikeStructure::searchItemByIndex(int index)
{
    return findItemInFolderByIndex(treeRoot, index);
}


TreeLikeStructure::Element* TreeLikeStructure::findItemInFolderByAbsolutePath(TreeLikeStructure::Element* folder, const std::string& absolutePath)
{
    int childrenSize = folder->children.size();
    for(int i = 0 ; i < childrenSize ; i++)
    {
        if(folder->children[i]->type == ELEMENT_TYPE_FILE && folder->children[i]->absolutePath == absolutePath)
        {
            return folder->children[i];
        }
        else
        {
            Element* returnValue = findItemInFolderByAbsolutePath(folder->children[i], absolutePath);
            if(returnValue != NULL)
            {
                return returnValue;
            }
        }
        
    }
    return NULL;
}
TreeLikeStructure::Element* TreeLikeStructure::searchItemByAbsolutePath(const std::string& absolutePath)
{
    return findItemInFolderByAbsolutePath(treeRoot, absolutePath);
}

TreeLikeStructure::Element* TreeLikeStructure::Element::getFolderByName(const std::string& folderName)
{
    int childrenSize = children.size();
    for(int i = 0 ; i < childrenSize ; i++)
    {
        if(children[i]->elementName == folderName && children[i]->type == ELEMENT_TYPE_FOLDER)
        {
            return children[i];
        }
    }
    return NULL;
}

void TreeLikeStructure::Element::insertFolder(const std::string& folderName)
{

    
    #ifdef DEBUG_TREELIKESTRUCTURE
    LOG(DEBUG) << "insertFolder called (folderName = " << folderName << ")";
    #endif
    

    Element* newElement = new Element();
    newElement->type = ELEMENT_TYPE_FOLDER;
    newElement->elementName = folderName;
    if(this->absolutePath == "")
    {
        newElement->absolutePath = folderName;
    }
    else
    {
        newElement->absolutePath = this->absolutePath + "\\" + folderName;
    }
    newElement->relativePath = this->absolutePath;
}

TreeLikeStructure::Element* TreeLikeStructure::Element::enterFolder(const std::string& folderToEnter)
{
    
    #ifdef DEBUG_TREELIKESTRUCTURE
    LOG(DEBUG) << "enterFolder called (folderToEnter = " << folderToEnter << ")";
    #endif
    

    int childrenSize = children.size();
    for(int i = 0 ; i < childrenSize ; i++)
    {
        if(children[i]->elementName == folderToEnter && children[i]->type == ELEMENT_TYPE_FOLDER)
        {
            return children[i];
        }
    }

    #ifdef DEBUG_TREELIKESTRUCTURE
    LOG(DEBUG) << "folder did not exist logically, creating it...";
    #endif

    Element* newElement = new Element();
    newElement->type = ELEMENT_TYPE_FOLDER;
    newElement->elementName = folderToEnter;
    if(this->absolutePath == "")
    {
        newElement->absolutePath = folderToEnter;
    }
    else
    {
        newElement->absolutePath = this->absolutePath + "\\" + folderToEnter;
    }
    int lastSlashPosition = newElement->absolutePath.find_last_of("\\");
    if(lastSlashPosition == std::string::npos)
    {
        newElement->relativePath = newElement->absolutePath;
    }
    else
    {
        newElement->relativePath = newElement->absolutePath.substr(0, lastSlashPosition);
    }
    
    this->children.push_back(newElement);
    return newElement;
}

TreeLikeStructure::Element* TreeLikeStructure::Element::insertFile(const std::string& fileName, int index)
{
    //printf("absolutePath: %s\n", absolutePath.c_str());
    //printf("about to insert file: %s\n", fileName.c_str());
    
    #ifdef DEBUG_TREELIKESTRUCTURE

    LOG(DEBUG) << "insertFile called (fileName = " << fileName << ", index = " << index << ")";

    #endif
    
    //had to be removed due to bug
    /*
    if(this->containsFile(fileName))
    {
        //printf("contains already! %s\n", fileName);
        return NULL;
    }
    */

    Element* newElement = new Element();
    newElement->type = ELEMENT_TYPE_FILE;
    newElement->elementName = fileName;

    
    #ifdef DEBUG_TREELIKESTRUCTURE
    LOG(DEBUG) << "newElement->elementName = " << fileName;
    #endif

    if(this->absolutePath == "")
    {
        #ifdef DEBUG_TREELIKESTRUCTURE
        LOG(DEBUG) << "this->absolutePath == \"\"";
        #endif

        newElement->absolutePath = fileName;
    }
    else
    {
        #ifdef DEBUG_TREELIKESTRUCTURE
        LOG(DEBUG) << "this->absolutePath != \"\"";
        #endif

        newElement->absolutePath = this->absolutePath + "\\" + fileName;
    }


    newElement->relativePath = this->absolutePath;
    newElement->fileIndex = index;

    #ifdef DEBUG_TREELIKESTRUCTURE
    LOG(DEBUG) << "newElement->absolutePath = " << newElement->absolutePath;
    LOG(DEBUG) << "newElement->relativePath = " << newElement->relativePath;
    LOG(DEBUG) << "newElement->fileIndex = " << newElement->fileIndex;
    #endif

    

    return newElement;
}

/*
bool TreeLikeStructure::Element::containsFile(const std::string& fileName)
{

    #ifdef DEBUG_TREELIKESTRUCTURE
    LOG(DEBUG) << "containsFile called";
    #endif

    int childrenSize = children.size();
    for(int i = 0 ; i < childrenSize ; i++)
    {
        if(children[i]->type == ELEMENT_TYPE_FILE && children[i]->elementName == fileName)
        {
            return true;
        }
    }
    return false;
}
*/

bool TreeLikeStructure::Element::containsFile(const std::string& absoluteFilePath)
{

    #ifdef DEBUG_TREELIKESTRUCTURE
    LOG(DEBUG) << "containsFile called";
    #endif

    int childrenSize = children.size();
    for(int i = 0 ; i < childrenSize ; i++)
    {
        if(children[i]->type == ELEMENT_TYPE_FILE && children[i]->absolutePath == absoluteFilePath)
        {
            return true;
        }
    }
    return false;
}

void TreeLikeStructure::deleteElement(TreeLikeStructure::Element* element)
{
    #ifdef DEBUG_TREELIKESTRUCTURE
    LOG(DEBUG) << "deleteElement called";
    #endif

    if(element->type == ELEMENT_TYPE_FILE)
    {
        delete element;
    }
    else // folder
    {
        int childrenSize = element->children.size();
        for(int i = 0 ; i < childrenSize ; i++)
        {
            deleteElement(element->children[i]);
        }
        delete element;
    }
}

void TreeLikeStructure::generateTreeStructure(std::vector<std::string>& items)
{

    int itemsSize = items.size();

    #ifdef DEBUG_TREELIKESTRUCTURE
    
    LOG(DEBUG) << "generateTreeStructure called";

    for(int i = 0 ; i < itemsSize ; i++)
    {
        LOG(DEBUG) << "items[" << i << "] = " << items[i];
    }
    
    
    
    #endif

    
    

    // replace / by \\ in all strings
    for(int i = 0 ; i < itemsSize ; i++)
    {
        correctSlashes(items[i]);
    }


    /*
    if(itemsSize == 0)
    {
        std::cout << "error: no items in tree structure" << std::endl;
        crash();
    }
    */





    bool firstFileRecognized = false;
    selectedFile = NULL;

    treeRoot = new TreeLikeStructure::Element();
    if(treeRoot == NULL)
    {
        std::cout << "error allocating memory: treeRoot" << std::endl;
    }
    treeRoot->type = ELEMENT_TYPE_ROOT;
    treeRoot->absolutePath = "";
    treeRoot->elementName = "";
    treeRoot->relativePath = "";

    

    TreeLikeStructure::Element* currentTreeNode = treeRoot;
    
    for(int i = 0 ; i < itemsSize ; i++)
    {
        //std::cout << items[i] << std::endl;

        #ifdef DEBUG_TREELIKESTRUCTURE
        LOG(DEBUG) << "items[i] = " << items[i];
        #endif    

        std::string itemActualName;
        std::string itemRelativePath;

        int lastSlashPosition = items[i].find_last_of("\\");
        if(lastSlashPosition == std::string::npos)
        {
            itemActualName = items[i];
            itemRelativePath = "";
        }
        else
        {
            itemActualName = items[i].substr(lastSlashPosition + 1, items[i].size());
            itemRelativePath = items[i].substr(0, lastSlashPosition);
        }

        #ifdef DEBUG_TREELIKESTRUCTURE
        LOG(DEBUG) << "itemActualName = " << itemActualName;
        LOG(DEBUG) << "itemRelativePath = " << itemRelativePath;
        LOG(DEBUG) << "currentTreeNode->relativePath = " << currentTreeNode->relativePath;
        #endif

        //printf("\n\nENTERED LOOP!!!\n\n");
        //if(itemRelativePath != currentTreeNode->relativePath)
        
        {

            #ifdef DEBUG_TREELIKESTRUCTURE
            LOG(DEBUG) << "itemRelativePath != currentTreeNode->absolutePath";
            #endif

            currentTreeNode = treeRoot;

            #ifdef DEBUG_TREELIKESTRUCTURE
            LOG(DEBUG) << "currentTreeNode = treeRoot";
            #endif

            std::string relativePathToEnter = itemRelativePath;

            //std::cout << "relativePathToEnter: " << relativePathToEnter << std::endl;

            while(currentTreeNode->absolutePath != itemRelativePath)
            {
                //std::cout << "currentTreeNode->relativePath: " << currentTreeNode->relativePath << std::endl;
                //std::cout << "itemRelativePath: " << itemRelativePath << std::endl;
                int firstSlashPosition = relativePathToEnter.find_first_of("\\");
                std::string folderToEnter;
                if(firstSlashPosition == std::string::npos)
                {
                    folderToEnter = relativePathToEnter;
                }
                else
                {
                    folderToEnter = relativePathToEnter.substr(0, firstSlashPosition);
                }
                relativePathToEnter = relativePathToEnter.substr(firstSlashPosition + 1, relativePathToEnter.size());
                //std::cout << "folderToEnter: " << folderToEnter << std::endl;

                #ifdef DEBUG_TREELIKESTRUCTURE
                LOG(DEBUG) << "logically entering folder (folderToEnter = " << folderToEnter << ")";
                #endif

                currentTreeNode = currentTreeNode->enterFolder(folderToEnter);

                //std::cout << "LOOP EXECUTED!" << std::endl << std::endl;

                //waitFor(1000);
            }
        }

        //printf("inserting file: %s\n", itemActualName.c_str());
        Element* insertedFile = currentTreeNode->insertFile(itemActualName, this->elementsInTree);

        /*
        // bug fix
        insertedFile->relativePath = currentTreeNode->absolutePath;
        if(currentTreeNode->absolutePath == "")
        {
            insertedFile->absolutePath = insertedFile->elementName;
        }
        else
        {
            insertedFile->absolutePath = insertedFile->relativePath + "\\" + insertedFile->elementName;
        }
        */

        #ifdef DEBUG_TREELIKESTRUCTURE
        LOG(DEBUG) << "insertedFile->elementName = " << insertedFile->elementName;
        LOG(DEBUG) << "insertedFile->relativePath = " << insertedFile->relativePath;
        LOG(DEBUG) << "insertedFile->absolutePath = " << insertedFile->absolutePath;
        #endif


        //printf("insertFile: %d\n", insertedFile);
        this->elementsInTree++;

        currentTreeNode->children.push_back(insertedFile);
        //printf("pushed: %d\n", insertedFile);


        if(!firstFileRecognized)
        {
            selectedFile = insertedFile;
            firstFileRecognized = true;
        }
        

        


    }


}

void TreeLikeStructure::showFileGUI(TreeLikeStructure::Element* file)
{
    static ImGuiTreeNodeFlags base_flags_file = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth |  ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    ImGuiTreeNodeFlags file_flags = base_flags_file;
    base_flags_file |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
    if(selectedFile == file)
    {
        file_flags |= ImGuiTreeNodeFlags_Selected;
    }
    if(ImGui::TreeNodeEx((void*)(intptr_t)currentId, file_flags, file->elementName.c_str()))
    {
        if(ImGui::IsItemClicked())
        {
            selectedFile = file;
        }
    }
    currentId++;
}

void TreeLikeStructure::showFolderGUI(TreeLikeStructure::Element* folder)
{

    #ifdef DEBUG_TREELIKESTRUCTURE
    LOG(DEBUG) << "showFolderGUI called (folder = " << folder << ")";
    #endif

    static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
    static bool align_label_with_current_x_position = false;
    static bool test_drag_and_drop = false;
    if (align_label_with_current_x_position)
        ImGui::Unindent(ImGui::GetTreeNodeToLabelSpacing());

    bool nodeOpen;
    if(folder->type == ELEMENT_TYPE_ROOT)
    {

        #ifdef DEBUG_TREELIKESTRUCTURE
        LOG(DEBUG) << "folder->type == ELEMENT_TYPE_ROOT";
        #endif

        nodeOpen = ImGui::TreeNodeEx(this->name.c_str(), base_flags);
        currentId++;
    }
    else
    {
        nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)currentId, base_flags, folder->elementName.c_str());
        currentId++;
    }
    if(nodeOpen)
    {
        int childrenSize = folder->children.size();

        #ifdef DEBUG_TREELIKESTRUCTURE
        LOG(DEBUG) << "childrenSize = " << childrenSize;
        #endif

        //printf("before\n");
        for(int i = 0 ; i < childrenSize ; i++)
        {
            //printf("i = %d\n", i);

            //printf("absolutePath-> %s\n", folder->children[i]->absolutePath.c_str());

            #ifdef DEBUG_TREELIKESTRUCTURE
            LOG(DEBUG) << "folder->children[i]->absolutePath = " << folder->children[i]->absolutePath;
            #endif

            if(folder->children[i]->type == ELEMENT_TYPE_FILE)
            {

                #ifdef DEBUG_TREELIKESTRUCTURE
                LOG(DEBUG) << "folder->children[i]->type == ELEMENT_TYPE_FILE";
                LOG(DEBUG) << "calling showFileGUI (folder->children[i]->absolutePath = " << folder->children[i]->absolutePath << ")";
                #endif

                //char dbg[256];
                //sprintf(dbg, "%s", folder->children[i]->absolutePath.c_str());

                //dbgMsg(dbg);
                //printf("%s\n", dbg);



                showFileGUI(folder->children[i]);
            }
            else
            {
                #ifdef DEBUG_TREELIKESTRUCTURE
                LOG(DEBUG) << "folder->children[i]->type != ELEMENT_TYPE_FILE";
                LOG(DEBUG) << "calling showFolderGUI...";
                #endif
                showFolderGUI(folder->children[i]);
            }
            
        }
        ImGui::TreePop();
    }
    
}

void TreeLikeStructure::showGUI()
{  
    currentId = 0;



    showFolderGUI(treeRoot);
    
    char selectedFileText[2048];

    if(selectedFile != NULL)
    {
        sprintf(selectedFileText, "selected tree item: %s (%d)", selectedFile->absolutePath.c_str(), selectedFile->fileIndex);
    }
    else
    {
        sprintf(selectedFileText, "selected tree item: fileIndex is NULL");
    }
    ImGui::TextWrapped(selectedFileText);

    /*
    char totalFilesText[2048];
    sprintf(totalFilesText, "total elements: %d", elementsInTree);
    ImGui::TextWrapped(totalFilesText);
    */
    
}

/*
void TreeLikeStructure::showGUIex()
{
    if (ImGui::TreeNode("treeRoot"))
        {
            static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
            static bool align_label_with_current_x_position = false;
            static bool test_drag_and_drop = false;
            //ImGui::CheckboxFlags("ImGuiTreeNodeFlags_OpenOnArrow",       (unsigned int*)&base_flags, ImGuiTreeNodeFlags_OpenOnArrow);
            //ImGui::CheckboxFlags("ImGuiTreeNodeFlags_OpenOnDoubleClick", (unsigned int*)&base_flags, ImGuiTreeNodeFlags_OpenOnDoubleClick);
            //ImGui::CheckboxFlags("ImGuiTreeNodeFlags_SpanAvailWidth",    (unsigned int*)&base_flags, ImGuiTreeNodeFlags_SpanAvailWidth); ImGui::SameLine();
            //ImGui::CheckboxFlags("ImGuiTreeNodeFlags_SpanFullWidth",     (unsigned int*)&base_flags, ImGuiTreeNodeFlags_SpanFullWidth);
            //ImGui::Checkbox("Align label with current X position", &align_label_with_current_x_position);
            //ImGui::Checkbox("Test tree node as drag source", &test_drag_and_drop);
            ImGui::Text("Hello!");
            if (align_label_with_current_x_position)
                ImGui::Unindent(ImGui::GetTreeNodeToLabelSpacing());

            // 'selection_mask' is dumb representation of what may be user-side selection state.
            //  You may retain selection state inside or outside your objects in whatever format you see fit.
            // 'node_clicked' is temporary storage of what node we have clicked to process selection at the end
            /// of the loop. May be a pointer to your own node type, etc.
            static int selection_mask = (1 << 2);
            int node_clicked = -1;
            for (int i = 0; i < 6; i++)
            {
                // Disable the default "open on single-click behavior" + set Selected flag according to our selection.
                ImGuiTreeNodeFlags node_flags = base_flags;
                const bool is_selected = (selection_mask & (1 << i)) != 0;
                if (is_selected)
                    node_flags |= ImGuiTreeNodeFlags_Selected;
                if (i < 3)
                {
                    // Items 0..2 are Tree Node
                    bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, "Selectable Node %d", i);
                    //if (ImGui::IsItemClicked())
                        //node_clicked = i;
                    if (node_open)
                    {
                        ImGui::BulletText("Blah blah\nBlah Blah");
                        ImGui::TreePop();
                    }
                }
                else
                {
                    // Items 3..5 are Tree Leaves
                    // The only reason we use TreeNode at all is to allow selection of the leaf. Otherwise we can
                    // use BulletText() or advance the cursor by GetTreeNodeToLabelSpacing() and call Text().
                    node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
                    ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, "Selectable Leaf %d", i);
                    if (ImGui::IsItemClicked())
                        node_clicked = i;
                }
            }

            
            if (node_clicked != -1)
            {
                // Update selection state
                // (process outside of tree loop to avoid visual inconsistencies during the clicking frame)
                if (ImGui::GetIO().KeyCtrl)
                    selection_mask ^= (1 << node_clicked);          // CTRL+click to toggle
                else //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, may want to preserve selection when clicking on item that is part of the selection
                    selection_mask = (1 << node_clicked);           // Click to single-select
            }
            
            
            if (align_label_with_current_x_position)
                ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing());
            
            ImGui::TreePop();
        }
}
*/