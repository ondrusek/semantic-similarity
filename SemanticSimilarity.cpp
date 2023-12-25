#include <unordered_map>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stack>
#include <limits>
#include "SemanticSimilarity.h"

SemanticSimilarity::SemanticSimilarity()
{
    std::ifstream dataFile("./data.txt");
    std::string lineString;
    std::string itemString;
    int depth;

    while (std::getline(dataFile, lineString)) {
        depth = 0;
        for (char c : lineString) {
            if (c == ' ') {
                ++depth;
            } else {
                break;
            }
        }
        itemString = lineString.substr(depth);
        depth /= 2;

        wordsById.emplace_back(depth, itemString);

        if (itemString.find('/') != std::string::npos) {
            std::string subItemString;
            std::stringstream ss(itemString);
            while (std::getline(ss, subItemString, '/')) {
                wordsById.emplace_back(depth + 1, subItemString);
            }
        }
    }

    wordMap.reserve(wordsById.size());
    parents.reserve(wordsById.size());

    int nextDepth;
    std::stack<int> stack;

    for (int i = 0; i < wordsById.size(); ++i) {
        wordMap.insert({wordsById[i].second, i});

        depth = wordsById[i].first;

        if (i < wordsById.size() - 1) {
            nextDepth = wordsById[i + 1].first;
        } else {
            nextDepth = 0;
        }

        if (stack.empty()) {
            parents.push_back(-1);
        } else {
            parents.push_back(stack.top());
        }

        if (depth > nextDepth) {
            for (int j = 0; j < depth - nextDepth; ++j) {
                if (!stack.empty()) {
                    stack.pop();
                }
            }
        }

        if (depth < nextDepth) {
            stack.push(i);
        }
    }
}

auto SemanticSimilarity::getWordNotBelongingToSet(const std::set<std::string>& selectedWords) -> std::string
{
    if (selectedWords.size() < 3) {
        return "";
    }

    std::unordered_multimap<int, std::vector<int>> wordsSets; // {wordId, parentsSet}
    wordsSets.reserve(selectedWords.size() * 2);

    for (const auto& selectedWord : selectedWords) {
        auto its = wordMap.equal_range(selectedWord); // include same words with different id
        for (auto it = its.first; it != its.second; ++it) {
            int id = it->second;
            std::string word = it->first;

            std::vector<int> tmp;
            int i = id;

            tmp.push_back(i);

            while (parents[i] != -1) {
                tmp.push_back(parents[i]);
                i = parents[i];
            }

            wordsSets.insert({id, tmp});
        }
    }

    std::set<std::pair<int, int>> wordPairs; // {wordId, wordId}

    for (const auto& wordSet : wordsSets) {
        for (const auto& wordSet2 : wordsSets) {
            if (wordSet.first != wordSet2.first) {
                //if (!wordPairs.contains({wordSet.first, wordSet2.first}) && !wordPairs.contains({wordSet2.first, wordSet.first})) {
                if (wordPairs.count({wordSet.first, wordSet2.first}) == 0 && wordPairs.count({wordSet2.first, wordSet.first}) == 0) {
                    wordPairs.emplace(wordSet.first, wordSet2.first);
                }
            }
        }
    }

    std::unordered_multimap<int, int> scores; // {wordId, score}
    scores.reserve(wordPairs.size());

    for (const auto& wordPair : wordPairs) {
        std::vector<int> firstSets = wordsSets.find(wordPair.first)->second;
        std::vector<int> secondSets = wordsSets.find(wordPair.second)->second;

        int wordsSimilarity = getDistanceBetweenWords(firstSets, secondSets);

        std::cout << "{" << wordsById[wordPair.first].second << " (" << wordPair.first << "), " << wordsById[wordPair.second].second << " (" << wordPair.second << ")} -> " << wordsSimilarity << std::endl;

        auto it = scores.find(wordPair.first);
        if (it != scores.end()) {
            it->second += wordsSimilarity;
        } else {
            scores.insert({wordPair.first, wordsSimilarity});
        }

        auto it2 = scores.find(wordPair.second);
        if (it2 != scores.end()) {
            it2->second += wordsSimilarity;
        } else {
            scores.insert({wordPair.second, wordsSimilarity});
        }
    }

    std::unordered_map<std::string, int> scoresUnique; // {word, score}
    scoresUnique.reserve(scores.size());

    for (const auto& score : scores) {
        auto it = scoresUnique.find(wordsById[score.first].second);
        if (it != scoresUnique.end()) {
            if (it->second < score.second) {
                it->second = score.second;
            }
        } else {
            scoresUnique.insert({wordsById[score.first].second, score.second});
        }
    }

    std::cout << "---------------" << std::endl;

    int worstScore = std::numeric_limits<int>::max();;
    int secondWorstScore = std::numeric_limits<int>::max();;
    std::string excludedWord;

    for (const auto& score : scoresUnique) {
        if (score.second < worstScore) {
            if (worstScore != std::numeric_limits<int>::max()) {
                secondWorstScore = worstScore;
            }
            worstScore = score.second;
            excludedWord = score.first;
        } else {
            if (score.second < secondWorstScore) {
                secondWorstScore = score.second;
            }
        }
        std::cout << score.first << ": " << score.second << std::endl;
    }

    if (worstScore == secondWorstScore) {
        excludedWord = "";
    }

    std::cout << "---------------" << std::endl;
    std::cout << "Worst score: " << worstScore << std::endl;
    std::cout << "Second worst score: " << secondWorstScore << std::endl;
    std::cout << "EXCLUDED WORD: " << (excludedWord.empty() ? "-" : excludedWord) << std::endl;
    std::cout << "---------------" << std::endl;

    return excludedWord;
}

int SemanticSimilarity::getDistanceBetweenWords(const std::vector<int>& firstWordSets, const std::vector<int>& secondWordSets)
{
    int score = 0;
    for (const auto& firstWordSet : firstWordSets) {
        for (const auto& secondWordSet : secondWordSets) {
            if (firstWordSet == secondWordSet) {
                score += 1;
            }
        }
    }
    return score;
}
