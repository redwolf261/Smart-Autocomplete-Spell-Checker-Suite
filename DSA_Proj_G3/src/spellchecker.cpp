#include "spellchecker.h"
#include "filehandler.h"
#include "utils.h"
#include "sort.h"
#include <algorithm>
#include <sstream>

SpellChecker::SpellChecker(int cacheSize, int bloomSize) 
    : cacheHits(0), cacheMisses(0), totalQueries(0) {
    
    trie = new Trie();
    bloomFilter = new BloomFilter(bloomSize, 4);
    frequencyTable = new HashTable(1000);
    cache = new LRUCache(cacheSize);
    bkTree = new BKTree();
}

SpellChecker::~SpellChecker() {
    delete trie;
    delete bloomFilter;
    delete frequencyTable;
    delete cache;
    delete bkTree;
}

int SpellChecker::loadDictionary(const std::string& filename) {
    Utils::logInfo("Loading dictionary from: " + filename);
    
    auto words = FileHandler::readDictionary(filename);
    
    if (words.empty()) {
        Utils::logError(std::string("Failed to load dictionary or dictionary is empty"));
        return 0;
    }
    
    // Populate all data structures
    for (const auto& wordPair : words) {
        const std::string& word = wordPair.first;
        int frequency = wordPair.second;
        trie->insertWord(word, frequency);
        bloomFilter->addWord(word);
        frequencyTable->insert(word, frequency);
        bkTree->insert(word);
    }
    
    Utils::logInfo("Dictionary loaded: " + Utils::formatNumber(words.size()) + " words");
    Utils::logInfo("Bloom Filter FPR: " + Utils::formatDouble(bloomFilter->getFalsePositiveRate() * 100, 4) + "%");
    
    return words.size();
}

std::vector<std::string> SpellChecker::autocomplete(const std::string& prefix, int maxResults) {
    totalQueries++;
    
    // Check cache first
    std::string cacheKey = "auto:" + prefix;
    std::string cachedResult;
    
    if (cache->get(cacheKey, cachedResult)) {
        cacheHits++;
        // Parse cached result
        return Utils::split(cachedResult, '|');
    }
    
    cacheMisses++;
    
    // Use Trie to get suggestions
    std::vector<std::string> suggestions = trie->suggestWords(prefix, maxResults);
    
    // Cache the result
    std::string resultStr = Utils::join(suggestions, std::string("|"));
    cache->put(cacheKey, resultStr);
    
    return suggestions;
}

bool SpellChecker::checkSpelling(const std::string& word) {
    // Quick check with Bloom Filter
    if (!bloomFilter->containsWord(word)) {
        return false;  // Definitely not in dictionary
    }
    
    // Confirm with Trie (Bloom filter might have false positives)
    return trie->searchWord(word);
}

std::vector<std::string> SpellChecker::getCorrections(
    const std::string& word, int maxDistance, int maxResults) {
    
    totalQueries++;
    
    // Check cache first
    std::string cacheKey = "spell:" + word;
    std::string cachedResult;
    
    if (cache->get(cacheKey, cachedResult)) {
        cacheHits++;
        return Utils::split(cachedResult, '|');
    }
    
    cacheMisses++;
    
    // Use BK-Tree to find corrections
    auto candidates = bkTree->searchByDistance(word, maxDistance);
    
    // Extract just the words (sorted by distance already)
    std::vector<std::string> corrections;
    for (size_t i = 0; i < candidates.size() && i < static_cast<size_t>(maxResults); i++) {
        corrections.push_back(candidates[i].first);
    }
    
    // Cache the result
    std::string resultStr = Utils::join(corrections, "|");
    cache->put(cacheKey, resultStr);
    
    return corrections;
}

void SpellChecker::updateFrequency(const std::string& word) {
    trie->updateFrequency(word, 1);
    frequencyTable->increment(word, 1);
}

