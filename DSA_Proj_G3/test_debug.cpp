#include <iostream>
#include ""../include/trie.h""

int main() {
    Trie trie;
    std::cout << ""Initial count: "" << trie.getWordCount() << std::endl;
    
    trie.insertWord(""word1"", 1);
    std::cout << ""After word1: "" << trie.getWordCount() << std::endl;
    
    trie.insertWord(""word2"", 1);
    std::cout << ""After word2: "" << trie.getWordCount() << std::endl;
    
    trie.insertWord(""word3"", 1);
    std::cout << ""After word3: "" << trie.getWordCount() << std::endl;
    
    return 0;
}
