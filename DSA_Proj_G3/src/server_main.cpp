/**
 * DSA Project - Version 3.0: Full-Stack API Server
 * Group 3
 * 
 * This server integrates:
 * - C++ backend with manual DSA implementations
 * - Datamuse API for autocomplete
 * - DictionaryAPI for definitions
 * - HTTP server for frontend connectivity
 * 
 * Endpoints:
 * - POST /query - Get autocomplete/spellcheck suggestions
 * - GET /health - Server health check
 */

#include <iostream>
#include <csignal>
#include "httpserver.h"
#include "autocomplete.h"
#include "spellchecker.h"
#include "filehandler.h"
#include "utils.h"

// Global server pointer for signal handling
HTTPServer* globalServer = nullptr;

void signalHandler(int signal) {
    if (signal == SIGINT) {
        Utils::logInfo("\nReceived SIGINT, shutting down server...");
        if (globalServer) {
            globalServer->stop();
        }
    }
}

int main() {
    Utils::logInfo("=== DSA Project - Version 3.0: API Server ===");
    Utils::logInfo("Initializing data structures and APIs...\n");

    // Initialize SpellChecker with all DSA components
    SpellChecker* spellchecker = new SpellChecker();
    
    // Load dictionary
    int wordsLoaded = spellchecker->loadDictionary("data/dictionary.txt");
    if (wordsLoaded == 0) {
        Utils::logError("Failed to load dictionary! Server cannot start.");
        delete spellchecker;
        return 1;
    }

    // Initialize AutocompleteEngine with Trie and HashTable from SpellChecker
    // Note: We're creating a new instance for API integration
    Trie* trie = new Trie();
    HashTable* frequencyMap = new HashTable(1000);
    
    // Load words into autocomplete engine
    Utils::logInfo("Loading autocomplete data...");
    auto words = FileHandler::readDictionary("data/dictionary.txt");
    for (const auto& wordPair : words) {
        trie->insertWord(wordPair.first, wordPair.second);
        frequencyMap->insert(wordPair.first, wordPair.second);
    }

    AutocompleteEngine* autocomplete = new AutocompleteEngine(trie, frequencyMap, 10);
    
    Utils::logInfo("Autocomplete engine ready: " + std::to_string(autocomplete->getWordCount()) + " words\n");

    // Create HTTP server on port 8080
    const int SERVER_PORT = 8080;
    HTTPServer* server = new HTTPServer(SERVER_PORT, autocomplete, spellchecker);
    globalServer = server;

    // Register signal handler for graceful shutdown
    std::signal(SIGINT, signalHandler);

    // Display server info
    Utils::logInfo("Server Configuration:");
    Utils::logInfo("- Port: " + std::to_string(SERVER_PORT));
    Utils::logInfo("- Dictionary: " + std::to_string(wordsLoaded) + " words");
    Utils::logInfo("- Data Structures: Trie, HashTable, Bloom Filter, LRU Cache, BK-Tree");
    Utils::logInfo("- APIs: Datamuse (autocomplete), DictionaryAPI (definitions)");
    Utils::logInfo("");

    // Start server (blocks until Ctrl+C)
    server->start();

    // Cleanup
    delete server;
    delete autocomplete;
    delete trie;
    delete frequencyMap;
    delete spellchecker;

    Utils::logInfo("Server shutdown complete.");
    return 0;
}