std::vector<std::string> SpellChecker::rankByFrequency(
    const std::vector<std::pair<std::string, int>>& candidates,
    int maxResults) {
    
    // Create a mutable copy for sorting
    std::vector<std::pair<std::string, int>> sortedCandidates = candidates;
    
    // Use custom QuickSort by frequency (descending order)
    SortAlgorithms::sortByFrequency(sortedCandidates);
    
    // Extract top N results
    std::vector<std::string> ranked;
    size_t limit = static_cast<size_t>(maxResults) < sortedCandidates.size() ? maxResults : sortedCandidates.size();
    
    for (size_t i = 0; i < limit; i++) {
        ranked.push_back(sortedCandidates[i].first);
    }
    
    return ranked;
}

void SpellChecker::getCacheStats(int& hits, int& misses, double& hitRate) {
    hits = cacheHits;
    misses = cacheMisses;
    
    if (totalQueries > 0) {
        hitRate = (static_cast<double>(cacheHits) / totalQueries) * 100.0;
    } else {
        hitRate = 0.0;
    }
}

double SpellChecker::getBloomFilterFPR() {
    return bloomFilter->getFalsePositiveRate();
}

int SpellChecker::getDictionarySize() {
    return trie->getWordCount();
}

void SpellChecker::clearCache() {
    cache->clear();
    cacheHits = 0;
    cacheMisses = 0;
}

void SpellChecker::resetStats() {
    cacheHits = 0;
    cacheMisses = 0;
    totalQueries = 0;
}

std::string SpellChecker::getStatistics() {
    std::stringstream ss;
    
    ss << "=== SpellChecker Statistics ===\n";
    ss << "Dictionary Size: " << Utils::formatNumber(getDictionarySize()) << " words\n";
    ss << "Total Queries: " << Utils::formatNumber(totalQueries) << "\n";
    ss << "Cache Hits: " << cacheHits << "\n";
    ss << "Cache Misses: " << cacheMisses << "\n";
    
    if (totalQueries > 0) {
        double hitRate = (static_cast<double>(cacheHits) / totalQueries) * 100.0;
        ss << "Cache Hit Rate: " << Utils::formatDouble(hitRate, 2) << "%\n";
    }
    
    ss << "Bloom Filter FPR: " << Utils::formatDouble(getBloomFilterFPR() * 100, 4) << "%\n";
    ss << "BK-Tree Size: " << bkTree->size() << " nodes\n";
    
    return ss.str();
}

void SpellChecker::processQuery(
    const std::string& queryFile,
    const std::string& outputFile,
    bool isAutocomplete) {
    
    std::string query = FileHandler::readQuery(queryFile);
    
    if (query.empty()) {
        Utils::logError("Empty query from file: " + queryFile);
        return;
    }
    
    Utils::Timer timer;
    timer.start();
    
    if (isAutocomplete) {
        // Autocomplete mode
        std::vector<std::string> suggestions = autocomplete(query, 10);
        
        timer.stop();
        double timeTaken = timer.elapsedMilliseconds();
        
        bool cacheHit = (cacheHits > 0);
        std::vector<std::string> dsUsed = {"Trie", "LRU Cache", "Priority Queue"};
        
        FileHandler::writeAutocompleteOutput(outputFile, query, suggestions, 
                                            timeTaken, cacheHit, dsUsed);
        
        Utils::logInfo("Autocomplete for '" + query + "': " + 
                      std::to_string(suggestions.size()) + " suggestions (" +
                      Utils::formatDouble(timeTaken, 2) + " ms)");
    } else {
        // Spell check mode
        bool isCorrect = checkSpelling(query);
        std::vector<std::string> corrections;
        
        if (!isCorrect) {
            corrections = getCorrections(query, 2, 5);
        }
        
        timer.stop();
        double timeTaken = timer.elapsedMilliseconds();
        
        FileHandler::writeSpellCheckOutput(outputFile, query, isCorrect, 
                                          corrections, timeTaken);
        
        Utils::logInfo("Spell check for '" + query + "': " + 
                      (isCorrect ? "CORRECT" : "INCORRECT") + " (" +
                      Utils::formatDouble(timeTaken, 2) + " ms)");
    }
}
