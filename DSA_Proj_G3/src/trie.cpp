#include "trie.h"
#include <algorithm>

// ==================== TrieNode Implementation ====================

TrieNode::TrieNode() : isEndOfWord(false), frequency(0) {
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        children[i] = nullptr;
    }
}

TrieNode::~TrieNode() {
    // Destructor handled by Trie destructor to avoid double deletion
}

// ==================== Trie Implementation ====================

Trie::Trie() {
    root = new TrieNode();
}

Trie::~Trie() {
    destroyTrie(root);
}

void Trie::destroyTrie(TrieNode* node) {
    if (node == nullptr) return;
    
    for (int i = 0; i < TrieNode::ALPHABET_SIZE; i++) {
        if (node->children[i] != nullptr) {
            destroyTrie(node->children[i]);
        }
    }
    delete node;
}

void Trie::insertWord(const std::string& word, int frequency) {
    if (word.empty()) return;
    
    TrieNode* current = root;
    
    for (char ch : word) {
        // Convert to lowercase and get index (a=0, b=1, ..., z=25)
        int index = tolower(ch) - 'a';
        
        // Skip if character is not a-z
        if (index < 0 || index >= TrieNode::ALPHABET_SIZE) {
            continue;
        }
        
        // Create new node if path doesn't exist
        if (current->children[index] == nullptr) {
            current->children[index] = new TrieNode();
        }
        
        current = current->children[index];
    }
    
    // Mark end of word and set frequency
    current->isEndOfWord = true;
    current->frequency = frequency;
}

bool Trie::searchWord(const std::string& word) {
    if (word.empty()) return false;
    
    TrieNode* current = root;
    
    for (char ch : word) {
        int index = tolower(ch) - 'a';
        
        if (index < 0 || index >= TrieNode::ALPHABET_SIZE) {
            return false;
        }
        
        if (current->children[index] == nullptr) {
            return false;
        }
        
        current = current->children[index];
    }
    
    return current->isEndOfWord;
}

std::vector<std::pair<std::string, int>> Trie::searchPrefix(const std::string& prefix) {
    std::vector<std::pair<std::string, int>> results;
    
    if (prefix.empty()) return results;
    
    TrieNode* current = root;
    
    // Navigate to the end of the prefix
    for (char ch : prefix) {
        int index = tolower(ch) - 'a';
        
        if (index < 0 || index >= TrieNode::ALPHABET_SIZE) {
            return results;
        }
        
        if (current->children[index] == nullptr) {
            return results;  // Prefix doesn't exist
        }
        
        current = current->children[index];
    }
    
    // Collect all words with this prefix
    std::string prefixLower = prefix;
    std::transform(prefixLower.begin(), prefixLower.end(), prefixLower.begin(), ::tolower);
    collectWords(current, prefixLower, results);
    
    return results;
}

void Trie::collectWords(TrieNode* node, std::string currentWord, 
                       std::vector<std::pair<std::string, int>>& results) {
    if (node == nullptr) return;
    
    if (node->isEndOfWord) {
        results.push_back({currentWord, node->frequency});
    }
    
    // Recursively collect words from all children
    for (int i = 0; i < TrieNode::ALPHABET_SIZE; i++) {
        if (node->children[i] != nullptr) {
            collectWords(node->children[i], currentWord + char('a' + i), results);
        }
    }
}

std::vector<std::string> Trie::suggestWords(const std::string& prefix, int maxResults) {
    std::vector<std::pair<std::string, int>> matches = searchPrefix(prefix);
    
    // Sort by frequency (descending)
    std::sort(matches.begin(), matches.end(), 
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Extract top maxResults words
    std::vector<std::string> suggestions;
    int count = std::min(maxResults, static_cast<int>(matches.size()));
    
    for (int i = 0; i < count; i++) {
        suggestions.push_back(matches[i].first);
    }
    
    return suggestions;
}

void Trie::updateFrequency(const std::string& word, int increment) {
    if (word.empty()) return;
    
    TrieNode* current = root;
    
    for (char ch : word) {
        int index = tolower(ch) - 'a';
        
        if (index < 0 || index >= TrieNode::ALPHABET_SIZE) {
            return;
        }
        
        if (current->children[index] == nullptr) {
            return;  // Word doesn't exist
        }
        
        current = current->children[index];
    }
    
    if (current->isEndOfWord) {
        current->frequency += increment;
    }
}

int Trie::getWordCount() {
    std::vector<std::pair<std::string, int>> allWords;
    collectWords(root, "", allWords);
    return allWords.size();
}
