#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <string>
#include <vector>

/**
 * Node for chaining in hash table buckets
 */
struct HashNode {
    std::string key;
    int value;
    HashNode* next;

    HashNode(const std::string& k, int v) : key(k), value(v), next(nullptr) {}
};

/**
 * Hash Table Implementation using Chaining
 * 
 * Purpose: Store word-frequency mappings efficiently
 * 
 * Collision Resolution: Chaining with linked lists
 * 
 * Time Complexity (Average Case):
 * - Insert: O(1)
 * - Search: O(1)
 * - Update: O(1)
 * - Delete: O(1)
 * 
 * Time Complexity (Worst Case - all collisions):
 * - Insert: O(n)
 * - Search: O(n)
 * 
 * Space Complexity: O(n + m) where n is table size, m is number of elements
 * 
 * Load Factor = m/n (typically keep < 0.75 for good performance)
 */
class HashTable {
private:
    std::vector<HashNode*> table;
    int tableSize;
    int elementCount;
    
    /**
     * Hash function: Maps string key to table index
     * Uses polynomial rolling hash with modulo
     */
    int hashFunction(const std::string& key);

    /**
     * Resize and rehash the table when load factor is too high
     */
    void resize();

public:
    /**
     * Constructor
     * @param size: Initial size of hash table (should be prime for better distribution)
     */
    HashTable(int size = 1000);

    /**
     * Destructor: Clean up all linked lists
     */
    ~HashTable();

    /**
     * Insert or update a key-value pair
     * @param key: The word to insert
     * @param value: The frequency/value associated with the word
     */
    void insert(const std::string& key, int value);

    /**
     * Search for a key in the hash table
     * @param key: The word to search for
     * @param value: Reference to store the found value
     * @return: true if key exists, false otherwise
     */
    bool search(const std::string& key, int& value);

    /**
     * Update the value associated with a key
     * @param key: The word to update
     * @param value: The new value
     * @return: true if key exists and was updated, false otherwise
     */
    bool update(const std::string& key, int value);

    /**
     * Increment the value associated with a key
     * @param key: The word to increment
     * @param increment: Amount to add (default: 1)
     * @return: true if key exists and was incremented, false otherwise
     */
    bool increment(const std::string& key, int increment = 1);

    /**
     * Delete a key from the hash table
     * @param key: The word to delete
     * @return: true if key was deleted, false if not found
     */
    bool remove(const std::string& key);

    /**
     * Check if a key exists
     * @param key: The word to check
     * @return: true if key exists, false otherwise
     */
    bool contains(const std::string& key);

    /**
     * Get the current load factor
     * @return: elementCount / tableSize
     */
    double getLoadFactor() const;

    /**
     * Get the number of elements
     */
    int getElementCount() const { return elementCount; }

    /**
     * Get the table size
     */
    int getTableSize() const { return tableSize; }

    /**
     * Clear all elements from the table
     */
    void clear();
};

#endif // HASHTABLE_H
