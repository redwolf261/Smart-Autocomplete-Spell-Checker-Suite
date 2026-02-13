#ifndef LRUCACHE_H
#define LRUCACHE_H

#include <string>
#include <unordered_map>
#include <vector>

/**
 * Node for Doubly Linked List in LRU Cache
 */
struct CacheNode {
    std::string key;
    std::string value;
    CacheNode* prev;
    CacheNode* next;

    CacheNode(const std::string& k, const std::string& v) 
        : key(k), value(v), prev(nullptr), next(nullptr) {}
};

/**
 * LRU (Least Recently Used) Cache Implementation
 * 
 * Purpose: Cache most recent lookups for faster repeated access
 * 
 * Data Structures:
 * - Doubly Linked List: Maintains order of usage (most recent at head)
 * - Hash Map: Provides O(1) access to nodes
 * 
 * Time Complexity:
 * - get(): O(1) - Hash lookup + list manipulation
 * - put(): O(1) - Hash insert + list manipulation
 * 
 * Space Complexity: O(capacity)
 * 
 * Eviction Policy:
 * When cache is full, removes the least recently used item (tail of list)
 */
class LRUCache {
private:
    int capacity;
    int size;
    CacheNode* head;  // Most recently used
    CacheNode* tail;  // Least recently used
    std::unordered_map<std::string, CacheNode*> cache;

    /**
     * Move a node to the head (mark as most recently used)
     */
    void moveToHead(CacheNode* node);

    /**
     * Remove a node from the list
     */
    void removeNode(CacheNode* node);

    /**
     * Add a node right after head
     */
    void addToHead(CacheNode* node);

    /**
     * Remove the tail node (least recently used)
     */
    CacheNode* removeTail();

public:
    /**
     * Constructor
     * @param capacity: Maximum number of items in cache
     */
    LRUCache(int capacity = 100);

    /**
     * Destructor: Clean up all nodes
     */
    ~LRUCache();

    /**
     * Get a value from cache
     * @param key: The key to look up
     * @param value: Reference to store the found value
     * @return: true if key exists (cache hit), false otherwise (cache miss)
     */
    bool get(const std::string& key, std::string& value);

    /**
     * Put a key-value pair into cache
     * If cache is full, evicts the least recently used item
     * @param key: The key to insert
     * @param value: The value to store
     */
    void put(const std::string& key, const std::string& value);

    /**
     * Check if a key exists in cache
     * @param key: The key to check
     * @return: true if key exists, false otherwise
     */
    bool contains(const std::string& key);

    /**
     * Get the current size of cache
     */
    int getSize() const { return size; }

    /**
     * Get the capacity of cache
     */
    int getCapacity() const { return capacity; }

    /**
     * Calculate cache hit rate
     * @param hits: Number of cache hits
     * @param totalRequests: Total number of requests
     * @return: Hit rate as a percentage (0-100)
     */
    static double calculateHitRate(int hits, int totalRequests);

    /**
     * Clear all items from cache
     */
    void clear();

    /**
     * Get all keys currently in cache (for debugging/testing)
     */
    std::vector<std::string> getKeys() const;
};

#endif // LRUCACHE_H
