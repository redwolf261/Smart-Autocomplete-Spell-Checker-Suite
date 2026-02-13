#include <iostream>
#include <cassert>
#include <chrono>
#include "../include/sort.h"

using namespace SortAlgorithms;

// Test counter
int tests_passed = 0;
int tests_failed = 0;

#define ASSERT_TEST(condition, message) \
    if (condition) { \
        tests_passed++; \
        std::cout << "✓ " << message << std::endl; \
    } else { \
        tests_failed++; \
        std::cout << "✗ " << message << " FAILED!" << std::endl; \
    }

// ==================== TEST FUNCTIONS ====================

void testLexicographicSorting() {
    std::cout << "\nTesting Lexicographic Sorting..." << std::endl;
    
    // Test 1: Basic alphabetical sorting
    std::vector<std::string> words1 = {"zebra", "apple", "banana", "cherry"};
    sortLexicographic(words1);
    ASSERT_TEST(words1[0] == "apple" && words1[1] == "banana" && 
                words1[2] == "cherry" && words1[3] == "zebra",
                "Basic alphabetical sort");
    
    // Test 2: Case insensitive
    std::vector<std::string> words2 = {"Zebra", "apple", "Banana", "CHERRY"};
    sortLexicographic(words2);
    ASSERT_TEST(words2[0] == "apple" && words2[1] == "Banana",
                "Case insensitive sort");
    
    // Test 3: Words with common prefixes
    std::vector<std::string> words3 = {"computer", "computing", "compile", "compiler", "complete"};
    sortLexicographic(words3);
    ASSERT_TEST(words3[0] == "compile" && words3[1] == "compiler" && 
                words3[2] == "complete" && words3[3] == "computer",
                "Common prefix sort");
    
    // Test 4: Already sorted
    std::vector<std::string> words4 = {"a", "b", "c", "d"};
    sortLexicographic(words4);
    ASSERT_TEST(words4[0] == "a" && words4[3] == "d",
                "Already sorted array");
    
    // Test 5: Reverse sorted
    std::vector<std::string> words5 = {"d", "c", "b", "a"};
    sortLexicographic(words5);
    ASSERT_TEST(words5[0] == "a" && words5[3] == "d",
                "Reverse sorted array");
}

void testFrequencySorting() {
    std::cout << "\nTesting Frequency-Based Sorting..." << std::endl;
    
    // Test 1: Basic frequency sort (descending)
    std::vector<std::pair<std::string, int>> words1 = {
        {"apple", 100},
        {"banana", 500},
        {"cherry", 300}
    };
    sortByFrequency(words1);
    ASSERT_TEST(words1[0].first == "banana" && words1[0].second == 500,
                "Highest frequency first");
    ASSERT_TEST(words1[2].first == "apple" && words1[2].second == 100,
                "Lowest frequency last");
    
    // Test 2: Equal frequencies (should sort lexicographically)
    std::vector<std::pair<std::string, int>> words2 = {
        {"zebra", 100},
        {"apple", 100},
        {"banana", 100}
    };
    sortByFrequency(words2);
    ASSERT_TEST(words2[0].first == "apple",
                "Equal frequency sorted alphabetically");
    
    // Test 3: Mixed frequencies
    std::vector<std::pair<std::string, int>> words3 = {
        {"program", 450},
        {"programming", 430},
        {"programmer", 420},
        {"programs", 410},
        {"code", 400}
    };
    sortByFrequency(words3);
    ASSERT_TEST(words3[0].first == "program" && words3[4].first == "code",
                "Descending frequency order");
}

