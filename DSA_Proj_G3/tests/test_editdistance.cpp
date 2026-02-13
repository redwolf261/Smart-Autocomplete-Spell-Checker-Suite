#include "../include/editdistance.h"
#include <iostream>
#include <cassert>

void testBasicDistance() {
    std::cout << "Testing Edit Distance - Basic Cases..." << std::endl;
    
    // Identical strings
    assert(EditDistance::calculate("test", "test") == 0);
    
    // One character difference
    assert(EditDistance::calculate("test", "tent") == 1);
    
    // Classic example: kitten -> sitting
    assert(EditDistance::calculate("kitten", "sitting") == 3);
    
    // Empty strings
    assert(EditDistance::calculate("", "") == 0);
    assert(EditDistance::calculate("abc", "") == 3);
    assert(EditDistance::calculate("", "xyz") == 3);
    
    std::cout << "✓ Basic Distance tests passed!" << std::endl;
}

void testInsertDeleteReplace() {
    std::cout << "Testing Edit Distance - Operations..." << std::endl;
    
    // Insert operation
    assert(EditDistance::calculate("cat", "cart") == 1);
    
    // Delete operation
    assert(EditDistance::calculate("cart", "cat") == 1);
    
    // Replace operation
    assert(EditDistance::calculate("cat", "bat") == 1);
    
    // Multiple operations
    assert(EditDistance::calculate("saturday", "sunday") == 3);
    
    std::cout << "✓ Operations tests passed!" << std::endl;
}

void testOptimizedVersion() {
    std::cout << "Testing Edit Distance - Optimized Algorithm..." << std::endl;
    
    // Test that optimized version gives same results
    assert(EditDistance::calculate("kitten", "sitting") == 
           EditDistance::calculateOptimized("kitten", "sitting"));
    
    assert(EditDistance::calculate("algorithm", "altruistic") == 
           EditDistance::calculateOptimized("algorithm", "altruistic"));
    
    std::cout << "✓ Optimized Algorithm tests passed!" << std::endl;
}

void testSimilarity() {
    std::cout << "Testing Edit Distance - Similarity Score..." << std::endl;
    
    // Identical strings should have similarity 1.0
    double sim1 = EditDistance::similarity("test", "test");
    assert(sim1 == 1.0);
    
    // Completely different strings should have low similarity
    double sim2 = EditDistance::similarity("abc", "xyz");
    assert(sim2 < 0.5);
    
    // Similar strings should have high similarity
    double sim3 = EditDistance::similarity("testing", "test");
    assert(sim3 > 0.5);
    
    std::cout << "✓ Similarity Score tests passed!" << std::endl;
}

void testIsSimilar() {
    std::cout << "Testing Edit Distance - isSimilar Function..." << std::endl;
    
    // Within distance 1
    assert(EditDistance::isSimilar("test", "tent", 1) == true);
    assert(EditDistance::isSimilar("test", "best", 1) == true);
    
    // Not within distance 1
    assert(EditDistance::isSimilar("test", "tests", 1) == true);
    assert(EditDistance::isSimilar("test", "testing", 1) == false);
    
    // Within distance 2
    assert(EditDistance::isSimilar("cat", "cart", 2) == true);
    assert(EditDistance::isSimilar("algorithm", "altruism", 2) == false);
    
    std::cout << "✓ isSimilar Function tests passed!" << std::endl;
}

void testCaseInsensitive() {
    std::cout << "Testing Edit Distance - Case Insensitivity..." << std::endl;
    
    // Should be case-insensitive
    assert(EditDistance::calculate("Test", "test") == 0);
    assert(EditDistance::calculate("TEST", "test") == 0);
    assert(EditDistance::calculate("TeSt", "TesT") == 0);
    
    std::cout << "✓ Case Insensitivity tests passed!" << std::endl;
}

void testSpellCheckScenarios() {
    std::cout << "Testing Edit Distance - Spell Check Scenarios..." << std::endl;
    
    // Common misspellings
    assert(EditDistance::calculate("recieve", "receive") == 2);  // ie <-> ei
    assert(EditDistance::calculate("occured", "occurred") == 1);  // missing r
    assert(EditDistance::calculate("seperate", "separate") == 1);  // e <-> a
    
    std::cout << "✓ Spell Check Scenarios tests passed!" << std::endl;
}

int main() {
    std::cout << "====================================" << std::endl;
    std::cout << " EDIT DISTANCE ALGORITHM TESTS     " << std::endl;
    std::cout << "====================================" << std::endl;
    std::cout << std::endl;
    
    try {
        testBasicDistance();
        testInsertDeleteReplace();
        testOptimizedVersion();
        testSimilarity();
        testIsSimilar();
        testCaseInsensitive();
        testSpellCheckScenarios();
        
        std::cout << std::endl;
        std::cout << "====================================" << std::endl;
        std::cout << " ✓ ALL EDIT DISTANCE TESTS PASSED! " << std::endl;
        std::cout << "====================================" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "✗ Test failed: " << e.what() << std::endl;
        return 1;
    }
}
