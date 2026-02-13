#include "hashtable.h"

HashTable::HashTable(int size) : tableSize(size), elementCount(0) {
    table.resize(tableSize, nullptr);
}

HashTable::~HashTable() {
    clear();
}

int HashTable::hashFunction(const std::string& key) {
    // Polynomial rolling hash with prime modulo
    unsigned long hash = 0;
    int prime = 31;
    
    for (char ch : key) {
        hash = (hash * prime + tolower(ch)) % tableSize;
    }
    
    return hash;
}

void HashTable::resize() {
    int oldSize = tableSize;
    tableSize = tableSize * 2;
    
    std::vector<HashNode*> oldTable = table;
    table.clear();
    table.resize(tableSize, nullptr);
    
    elementCount = 0;
    
    // Rehash all elements
    for (int i = 0; i < oldSize; i++) {
        HashNode* current = oldTable[i];
        while (current != nullptr) {
            insert(current->key, current->value);
            HashNode* temp = current;
            current = current->next;
            delete temp;
        }
    }
}

void HashTable::insert(const std::string& key, int value) {
    // Check if load factor exceeds threshold
    if (getLoadFactor() > 0.75) {
        resize();
    }
    
    int index = hashFunction(key);
    HashNode* current = table[index];
    
    // Check if key already exists
    while (current != nullptr) {
        if (current->key == key) {
            current->value = value;  // Update existing value
            return;
        }
        current = current->next;
    }
    
    // Insert new node at the beginning of the chain
    HashNode* newNode = new HashNode(key, value);
    newNode->next = table[index];
    table[index] = newNode;
    elementCount++;
}

bool HashTable::search(const std::string& key, int& value) {
    int index = hashFunction(key);
    HashNode* current = table[index];
    
    while (current != nullptr) {
        if (current->key == key) {
            value = current->value;
            return true;
        }
        current = current->next;
    }
    
    return false;
}

bool HashTable::update(const std::string& key, int value) {
    int index = hashFunction(key);
    HashNode* current = table[index];
    
    while (current != nullptr) {
        if (current->key == key) {
            current->value = value;
            return true;
        }
        current = current->next;
    }
    
    return false;
}

bool HashTable::increment(const std::string& key, int increment) {
    int index = hashFunction(key);
    HashNode* current = table[index];
    
    while (current != nullptr) {
        if (current->key == key) {
            current->value += increment;
            return true;
        }
        current = current->next;
    }
    
    return false;
}

bool HashTable::remove(const std::string& key) {
    int index = hashFunction(key);
    HashNode* current = table[index];
    HashNode* prev = nullptr;
    
    while (current != nullptr) {
        if (current->key == key) {
            if (prev == nullptr) {
                table[index] = current->next;
            } else {
                prev->next = current->next;
            }
            delete current;
            elementCount--;
            return true;
        }
        prev = current;
        current = current->next;
    }
    
    return false;
}

bool HashTable::contains(const std::string& key) {
    int value;
    return search(key, value);
}

double HashTable::getLoadFactor() const {
    return static_cast<double>(elementCount) / tableSize;
}

void HashTable::clear() {
    for (int i = 0; i < tableSize; i++) {
        HashNode* current = table[i];
        while (current != nullptr) {
            HashNode* temp = current;
            current = current->next;
            delete temp;
        }
        table[i] = nullptr;
    }
    elementCount = 0;
}
