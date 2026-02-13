#include "sort.h"
#include <cctype>

namespace SortAlgorithms {

    // ==================== UTILITY FUNCTIONS ====================
    
    int compareStrings(const std::string& a, const std::string& b) {
        size_t minLen = a.length() < b.length() ? a.length() : b.length();
        
        for (size_t i = 0; i < minLen; i++) {
            char charA = std::tolower(a[i]);
            char charB = std::tolower(b[i]);
            
            if (charA < charB) return -1;
            if (charA > charB) return 1;
        }
        
        // If all characters match, shorter string comes first
        if (a.length() < b.length()) return -1;
        if (a.length() > b.length()) return 1;
        return 0;
    }

    // ==================== LEXICOGRAPHIC QUICKSORT ====================
    
    int partitionLexicographic(std::vector<std::string>& arr, int low, int high) {
        std::string pivot = arr[high];
        int i = low - 1;
        
        for (int j = low; j < high; j++) {
            if (compareStrings(arr[j], pivot) <= 0) {
                i++;
                swap(arr[i], arr[j]);
            }
        }
        
        swap(arr[i + 1], arr[high]);
        return i + 1;
    }
    
    void quickSortLexicographic(std::vector<std::string>& arr, int low, int high) {
        if (low < high) {
            int pi = partitionLexicographic(arr, low, high);
            quickSortLexicographic(arr, low, pi - 1);
            quickSortLexicographic(arr, pi + 1, high);
        }
    }
    
    void sortLexicographic(std::vector<std::string>& arr) {
        if (arr.size() <= 1) return;
        quickSortLexicographic(arr, 0, arr.size() - 1);
    }

    // ==================== FREQUENCY-BASED QUICKSORT ====================
    
    int partitionByFrequency(std::vector<std::pair<std::string, int>>& arr, int low, int high) {
        int pivotFreq = arr[high].second;
        int i = low - 1;
        
        for (int j = low; j < high; j++) {
            // Sort in DESCENDING order (higher frequency first)
            if (arr[j].second > pivotFreq) {
                i++;
                swap(arr[i], arr[j]);
            } else if (arr[j].second == pivotFreq) {
                // If frequencies are equal, sort lexicographically
                if (compareStrings(arr[j].first, arr[high].first) < 0) {
                    i++;
                    swap(arr[i], arr[j]);
                }
            }
        }
        
        swap(arr[i + 1], arr[high]);
        return i + 1;
    }
    
    void quickSortByFrequency(std::vector<std::pair<std::string, int>>& arr, int low, int high) {
        if (low < high) {
            int pi = partitionByFrequency(arr, low, high);
            quickSortByFrequency(arr, low, pi - 1);
            quickSortByFrequency(arr, pi + 1, high);
        }
    }
    
    void sortByFrequency(std::vector<std::pair<std::string, int>>& arr) {
        if (arr.size() <= 1) return;
        quickSortByFrequency(arr, 0, arr.size() - 1);
    }

    // ==================== CORRECTION RANKING QUICKSORT ====================
    
    int partitionByRank(std::vector<CorrectionCandidate>& arr, int low, int high) {
        const CorrectionCandidate& pivot = arr[high];
        int i = low - 1;
        
        for (int j = low; j < high; j++) {
            bool shouldSwap = false;
            
            // Primary: Sort by distance (ascending - lower distance is better)
            if (arr[j].distance < pivot.distance) {
                shouldSwap = true;
            } else if (arr[j].distance == pivot.distance) {
                // Secondary: Sort by frequency (descending - higher frequency is better)
                if (arr[j].frequency > pivot.frequency) {
                    shouldSwap = true;
                } else if (arr[j].frequency == pivot.frequency) {
                    // Tertiary: Sort lexicographically
                    if (compareStrings(arr[j].word, pivot.word) < 0) {
                        shouldSwap = true;
                    }
                }
            }
            
            if (shouldSwap) {
                i++;
                swap(arr[i], arr[j]);
            }
        }
        
        swap(arr[i + 1], arr[high]);
        return i + 1;
    }
    
    void quickSortByRank(std::vector<CorrectionCandidate>& arr, int low, int high) {
        if (low < high) {
            int pi = partitionByRank(arr, low, high);
            quickSortByRank(arr, low, pi - 1);
            quickSortByRank(arr, pi + 1, high);
        }
    }
    
    void sortByRank(std::vector<CorrectionCandidate>& arr) {
        if (arr.size() <= 1) return;
        quickSortByRank(arr, 0, arr.size() - 1);
    }

    // ==================== MERGE SORT (Alternative Implementation) ====================
    
    void merge(std::vector<std::string>& arr, int left, int mid, int right) {
        int n1 = mid - left + 1;
        int n2 = right - mid;
        
        // Create temp arrays
        std::vector<std::string> leftArr(n1);
        std::vector<std::string> rightArr(n2);
        
        // Copy data to temp arrays
        for (int i = 0; i < n1; i++) {
            leftArr[i] = arr[left + i];
        }
        for (int j = 0; j < n2; j++) {
            rightArr[j] = arr[mid + 1 + j];
        }
        
        // Merge the temp arrays back
        int i = 0, j = 0, k = left;
        
        while (i < n1 && j < n2) {
            if (compareStrings(leftArr[i], rightArr[j]) <= 0) {
                arr[k] = leftArr[i];
                i++;
            } else {
                arr[k] = rightArr[j];
                j++;
            }
            k++;
        }
        
        // Copy remaining elements
        while (i < n1) {
            arr[k] = leftArr[i];
            i++;
            k++;
        }
        
        while (j < n2) {
            arr[k] = rightArr[j];
            j++;
            k++;
        }
    }
    
    void mergeSortLexicographic(std::vector<std::string>& arr, int left, int right) {
        if (left < right) {
            int mid = left + (right - left) / 2;
            
            mergeSortLexicographic(arr, left, mid);
            mergeSortLexicographic(arr, mid + 1, right);
            
            merge(arr, left, mid, right);
        }
    }

} // namespace SortAlgorithms
