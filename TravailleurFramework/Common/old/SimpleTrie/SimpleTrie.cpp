#include "SimpleTrie.h"

SimpleTrieNode::SimpleTrieNode()
{
    for(int i = 0 ; i < 256 ; i++)
    {
        this->nodes[i] = nullptr;
    }
}

SimpleTrieNode::~SimpleTrieNode()
{
    for(int i = 0 ; i < 256 ; i++)
    {
        if(this->nodes[i] != nullptr)
        {
            delete this->nodes[i];
        }
    }
}

bool SimpleTrieNode::isLeaf()
{
    for(int i = 0 ; i < 256 ; i++)
    {
        if(this->nodes[i] != nullptr)
        {
            return false;
        }
    }
    return true;
}

SimpleTrie::SimpleTrie()
{
    this->rootNode = new SimpleTrieNode();
}


SimpleTrie::SimpleTrie(const std::string& str)
{
    this->rootNode = new SimpleTrieNode();
    this->insertString(str);
}

SimpleTrie::SimpleTrie(const std::vector<std::string>& strVector)
{
    this->rootNode = new SimpleTrieNode();
    this->insertStrings(strVector);
}

SimpleTrie::~SimpleTrie()
{
    if(rootNode != nullptr)
    {
        delete rootNode;
    }
}

void SimpleTrie::clear()
{
    if(rootNode != nullptr)
    {
        delete rootNode;
    }
    rootNode = new SimpleTrieNode();
    this->size = 0;
}

void SimpleTrie::insertString(const std::string& str)
{

    //printf("inserting %s\n", str.c_str());

    this->size++;

    if(str.empty())
    {
        this->rootNode->quantityContained++;
        return;
    }
    int strSize = str.size();

    SimpleTrieNode* currentTrieNode = this->rootNode;
    for(int i = 0 ; i < strSize ; i++)
    {
        //printf("currentTrieNode: %d\n", currentTrieNode);
        if(currentTrieNode->nodes[str[i]] == nullptr)
        {
            //printf("currentTrieNode->nodes[%d] == nullptr\n", str[i]);
            currentTrieNode->nodes[str[i]] = new SimpleTrieNode();
        }
        currentTrieNode = currentTrieNode->nodes[str[i]];
    }

    currentTrieNode->quantityContained++;

}


bool SimpleTrie::removeString(const std::string& str)
{
	//previous implementation
    //SimpleTrieNode* strTrieNode = getTrieNodeFromString(str);
    //if(strTrieNode == nullptr)
    //{
    //    return false;
    //}
    //if(strTrieNode->quantityContained > 0)
    //{
    //    strTrieNode->quantityContained--;
    //    return true;
    //}
    //else
    //{
    //    return false;
    //}
	
	if(str.empty())
    {
        if(rootNode->quantityContained > 0)
        {
            rootNode->quantityContained--;
            this->size--;
            return true;
        }
        return false;
    }
    int strSize = str.size();
    SimpleTrieNode* currentTrieNode = this->rootNode;
    for(int i = 0 ; i < strSize ; i++)
    {
        if(currentTrieNode->nodes[str[i]] == nullptr)
        {
            return false;
        }

        if(i == strSize - 1)
        {
            SimpleTrieNode* deletionTrieNode = currentTrieNode->nodes[str[i]];
            if(deletionTrieNode->quantityContained > 0)
            {
                this->size--;
                deletionTrieNode->quantityContained--;
                if(deletionTrieNode->quantityContained == 0)
                {
                    if(deletionTrieNode->isLeaf())
                    {
                        return true;
                    }
                    delete currentTrieNode->nodes[str[i]];
                    currentTrieNode->nodes[str[i]] = nullptr;
                }
                return true;
            }
            return false;
        }

        currentTrieNode = currentTrieNode->nodes[str[i]];
    }

    // should never get here
    printf("SimpleTrie logic error: should never get here\n");
    return false;
	
}

void SimpleTrie::insertStrings(const std::vector<std::string>& strVector)
{
    int strVectorSize = strVector.size();
    for(int i = 0 ; i < strVectorSize ; i++)
    {
        this->insertString(strVector[i]);
    }
}

SimpleTrieNode* SimpleTrie::getTrieNodeFromString(const std::string& str)
{
    if(str.empty())
    {
        return this->rootNode;
    }
    int strSize = str.size();
    SimpleTrieNode* currentTrieNode = this->rootNode;
    for(int i = 0 ; i < strSize ; i++)
    {
        if(currentTrieNode->nodes[str[i]] == nullptr)
        {
            return nullptr;
        }
        currentTrieNode = currentTrieNode->nodes[str[i]];
    }
    return currentTrieNode;
}

bool SimpleTrie::containsString(const std::string& str)
{
    SimpleTrieNode* strTrieNode = getTrieNodeFromString(str);
    if(strTrieNode == nullptr)
    {
        return false;
    }
    else
    {
        if(strTrieNode->quantityContained > 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}

int SimpleTrie::quantityOfString(const std::string& str)
{
    SimpleTrieNode* strTrieNode = getTrieNodeFromString(str);
    if(strTrieNode == nullptr)
    {
        return 0;
    }
    return strTrieNode->quantityContained;
}

void SimpleTrie::recursiveGetAutoCompleteResultsFunction(SimpleTrieNode* currentTrieNode, const std::string& currentTrieString, std::vector<std::string>& autoCompleteResults, int& howManyValidResultsLeft)
{
    if(currentTrieNode->quantityContained > 0)
    {
        autoCompleteResults.push_back(currentTrieString);
        howManyValidResultsLeft--;
        if(howManyValidResultsLeft <= 0)
        {
            return;
        }
    }

    for(int i = 0 ; i < 256 ; i++)
    {
        
        if(currentTrieNode->nodes[i] != nullptr)
        {
            std::string newCurrentTrieString = currentTrieString + (char)i;
            recursiveGetAutoCompleteResultsFunction(currentTrieNode->nodes[i], newCurrentTrieString, autoCompleteResults, howManyValidResultsLeft);
        }

        if(howManyValidResultsLeft <= 0)
        {
            return;
        }
    }

}

std::vector<std::string> SimpleTrie::getAutoCompleteResults(const std::string& str, int maximumNumberOfResults)
{
    std::vector<std::string> autoCompleteResults;
    if(maximumNumberOfResults <= 0)
    {
        return autoCompleteResults;
    }
    SimpleTrieNode* strTrieNode = getTrieNodeFromString(str);
    if(strTrieNode == nullptr)
    {
        return autoCompleteResults;
    }

    recursiveGetAutoCompleteResultsFunction(strTrieNode, str, autoCompleteResults, maximumNumberOfResults);

    return autoCompleteResults;

}