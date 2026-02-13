#include "bloomfilter.h"
#include <cmath>

BloomFilter::BloomFilter(int size, int numHashFunctions) 
    : size(size), numHashFunctions(numHashFunctions), elementCount(0) {
    bitArray.resize(size, false);
}

int BloomFilter::hash1(const std::string& word) {
    // Simple polynomial rolling hash
    unsigned long hash = 0;
    int prime = 31;
    
    for (char ch : word) {
        hash = (hash * prime + tolower(ch)) % size;
    }
    
    return hash;
}

int BloomFilter::hash2(const std::string& word) {
    // DJB2 hash algorithm
    unsigned long hash = 5381;
    
    for (char ch : word) {
        hash = ((hash << 5) + hash) + tolower(ch);
    }
    
    return hash % size;
}

int BloomFilter::hash3(const std::string& word) {
    // SDBM hash algorithm
    unsigned long hash = 0;
    
    for (char ch : word) {
        hash = tolower(ch) + (hash << 6) + (hash << 16) - hash;
    }
    
    return hash % size;
}

int BloomFilter::hash4(const std::string& word) {
    // Modified FNV-1a hash
    unsigned long hash = 2166136261u;
    
    for (char ch : word) {
        hash ^= tolower(ch);
        hash *= 16777619;
    }
    
    return hash % size;
}

std::vector<int> BloomFilter::getHashIndices(const std::string& word) {
    std::vector<int> indices;
    
    if (numHashFunctions >= 1) indices.push_back(hash1(word));
    if (numHashFunctions >= 2) indices.push_back(hash2(word));
    if (numHashFunctions >= 3) indices.push_back(hash3(word));
    if (numHashFunctions >= 4) indices.push_back(hash4(word));
    
    return indices;
}

void BloomFilter::addWord(const std::string& word) {
    if (word.empty()) return;
    
    std::vector<int> indices = getHashIndices(word);
    
    for (int index : indices) {
        bitArray[index] = true;
    }
    
    elementCount++;
}

bool BloomFilter::containsWord(const std::string& word) {
    if (word.empty()) return false;
    
    std::vector<int> indices = getHashIndices(word);
    
    for (int index : indices) {
        if (!bitArray[index]) {
            return false;  // Definitely not present
        }
    }
    
    return true;  // Might be present (could be false positive)
}

double BloomFilter::getFalsePositiveRate() {
    if (elementCount == 0) return 0.0;
    
    // Formula: (1 - e^(-k*n/m))^k
    // k = numHashFunctions
    // n = elementCount
    // m = size
    
    double exponent = -static_cast<double>(numHashFunctions * elementCount) / size;
    double probability = std::pow(1 - std::exp(exponent), numHashFunctions);
    
    return probability;
}

void BloomFilter::clear() {
    std::fill(bitArray.begin(), bitArray.end(), false);
    elementCount = 0;
}
