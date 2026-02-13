#ifndef SORT_H
#define SORT_H

#include <string>
#include <vector>
#include <utility>

/**
 * Custom Sorting Algorithms for DSA Project
 * Implements QuickSort for lexicographic and frequency-based ranking
 * NO STL sort() used - all implementations are manual
 */

namespace SortAlgorithms {

    // ==================== LEXICOGRAPHIC SORTING ====================
    
    /**
     * Quick Sort for lexicographic (alphabetical) string sorting
     * Time Complexity: O(n log n) average, O(n²) worst case
     * Space Complexity: O(log n) for recursion stack
     * @param arr: Vector of strings to sort
     * @param low: Starting index
     * @param high: Ending index
     */
    void quickSortLexicographic(std::vector<std::string>& arr, int low, int high);
    
    /**
     * Partition function for lexicographic quicksort
     * @param arr: Vector of strings
     * @param low: Starting index
     * @param high: Ending index
     * @return: Partition index
     */
    int partitionLexicographic(std::vector<std::string>& arr, int low, int high);
    
    /**
     * Public wrapper for lexicographic sorting
     * @param arr: Vector of strings to sort
     */
    void sortLexicographic(std::vector<std::string>& arr);

    // ==================== FREQUENCY-BASED SORTING ====================
    
    /**
     * Quick Sort for (word, frequency) pairs by frequency (descending)
     * Higher frequency = higher rank
     * Time Complexity: O(n log n) average
     * @param arr: Vector of (string, int) pairs
     * @param low: Starting index
     * @param high: Ending index
     */
    void quickSortByFrequency(std::vector<std::pair<std::string, int>>& arr, int low, int high);
    
    /**
     * Partition function for frequency-based quicksort
     * @param arr: Vector of pairs
     * @param low: Starting index
     * @param high: Ending index
     * @return: Partition index
     */
    int partitionByFrequency(std::vector<std::pair<std::string, int>>& arr, int low, int high);
    
    /**
     * Public wrapper for frequency sorting
     * @param arr: Vector of (word, frequency) pairs
     */
    void sortByFrequency(std::vector<std::pair<std::string, int>>& arr);

    // ==================== DISTANCE + FREQUENCY SORTING ====================
    
    /**
     * Sort by edit distance (ascending) then frequency (descending)
     * Used for spell correction ranking
     * Time Complexity: O(n log n)
     * @param arr: Vector of (word, distance, frequency) tuples
     * @param low: Starting index
     * @param high: Ending index
     */
    struct CorrectionCandidate {
        std::string word;
        int distance;
        int frequency;
        
        CorrectionCandidate(const std::string& w, int d, int f) 
            : word(w), distance(d), frequency(f) {}
    };
    
    void quickSortByRank(std::vector<CorrectionCandidate>& arr, int low, int high);
    
    /**
     * Partition for correction ranking
     * Priority: distance (lower is better), then frequency (higher is better)
     */
    int partitionByRank(std::vector<CorrectionCandidate>& arr, int low, int high);
    
    /**
     * Public wrapper for correction ranking
     */
    void sortByRank(std::vector<CorrectionCandidate>& arr);

    // ==================== MERGE SORT (Alternative) ====================
    
    /**
     * Merge Sort for stable sorting (optional alternative)
     * Time Complexity: O(n log n) guaranteed
     * Space Complexity: O(n)
     */
    void mergeSortLexicographic(std::vector<std::string>& arr, int left, int right);
    void merge(std::vector<std::string>& arr, int left, int mid, int right);

    // ==================== UTILITY FUNCTIONS ====================
    
    /**
     * Manual swap function
     */
    template<typename T>
    void swap(T& a, T& b) {
        T temp = a;
        a = b;
        b = temp;
    }
    
    /**
     * Compare strings lexicographically
     * @return: -1 if a < b, 0 if a == b, 1 if a > b
     */
    int compareStrings(const std::string& a, const std::string& b);
    
    /**
     * Get top N elements from sorted array
     */
    template<typename T>
    std::vector<T> getTopN(const std::vector<T>& arr, int n) {
        std::vector<T> result;
        int count = n < arr.size() ? n : arr.size();
        for (int i = 0; i < count; i++) {
            result.push_back(arr[i]);
        }
        return result;
    }
}

#endif // SORT_H
