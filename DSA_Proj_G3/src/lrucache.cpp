#include "lrucache.h"
#include <algorithm>

LRUCache::LRUCache(int capacity) : capacity(capacity), size(0) {
    head = new CacheNode("", "");
    tail = new CacheNode("", "");
    head->next = tail;
    tail->prev = head;
}

LRUCache::~LRUCache() {
    clear();
    delete head;
    delete tail;
}

void LRUCache::moveToHead(CacheNode* node) {
    removeNode(node);
    addToHead(node);
}

void LRUCache::removeNode(CacheNode* node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

void LRUCache::addToHead(CacheNode* node) {
    node->next = head->next;
    node->prev = head;
    head->next->prev = node;
    head->next = node;
}

CacheNode* LRUCache::removeTail() {
    CacheNode* node = tail->prev;
    removeNode(node);
    return node;
}

bool LRUCache::get(const std::string& key, std::string& value) {
    if (cache.find(key) == cache.end()) {
        return false;  // Cache miss
    }
    
    CacheNode* node = cache[key];
    value = node->value;
    moveToHead(node);  // Mark as recently used
    
    return true;  // Cache hit
}

void LRUCache::put(const std::string& key, const std::string& value) {
    if (cache.find(key) != cache.end()) {
        // Key exists, update value and move to head
        CacheNode* node = cache[key];
        node->value = value;
        moveToHead(node);
    } else {
        // New key
        CacheNode* newNode = new CacheNode(key, value);
        cache[key] = newNode;
        addToHead(newNode);
        size++;
        
        if (size > capacity) {
            // Remove least recently used item
            CacheNode* removed = removeTail();
            cache.erase(removed->key);
            delete removed;
            size--;
        }
    }
}

bool LRUCache::contains(const std::string& key) {
    return cache.find(key) != cache.end();
}

double LRUCache::calculateHitRate(int hits, int totalRequests) {
    if (totalRequests == 0) return 0.0;
    return (static_cast<double>(hits) / totalRequests) * 100.0;
}

void LRUCache::clear() {
    CacheNode* current = head->next;
    while (current != tail) {
        CacheNode* temp = current;
        current = current->next;
        delete temp;
    }
    
    head->next = tail;
    tail->prev = head;
    cache.clear();
    size = 0;
}

std::vector<std::string> LRUCache::getKeys() const {
    std::vector<std::string> keys;
    
    CacheNode* current = head->next;
    while (current != tail) {
        keys.push_back(current->key);
        current = current->next;
    }
    
    return keys;
}
