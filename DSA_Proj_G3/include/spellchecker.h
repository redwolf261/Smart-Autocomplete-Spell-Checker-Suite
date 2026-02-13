#ifndef SPELLCHECKER_H
#define SPELLCHECKER_H

#include <string>
#include <vector>
#include "trie.h"
#include "bloomfilter.h"
#include "hashtable.h"
#include "lrucache.h"
#include "priorityqueue.h"
#include "bktree.h"

/**
 * SpellChecker Core Engine
 * 
 * Purpose: Integrate all data structures into a cohesive spell-checking
 *          and autocomplete system
 * 
 * Components:
 * - Trie: Fast prefix-based autocomplete
 * - Bloom Filter: Quick word existence check (filter invalid words)
 * - Hash Table: Store word frequencies
 * - LRU Cache: Cache recent queries
 * - Priority Queue: Rank suggestions by frequency
 * - BK-Tree: Find spelling corrections via edit distance
 * 
 * Workflow:
 * 1. Autocomplete Query:
 *    - Check LRU cache first
 *    - Use Trie to find prefix matches
 *    - Rank by frequency using Priority Queue
 *    - Cache result
 * 
 * 2. Spell Check Query:
 *    - Check Bloom Filter (quick negative check)
 *    - If not in dictionary, use BK-Tree for corrections
 *    - Return top corrections
 */
class SpellChecker {
private:
    Trie* trie;
    BloomFilter* bloomFilter;
    HashTable* frequencyTable;
    LRUCache* cache;
    BKTree* bkTree;

    // Statistics
    int cacheHits;
    int cacheMisses;
    int totalQueries;

    /**
     * Rank suggestions by frequency
     */
    std::vector<std::string> rankByFrequency(
        const std::vector<std::pair<std::string, int>>& candidates,
        int maxResults
    );

public:
    /**
     * Constructor
     * @param cacheSize: Size of LRU cache
     * @param bloomSize: Size of Bloom filter bit array
     */
    SpellChecker(int cacheSize = 100, int bloomSize = 10000);

    /**
     * Destructor
     */
    ~SpellChecker();

    /**
     * Load dictionary from file
     * Populates Trie, Bloom Filter, Hash Table, and BK-Tree
     * @param filename: Path to dictionary file
     * @return: Number of words loaded
     */
    int loadDictionary(const std::string& filename);

    /**
     * Get autocomplete suggestions for a prefix
     * @param prefix: The prefix to search for
     * @param maxResults: Maximum number of suggestions (default: 10)
     * @return: Vector of suggested words, ranked by frequency
     */
    std::vector<std::string> autocomplete(const std::string& prefix, int maxResults = 10);

    /**
     * Check if a word is spelled correctly
     * @param word: The word to check
     * @return: true if word exists in dictionary, false otherwise
     */
    bool checkSpelling(const std::string& word);

    /**
     * Get spelling corrections for a misspelled word
     * @param word: The misspelled word
     * @param maxDistance: Maximum edit distance for corrections (default: 2)
     * @param maxResults: Maximum number of corrections to return (default: 5)
     * @return: Vector of suggested corrections, sorted by edit distance
     */
    std::vector<std::string> getCorrections(
        const std::string& word,
        int maxDistance = 2,
        int maxResults = 5
    );

    /**
     * Update word frequency (when user selects a suggestion)
     * @param word: The word to update
     */
    void updateFrequency(const std::string& word);

    /**
     * Get cache statistics
     */
    void getCacheStats(int& hits, int& misses, double& hitRate);

    /**
     * Get Bloom filter false positive rate
     */
    double getBloomFilterFPR();

    /**
     * Get total number of words in dictionary
     */
    int getDictionarySize();

    /**
     * Clear all cached data
     */
    void clearCache();

    /**
     * Reset statistics
     */
    void resetStats();

    /**
     * Get detailed statistics as string
     */
    std::string getStatistics();

    /**
     * Process query from file and write result to output file
     * @param queryFile: Input file with query
     * @param outputFile: Output JSON file for results
     * @param isAutocomplete: true for autocomplete, false for spell check
     */
    void processQuery(
        const std::string& queryFile,
        const std::string& outputFile,
        bool isAutocomplete = true
    );
};

#endif // SPELLCHECKER_H
