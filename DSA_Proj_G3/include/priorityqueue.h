#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H

#include <string>
#include <vector>
#include <utility>

/**
 * Priority Queue (Max Heap) Implementation
 * 
 * Purpose: Rank autocomplete suggestions by word frequency
 * 
 * Implementation: Array-based binary max heap
 * - Parent at index i has children at 2i+1 and 2i+2
 * - For max heap: parent >= children
 * 
 * Time Complexity:
 * - insert(): O(log n) - Insert at end, bubble up
 * - extractMax(): O(log n) - Remove root, bubble down
 * - peek(): O(1) - View maximum without removal
 * - heapify(): O(n) - Build heap from array
 * 
 * Space Complexity: O(n)
 * 
 * Use Case: Given word-frequency pairs, quickly get top-k most frequent words
 */
class PriorityQueue {
private:
    // Store pairs of (word, frequency)
    std::vector<std::pair<std::string, int>> heap;

    /**
     * Get parent index
     */
    int parent(int i) { return (i - 1) / 2; }

    /**
     * Get left child index
     */
    int leftChild(int i) { return 2 * i + 1; }

    /**
     * Get right child index
     */
    int rightChild(int i) { return 2 * i + 2; }

    /**
     * Bubble up element at index i to maintain heap property
     */
    void bubbleUp(int i);

    /**
     * Bubble down element at index i to maintain heap property
     */
    void bubbleDown(int i);

    /**
     * Swap two elements in the heap
     */
    void swap(int i, int j);

public:
    /**
     * Constructor
     */
    PriorityQueue();

    /**
     * Insert a word-frequency pair into the heap
     * @param word: The word to insert
     * @param frequency: The frequency/priority of the word
     */
    void insert(const std::string& word, int frequency);

    /**
     * Extract and return the maximum element (highest frequency)
     * @return: Pair of (word, frequency) with highest priority
     * @throws: std::runtime_error if heap is empty
     */
    std::pair<std::string, int> extractMax();

    /**
     * Peek at the maximum element without removing it
     * @return: Pair of (word, frequency) with highest priority
     * @throws: std::runtime_error if heap is empty
     */
    std::pair<std::string, int> peek() const;

    /**
     * Check if the heap is empty
     */
    bool isEmpty() const { return heap.empty(); }

    /**
     * Get the size of the heap
     */
    int size() const { return heap.size(); }

    /**
     * Clear all elements from the heap
     */
    void clear() { heap.clear(); }

    /**
     * Build heap from a vector of pairs (heapify)
     * @param elements: Vector of (word, frequency) pairs
     * 
     * Time Complexity: O(n) - More efficient than inserting one by one
     */
    void buildHeap(const std::vector<std::pair<std::string, int>>& elements);

    /**
     * Get top k elements without removing them
     * @param k: Number of top elements to retrieve
     * @return: Vector of top k (word, frequency) pairs in descending order
     */
    std::vector<std::pair<std::string, int>> getTopK(int k);

    /**
     * Extract top k elements (removes them from heap)
     * @param k: Number of top elements to extract
     * @return: Vector of top k (word, frequency) pairs in descending order
     */
    std::vector<std::pair<std::string, int>> extractTopK(int k);
};

#endif // PRIORITYQUEUE_H
