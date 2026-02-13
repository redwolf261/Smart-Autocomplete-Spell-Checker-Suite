#ifndef TRIE_H
#define TRIE_H

#include <string>
#include <vector>

/**
 * TrieNode represents a single node in the Trie data structure
 * Each node contains:
 * - children: Array of 26 pointers (for lowercase a-z)
 * - isEndOfWord: Flag indicating if this node marks the end of a valid word
 * - frequency: Word usage frequency for ranking suggestions
 */
class TrieNode {
public:
    static const int ALPHABET_SIZE = 26;
    TrieNode* children[ALPHABET_SIZE];
    bool isEndOfWord;
    int frequency;

    TrieNode();
    ~TrieNode();
};

/**
 * Trie (Prefix Tree) Implementation
 * 
 * Purpose: Enable efficient prefix-based autocomplete search
 * 
 * Time Complexity:
 * - Insert: O(m) where m is the length of the word
 * - Search: O(m) where m is the length of the word
 * - Prefix Search: O(m + k) where k is the number of matching words
 * 
 * Space Complexity: O(N * M * 26) where N is number of words, M is average length
 */
class Trie {
private:
    TrieNode* root;
    
    // Helper function for recursive word collection
    void collectWords(TrieNode* node, std::string currentWord, 
                     std::vector<std::pair<std::string, int>>& results);
    
    // Helper function for destroying the trie
    void destroyTrie(TrieNode* node);

public:
    Trie();
    ~Trie();

    /**
     * Insert a word into the Trie
     * @param word: The word to insert
     * @param frequency: Usage frequency of the word (default: 1)
     */
    void insertWord(const std::string& word, int frequency = 1);

    /**
     * Search for an exact word in the Trie
     * @param word: The word to search for
     * @return: true if word exists, false otherwise
     */
    bool searchWord(const std::string& word);

    /**
     * Search for all words with a given prefix
     * @param prefix: The prefix to search for
     * @return: Vector of (word, frequency) pairs matching the prefix
     */
    std::vector<std::pair<std::string, int>> searchPrefix(const std::string& prefix);

    /**
     * Get autocomplete suggestions for a prefix
     * Returns words sorted by frequency
     * @param prefix: The prefix to search for
     * @param maxResults: Maximum number of suggestions to return
     * @return: Vector of suggested words
     */
    std::vector<std::string> suggestWords(const std::string& prefix, int maxResults = 10);

    /**
     * Update the frequency of a word
     * @param word: The word to update
     * @param increment: Amount to increment frequency by
     */
    void updateFrequency(const std::string& word, int increment = 1);

    /**
     * Get the total number of words in the Trie
     * @return: Count of unique words
     */
    int getWordCount();
};

#endif // TRIE_H
