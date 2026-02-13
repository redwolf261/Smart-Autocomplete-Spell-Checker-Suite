#ifndef AUTOCOMPLETE_H
#define AUTOCOMPLETE_H

#include <string>
#include <vector>
#include <utility>

// Forward declarations
class Trie;
class HashTable;
class PriorityQueue;

/**
 * Autocomplete Engine Module
 * Provides intelligent word completion using Trie + Heap + Sorting
 */

class AutocompleteEngine {
private:
    Trie* trie;
    HashTable* frequencyMap;
    int maxSuggestions;

    // Collect all words with given prefix from Trie
    void collectSuggestions(const std::string& prefix, 
                           std::vector<std::string>& results);

    // Rank suggestions by frequency
    std::vector<std::pair<std::string, int>> rankByFrequency(
        const std::vector<std::string>& words
    );

public:
    /**
     * Constructor
     * @param t: Pointer to Trie data structure
     * @param hm: Pointer to HashTable for frequency mapping
     * @param maxSugg: Maximum number of suggestions to return
     */
    AutocompleteEngine(Trie* t, HashTable* hm, int maxSugg = 10);

    /**
     * Get autocomplete suggestions for a prefix
     * @param prefix: Input prefix
     * @return: Vector of (word, frequency) pairs, sorted
     */
    std::vector<std::pair<std::string, int>> getSuggestions(const std::string& prefix);

    /**
     * Add word to autocomplete system
     * @param word: Word to add
     * @param frequency: Frequency score (default: 1)
     */
    void addWord(const std::string& word, int frequency = 1);

    /**
     * Check if word exists
     * @param word: Word to check
     * @return: true if exists, false otherwise
     */
    bool hasWord(const std::string& word);

    /**
     * Get total word count
     * @return: Number of words in system
     */
    int getWordCount() const;

    /**
     * Clear all data
     */
    void clear();

    /**
     * Load words from API results
     * @param words: Vector of (word, score) pairs from API
     */
    void loadFromAPI(const std::vector<std::pair<std::string, int>>& words);

    // Destructor
    ~AutocompleteEngine();
};

#endif // AUTOCOMPLETE_H
