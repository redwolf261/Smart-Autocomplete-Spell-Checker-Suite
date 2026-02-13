#ifndef EDITDISTANCE_H
#define EDITDISTANCE_H

#include <string>
#include <vector>

/**
 * Edit Distance (Levenshtein Distance) Calculator
 * 
 * Purpose: Calculate the minimum number of single-character edits needed
 *          to transform one string into another
 * 
 * Operations allowed:
 * 1. Insert a character
 * 2. Delete a character
 * 3. Replace a character
 * 
 * Algorithm: Dynamic Programming
 * 
 * Time Complexity: O(m * n) where m, n are lengths of the two strings
 * Space Complexity: O(m * n) for DP table (can be optimized to O(min(m, n)))
 * 
 * Use Cases:
 * - Spell checking (find words with small edit distance)
 * - DNA sequence alignment
 * - Fuzzy string matching
 */
class EditDistance {
public:
    /**
     * Calculate Levenshtein distance between two strings
     * @param word1: First string
     * @param word2: Second string
     * @return: Minimum number of edits required
     * 
     * Example:
     * - editDistance("kitten", "sitting") = 3
     *   (replace k→s, replace e→i, insert g)
     */
    static int calculate(const std::string& word1, const std::string& word2);

    /**
     * Calculate edit distance with space optimization
     * Uses only O(min(m, n)) space instead of O(m * n)
     */
    static int calculateOptimized(const std::string& word1, const std::string& word2);

    /**
     * Get the similarity ratio between two strings
     * @param word1: First string
     * @param word2: Second string
     * @return: Similarity score from 0.0 (completely different) to 1.0 (identical)
     * 
     * Formula: 1 - (editDistance / maxLength)
     */
    static double similarity(const std::string& word1, const std::string& word2);

    /**
     * Check if two strings are similar within a threshold
     * @param word1: First string
     * @param word2: Second string
     * @param maxDistance: Maximum allowed edit distance
     * @return: true if edit distance <= maxDistance
     */
    static bool isSimilar(const std::string& word1, const std::string& word2, int maxDistance);

private:
    /**
     * Helper function to get minimum of three values
     */
    static int min3(int a, int b, int c);
};

#endif // EDITDISTANCE_H
