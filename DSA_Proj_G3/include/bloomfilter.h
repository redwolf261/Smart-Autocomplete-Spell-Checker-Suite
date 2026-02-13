#ifndef BLOOMFILTER_H
#define BLOOMFILTER_H

#include <string>
#include <vector>

/**
 * Bloom Filter Implementation
 * 
 * Purpose: Provide fast probabilistic word existence checking
 * 
 * Characteristics:
 * - Space efficient (uses bit array)
 * - Fast O(k) membership testing (k = number of hash functions)
 * - May have false positives (says "yes" when answer is "no")
 * - Never has false negatives (if it says "no", definitely not present)
 * 
 * Time Complexity:
 * - Add: O(k) where k is the number of hash functions
 * - Contains: O(k)
 * 
 * Space Complexity: O(n) where n is the size of bit array
 * 
 * False Positive Rate: Depends on (m/n) ratio and k
 * - m: number of elements
 * - n: size of bit array
 * - k: number of hash functions
 * - FPR ≈ (1 - e^(-kn/m))^k
 */
class BloomFilter {
private:
    std::vector<bool> bitArray;
    int size;
    int numHashFunctions;
    int elementCount;

    /**
     * Hash function 1: Simple polynomial rolling hash
     */
    int hash1(const std::string& word);

    /**
     * Hash function 2: DJB2 hash algorithm
     */
    int hash2(const std::string& word);

    /**
     * Hash function 3: SDBM hash algorithm
     */
    int hash3(const std::string& word);

    /**
     * Hash function 4: Modified FNV-1a hash
     */
    int hash4(const std::string& word);

    /**
     * Get all hash indices for a word
     */
    std::vector<int> getHashIndices(const std::string& word);

public:
    /**
     * Constructor
     * @param size: Size of the bit array (larger = fewer false positives)
     * @param numHashFunctions: Number of hash functions to use (typically 3-5)
     */
    BloomFilter(int size = 10000, int numHashFunctions = 4);

    /**
     * Add a word to the Bloom filter
     * @param word: The word to add
     */
    void addWord(const std::string& word);

    /**
     * Check if a word might exist in the filter
     * @param word: The word to check
     * @return: true if word MIGHT exist (or false positive), false if DEFINITELY doesn't exist
     */
    bool containsWord(const std::string& word);

    /**
     * Calculate the theoretical false positive rate
     * @return: Estimated false positive probability (0.0 to 1.0)
     */
    double getFalsePositiveRate();

    /**
     * Get the number of elements added
     */
    int getElementCount() const { return elementCount; }

    /**
     * Get the size of the bit array
     */
    int getSize() const { return size; }

    /**
     * Get the number of hash functions
     */
    int getNumHashFunctions() const { return numHashFunctions; }

    /**
     * Reset the bloom filter
     */
    void clear();
};

#endif // BLOOMFILTER_H
