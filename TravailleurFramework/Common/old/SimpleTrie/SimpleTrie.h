#ifndef SIMPLE_TRIE_H
#define SIMPLE_TRIE_H

#include <vector>
#include <string>

struct SimpleTrieNode
{
    SimpleTrieNode* nodes[256];
    int quantityContained = 0;
    SimpleTrieNode();
    ~SimpleTrieNode();
    bool isLeaf();
};

class SimpleTrie
{

    SimpleTrieNode* rootNode = nullptr;

    void recursiveGetAutoCompleteResultsFunction(SimpleTrieNode* currentTrieNode, const std::string& currentTrieString, std::vector<std::string>& autoCompleteResults, int& howManyValidResultsLeft);

    int size = 0;

    public:
    SimpleTrie();
    SimpleTrie(const std::string& str);
    SimpleTrie(const std::vector<std::string>& strVector);
    ~SimpleTrie();

    void clear();

    void insertString(const std::string& str);
    bool removeString(const std::string& str);
    void insertStrings(const std::vector<std::string>& strVector);

    SimpleTrieNode* getTrieNodeFromString(const std::string& str);

    bool containsString(const std::string& str);
    int quantityOfString(const std::string& str);

    std::vector<std::string> getAutoCompleteResults(const std::string& str, int maximumNumberOfResults);

    int getSize();

};

#endif