void testCorrectionRanking() {
    std::cout << "\nTesting Correction Ranking (Distance + Frequency)..." << std::endl;
    
    // Test 1: Sort by distance (lower is better)
    std::vector<CorrectionCandidate> corrections1;
    corrections1.push_back(CorrectionCandidate("program", 1, 450));
    corrections1.push_back(CorrectionCandidate("progarm", 2, 100));
    corrections1.push_back(CorrectionCandidate("programmer", 3, 420));
    
    sortByRank(corrections1);
    ASSERT_TEST(corrections1[0].word == "program" && corrections1[0].distance == 1,
                "Lower distance ranked first");
    
    // Test 2: Equal distance, sort by frequency
    std::vector<CorrectionCandidate> corrections2;
    corrections2.push_back(CorrectionCandidate("apple", 1, 100));
    corrections2.push_back(CorrectionCandidate("apply", 1, 200));
    corrections2.push_back(CorrectionCandidate("ample", 1, 150));
    
    sortByRank(corrections2);
    ASSERT_TEST(corrections2[0].word == "apply" && corrections2[0].frequency == 200,
                "Higher frequency ranked first when distance is equal");
    ASSERT_TEST(corrections2[2].word == "apple" && corrections2[2].frequency == 100,
                "Lower frequency ranked last when distance is equal");
    
    // Test 3: Complex ranking
    std::vector<CorrectionCandidate> corrections3;
    corrections3.push_back(CorrectionCandidate("algorithm", 1, 250));  // Best: distance=1, high freq
    corrections3.push_back(CorrectionCandidate("algorithms", 2, 230)); // Second: distance=2
    corrections3.push_back(CorrectionCandidate("algebraic", 3, 160));  // Third: distance=3
    
    sortByRank(corrections3);
    ASSERT_TEST(corrections3[0].word == "algorithm",
                "Best correction ranked first (distance 1, high freq)");
    ASSERT_TEST(corrections3[1].word == "algorithms",
                "Second best correction");
}

void testEdgeCases() {
    std::cout << "\nTesting Edge Cases..." << std::endl;
    
    // Test 1: Empty array
    std::vector<std::string> empty;
    sortLexicographic(empty);
    ASSERT_TEST(empty.size() == 0, "Empty array handling");
    
    // Test 2: Single element
    std::vector<std::string> single = {"word"};
    sortLexicographic(single);
    ASSERT_TEST(single.size() == 1 && single[0] == "word", "Single element array");
    
    // Test 3: Two elements
    std::vector<std::string> two = {"zebra", "apple"};
    sortLexicographic(two);
    ASSERT_TEST(two[0] == "apple" && two[1] == "zebra", "Two element sort");
    
    // Test 4: Duplicate words
    std::vector<std::string> dups = {"apple", "banana", "apple", "cherry", "banana"};
    sortLexicographic(dups);
    ASSERT_TEST(dups[0] == "apple" && dups[1] == "apple", "Duplicate handling");
}

void testPerformance() {
    std::cout << "\nTesting Performance (Large Dataset)..." << std::endl;
    
    // Create large dataset
    std::vector<std::string> largeDataset;
    for (int i = 1000; i >= 1; i--) {
        largeDataset.push_back("word" + std::to_string(i));
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    sortLexicographic(largeDataset);
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    ASSERT_TEST(largeDataset[0] == "word1" && largeDataset[999] == "word999",
                "Large dataset sorted correctly");
    
    std::cout << "  ⏱️  Sorted 1000 words in " << duration.count() << "ms" << std::endl;
    ASSERT_TEST(duration.count() < 100, "Performance acceptable (<100ms)");
}

// ==================== MAIN TEST RUNNER ====================

int main() {
    std::cout << "=====================================" << std::endl;
    std::cout << "   SORTING ALGORITHMS TESTS" << std::endl;
    std::cout << "=====================================" << std::endl;
    
    testLexicographicSorting();
    testFrequencySorting();
    testCorrectionRanking();
    testEdgeCases();
    testPerformance();
    
    std::cout << "\n=====================================" << std::endl;
    std::cout << "TEST RESULTS:" << std::endl;
    std::cout << "  Passed: " << tests_passed << std::endl;
    std::cout << "  Failed: " << tests_failed << std::endl;
    std::cout << "=====================================" << std::endl;
    
    if (tests_failed == 0) {
        std::cout << "\n✓ ALL SORTING TESTS PASSED!" << std::endl;
        std::cout << "=====================================" << std::endl;
        return 0;
    } else {
        std::cout << "\n✗ SOME TESTS FAILED!" << std::endl;
        std::cout << "=====================================" << std::endl;
        return 1;
    }
}
