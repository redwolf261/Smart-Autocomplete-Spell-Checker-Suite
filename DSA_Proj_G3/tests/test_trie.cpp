#include "../include/trie.h"
#include <iostream>
#include <cassert>
#include <vector>

void testInsertAndSearch() {
    std::cout << "Testing Trie Insert and Search..." << std::endl;
    
    Trie trie;
    
    // Insert words
    trie.insertWord("algorithm", 100);
    trie.insertWord("auto", 90);
    trie.insertWord("autocomplete", 95);
    trie.insertWord("computer", 120);
    
    // Test search - should find
    assert(trie.searchWord("algorithm") == true);
    assert(trie.searchWord("auto") == true);
    assert(trie.searchWord("autocomplete") == true);
    assert(trie.searchWord("computer") == true);
    
    // Test search - should not find
    assert(trie.searchWord("algor") == false);
    assert(trie.searchWord("compute") == false);
    assert(trie.searchWord("autocomplet") == false);
    assert(trie.searchWord("xyz") == false);
    
    std::cout << "✓ Insert and Search tests passed!" << std::endl;
}

void testPrefixSearch() {
    std::cout << "Testing Trie Prefix Search..." << std::endl;
    
    Trie trie;
    
    trie.insertWord("algorithm", 100);
    trie.insertWord("algebra", 80);
    trie.insertWord("algebraic", 70);
    trie.insertWord("auto", 90);
    trie.insertWord("autocomplete", 95);
    trie.insertWord("automation", 85);
    
    // Test prefix "alg"
    auto results1 = trie.searchPrefix("alg");
    assert(results1.size() == 3);
    
    // Test prefix "aut"
    auto results2 = trie.searchPrefix("aut");
    assert(results2.size() == 3);
    
    // Test prefix "algo"
    auto results3 = trie.searchPrefix("algo");
    assert(results3.size() == 1);
    
    // Test non-existent prefix
    auto results4 = trie.searchPrefix("xyz");
    assert(results4.size() == 0);
    
    std::cout << "✓ Prefix Search tests passed!" << std::endl;
}

void testSuggestWords() {
    std::cout << "Testing Trie Suggest Words..." << std::endl;
    
    Trie trie;
    
    trie.insertWord("code", 150);
    trie.insertWord("coding", 140);
    trie.insertWord("coder", 130);
    trie.insertWord("codec", 120);
    
    // Get suggestions for "cod"
    auto suggestions = trie.suggestWords("cod", 3);
    
    // Should return top 3 by frequency
    assert(suggestions.size() == 3);
    assert(suggestions[0] == "code");      // freq: 150
    assert(suggestions[1] == "coding");    // freq: 140
    assert(suggestions[2] == "coder");     // freq: 130
    
    std::cout << "✓ Suggest Words tests passed!" << std::endl;
}

void testFrequencyUpdate() {
    std::cout << "Testing Trie Frequency Update..." << std::endl;
    
    Trie trie;
    
    trie.insertWord("test", 10);
    
    // Update frequency
    trie.updateFrequency("test", 5);
    
    auto results = trie.searchPrefix("tes");
    assert(results.size() == 1);
    assert(results[0].second == 15);  // 10 + 5
    
    std::cout << "✓ Frequency Update tests passed!" << std::endl;
}

void testWordCount() {
    std::cout << "Testing Trie Word Count..." << std::endl;
    
    Trie trie;
    
    assert(trie.getWordCount() == 0);
    
    trie.insertWord("word1", 1);
    assert(trie.getWordCount() == 1);
    
    trie.insertWord("word2", 1);
    trie.insertWord("word3", 1);
    assert(trie.getWordCount() == 3);
    
    std::cout << "✓ Word Count tests passed!" << std::endl;
}

void testCaseInsensitive() {
    std::cout << "Testing Trie Case Insensitivity..." << std::endl;
    
    Trie trie;
    
    trie.insertWord("Test", 10);
    
    assert(trie.searchWord("test") == true);
    assert(trie.searchWord("TEST") == true);
    assert(trie.searchWord("TeSt") == true);
    
    std::cout << "✓ Case Insensitivity tests passed!" << std::endl;
}

int main() {
    std::cout << "====================================" << std::endl;
    std::cout << "    TRIE DATA STRUCTURE TESTS      " << std::endl;
    std::cout << "====================================" << std::endl;
    std::cout << std::endl;
    
    try {
        testInsertAndSearch();
        testPrefixSearch();
        testSuggestWords();
        testFrequencyUpdate();
        testWordCount();
        testCaseInsensitive();
        
        std::cout << std::endl;
        std::cout << "====================================" << std::endl;
        std::cout << "  ✓ ALL TRIE TESTS PASSED!         " << std::endl;
        std::cout << "====================================" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "✗ Test failed: " << e.what() << std::endl;
        return 1;
    }
}
