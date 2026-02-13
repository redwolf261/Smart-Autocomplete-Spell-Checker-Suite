#ifndef BKTREE_H
#define BKTREE_H

#include <string>
#include <vector>
#include <map>
#include "editdistance.h"

/**
 * BK-Tree (Burkhard-Keller Tree) Node
 */
struct BKNode {
    std::string word;
    std::map<int, BKNode*> children;  // Key: edit distance, Value: child node

    BKNode(const std::string& w) : word(w) {}
    ~BKNode();
};

/**
 * BK-Tree Implementation
 * 
 * Purpose: Support efficient approximate string matching using edit distance
 * 
 * Properties:
 * - Metric tree specialized for discrete distance metrics
 * - Children are organized by edit distance from parent
 * - Enables fast fuzzy search without comparing against all words
 * 
 * Time Complexity:
 * - Insert: O(log n) average, O(n) worst case
 * - Search: O(log n) average for small distances, much better than O(n) linear search
 * 
 * Space Complexity: O(n) where n is the number of words
 * 
 * Use Case: Find all words within edit distance k from a query word
 * Example: Query "speling" with distance 2 finds ["spelling", "selling", "peeling"]
 */
class BKTree {
private:
    BKNode* root;

    /**
     * Helper function to recursively insert a word
     */
    void insertHelper(BKNode* node, const std::string& word);

    /**
     * Helper function to recursively search for words within maxDistance
     */
    void searchHelper(BKNode* node, const std::string& word, int maxDistance,
                     std::vector<std::pair<std::string, int>>& results);

    /**
     * Helper function to destroy the tree
     */
    void destroyTree(BKNode* node);

    /**
     * Helper function to count nodes
     */
    int countNodes(BKNode* node);

public:
    /**
     * Constructor
     */
    BKTree();

    /**
     * Destructor
     */
    ~BKTree();

    /**
     * Insert a word into the BK-Tree
     * @param word: The word to insert
     */
    void insert(const std::string& word);

    /**
     * Search for words within a given edit distance
     * @param word: The query word
     * @param maxDistance: Maximum allowed edit distance
     * @return: Vector of (word, distance) pairs where distance <= maxDistance
     * 
     * Example:
     * - searchByDistance("speling", 1) might return:
     *   [("spelling", 1)]
     * - searchByDistance("speling", 2) might return:
     *   [("spelling", 1), ("selling", 2), ("peeling", 2)]
     */
    std::vector<std::pair<std::string, int>> searchByDistance(
        const std::string& word, int maxDistance);

    /**
     * Find the closest word(s) to the query
     * @param word: The query word
     * @param maxResults: Maximum number of results to return
     * @return: Vector of (word, distance) pairs, sorted by distance
     */
    std::vector<std::pair<std::string, int>> findClosest(
        const std::string& word, int maxResults = 5);

    /**
     * Check if tree is empty
     */
    bool isEmpty() const { return root == nullptr; }

    /**
     * Get the number of words in the tree
     */
    int size();

    /**
     * Clear the entire tree
     */
    void clear();
};

#endif // BKTREE_H
