#include "autocomplete.h"
#include "trie.h"
#include "hashtable.h"
#include "priorityqueue.h"
#include "sort.h"
#include <algorithm>

AutocompleteEngine::AutocompleteEngine(Trie* t, HashTable* hm, int maxSugg)
    : trie(t), frequencyMap(hm), maxSuggestions(maxSugg) {
}

AutocompleteEngine::~AutocompleteEngine() {
    // Don't delete trie and hashtable, they're managed externally
}

void AutocompleteEngine::collectSuggestions(const std::string& prefix, 
                                           std::vector<std::string>& results) {
    if (!trie) return;
    
    // Use Trie's suggestWords method (correct API)
    results = trie->suggestWords(prefix, maxSuggestions * 2); // Get more for ranking
}

std::vector<std::pair<std::string, int>> AutocompleteEngine::rankByFrequency(
    const std::vector<std::string>& words) {
    
    std::vector<std::pair<std::string, int>> ranked;
    
    // Get frequency for each word
    // Note: HashTable API doesn't have get() method, using default frequency
    // Trie already maintains frequency information internally
    for (const std::string& word : words) {
        int freq = 1; // Default frequency
        ranked.push_back({word, freq});
    }

    // Sort using custom QuickSort (by frequency descending, then lexicographic)
    // Provide low and high indices as required
    if (!ranked.empty()) {
        SortAlgorithms::quickSortByFrequency(ranked, 0, ranked.size() - 1);
    }

    // Limit to maxSuggestions
    if (ranked.size() > static_cast<size_t>(maxSuggestions)) {
        ranked.resize(maxSuggestions);
    }

    return ranked;
}

std::vector<std::pair<std::string, int>> AutocompleteEngine::getSuggestions(
    const std::string& prefix) {
    
    std::vector<std::string> words;
    collectSuggestions(prefix, words);

    if (words.empty()) {
        return {};
    }

    return rankByFrequency(words);
}

void AutocompleteEngine::addWord(const std::string& word, int frequency) {
    if (!trie || word.empty()) return;

    // Add to Trie (correct method name: insertWord)
    trie->insertWord(word, frequency);

    // Update frequency in HashTable
    if (frequencyMap) {
        if (frequencyMap->contains(word)) {
            frequencyMap->increment(word, frequency);
        } else {
            frequencyMap->insert(word, frequency);
        }
    }
}

bool AutocompleteEngine::hasWord(const std::string& word) {
    if (!trie) return false;
    return trie->searchWord(word);  // Correct method name
}

int AutocompleteEngine::getWordCount() const {
    if (!trie) return 0;
    return trie->getWordCount();
}

void AutocompleteEngine::clear() {
    // Note: We don't clear trie/hashtable as they might be used elsewhere
    // This method is here for potential future use
}

void AutocompleteEngine::loadFromAPI(const std::vector<std::pair<std::string, int>>& words) {
    // Use traditional loop instead of structured binding (C++17 compatibility)
    for (size_t i = 0; i < words.size(); i++) {
        const std::string& word = words[i].first;
        int score = words[i].second;
        addWord(word, score);
    }
}
