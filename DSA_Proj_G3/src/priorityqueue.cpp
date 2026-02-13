#include "priorityqueue.h"
#include <stdexcept>
#include <algorithm>

PriorityQueue::PriorityQueue() {
    // Empty constructor
}

void PriorityQueue::swap(int i, int j) {
    std::pair<std::string, int> temp = heap[i];
    heap[i] = heap[j];
    heap[j] = temp;
}

void PriorityQueue::bubbleUp(int i) {
    while (i > 0 && heap[parent(i)].second < heap[i].second) {
        swap(i, parent(i));
        i = parent(i);
    }
}

void PriorityQueue::bubbleDown(int i) {
    int maxIndex = i;
    int left = leftChild(i);
    int right = rightChild(i);
    
    if (left < static_cast<int>(heap.size()) && heap[left].second > heap[maxIndex].second) {
        maxIndex = left;
    }
    
    if (right < static_cast<int>(heap.size()) && heap[right].second > heap[maxIndex].second) {
        maxIndex = right;
    }
    
    if (i != maxIndex) {
        swap(i, maxIndex);
        bubbleDown(maxIndex);
    }
}

void PriorityQueue::insert(const std::string& word, int frequency) {
    heap.push_back({word, frequency});
    bubbleUp(heap.size() - 1);
}

std::pair<std::string, int> PriorityQueue::extractMax() {
    if (isEmpty()) {
        throw std::runtime_error("Priority queue is empty");
    }
    
    std::pair<std::string, int> maxElement = heap[0];
    heap[0] = heap.back();
    heap.pop_back();
    
    if (!isEmpty()) {
        bubbleDown(0);
    }
    
    return maxElement;
}

std::pair<std::string, int> PriorityQueue::peek() const {
    if (isEmpty()) {
        throw std::runtime_error("Priority queue is empty");
    }
    
    return heap[0];
}

void PriorityQueue::buildHeap(const std::vector<std::pair<std::string, int>>& elements) {
    heap = elements;
    
    // Start from last non-leaf node and heapify
    for (int i = (heap.size() / 2) - 1; i >= 0; i--) {
        bubbleDown(i);
    }
}

std::vector<std::pair<std::string, int>> PriorityQueue::getTopK(int k) {
    std::vector<std::pair<std::string, int>> result;
    
    // Create a copy of the heap
    std::vector<std::pair<std::string, int>> tempHeap = heap;
    
    // Extract top k elements
    for (int i = 0; i < k && !heap.empty(); i++) {
        result.push_back(extractMax());
    }
    
    // Restore the heap
    heap = tempHeap;
    
    return result;
}

std::vector<std::pair<std::string, int>> PriorityQueue::extractTopK(int k) {
    std::vector<std::pair<std::string, int>> result;
    
    for (int i = 0; i < k && !isEmpty(); i++) {
        result.push_back(extractMax());
    }
    
    return result;
}
