#pragma once

#include <string>
#include <set>
#include <vector>
#include <unordered_map>

class SemanticSimilarity
{
public:
    SemanticSimilarity();
    std::string getWordNotBelongingToSet(const std::set<std::string>& selectedWords);
private:
    std::vector<std::pair<int, std::string>> wordsById;
    std::vector<int> parents;
    std::unordered_multimap<std::string, int> wordMap;
    int static getDistanceBetweenWords(const std::vector<int>& firstWordSets, const std::vector<int>& secondWordSets);
};
