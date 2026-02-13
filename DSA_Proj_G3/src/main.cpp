#include <iostream>
#include <string>
#include <fstream>
#include "spellchecker.h"
#include "utils.h"
#include "filehandler.h"

void printMenu() {
    Utils::printSeparator(60, '=');
    std::cout << "  SMART AUTOCOMPLETE & SPELL-CHECKER SUITE" << std::endl;
    Utils::printSeparator(60, '=');
    std::cout << "\n";
    std::cout << "  1. Autocomplete (prefix search)" << std::endl;
    std::cout << "  2. Spell Check (with corrections)" << std::endl;
    std::cout << "  3. Process query from file" << std::endl;
    std::cout << "  4. View statistics" << std::endl;
    std::cout << "  5. Clear cache" << std::endl;
    std::cout << "  6. Exit" << std::endl;
    std::cout << "\n";
    Utils::printSeparator(60, '-');
}

void handleAutocomplete(SpellChecker& checker) {
    std::string prefix;
    std::cout << "\nEnter prefix: ";
    std::getline(std::cin, prefix);
    
    if (prefix.empty()) {
        Utils::logWarning("Empty prefix provided");
        return;
    }
    
    Utils::Timer timer;
    timer.start();
    
    std::vector<std::string> suggestions = checker.autocomplete(prefix, 10);
    
    timer.stop();
    
    std::cout << "\n--- Autocomplete Results ---\n";
    std::cout << "Prefix: " << prefix << "\n";
    std::cout << "Time: " << Utils::formatDouble(timer.elapsedMilliseconds(), 2) << " ms\n\n";
    
    if (suggestions.empty()) {
        std::cout << "No suggestions found.\n";
    } else {
        std::cout << "Suggestions (" << suggestions.size() << "):\n";
        for (size_t i = 0; i < suggestions.size(); i++) {
            std::cout << "  " << (i + 1) << ". " << suggestions[i] << "\n";
        }
    }
    std::cout << "\n";
}

void handleSpellCheck(SpellChecker& checker) {
    std::string word;
    std::cout << "\nEnter word to check: ";
    std::getline(std::cin, word);
    
    if (word.empty()) {
        Utils::logWarning("Empty word provided");
        return;
    }
    
    Utils::Timer timer;
    timer.start();
    
    bool isCorrect = checker.checkSpelling(word);
    
    std::cout << "\n--- Spell Check Results ---\n";
    std::cout << "Word: " << word << "\n";
    std::cout << "Status: " << (isCorrect ? "✓ CORRECT" : "✗ INCORRECT") << "\n";
    
    if (!isCorrect) {
        std::vector<std::string> corrections = checker.getCorrections(word, 2, 5);
        
        if (!corrections.empty()) {
            std::cout << "\nSuggested corrections:\n";
            for (size_t i = 0; i < corrections.size(); i++) {
                std::cout << "  " << (i + 1) << ". " << corrections[i] << "\n";
            }
        } else {
            std::cout << "\nNo corrections found within edit distance 2.\n";
        }
    }
    
    timer.stop();
    std::cout << "\nTime: " << Utils::formatDouble(timer.elapsedMilliseconds(), 2) << " ms\n\n";
}

void handleFileProcessing(SpellChecker& checker) {
    std::cout << "\nQuery file path (default: data/query.txt): ";
    std::string queryFile;
    std::getline(std::cin, queryFile);
    
    if (queryFile.empty()) {
        queryFile = "data/query.txt";
    }
    
    std::cout << "Output file path (default: data/output.json): ";
    std::string outputFile;
    std::getline(std::cin, outputFile);
    
    if (outputFile.empty()) {
        outputFile = "data/output.json";
    }
    
    std::cout << "Mode (1=Autocomplete, 2=Spell Check): ";
    std::string modeStr;
    std::getline(std::cin, modeStr);
    
    bool isAutocomplete = (modeStr != "2");
    
    checker.processQuery(queryFile, outputFile, isAutocomplete);
    
    std::cout << "\nProcessed! Results written to: " << outputFile << "\n\n";
}

void handleStatistics(SpellChecker& checker) {
    std::cout << "\n";
    std::cout << checker.getStatistics();
    std::cout << "\n";
}

int main() {
    Utils::printHeader("SMART AUTOCOMPLETE & SPELL-CHECKER");
    
    // Initialize spell checker
    SpellChecker checker(100, 10000);
    
    // Load dictionary
    std::string dictFile = "data/dictionary.txt";
    
    Utils::logInfo("Initializing system...");
    
    int wordsLoaded = checker.loadDictionary(dictFile);
    
    if (wordsLoaded == 0) {
        Utils::logError("Failed to load dictionary from: " + dictFile);
        Utils::logInfo("Please ensure dictionary.txt exists in the data/ folder");
        Utils::logInfo("Creating sample dictionary...");
        
        // Create sample dictionary
        std::ofstream dictStream(dictFile);
        if (dictStream.is_open()) {
            dictStream << "algorithm 100\n";
            dictStream << "autocomplete 95\n";
            dictStream << "auto 90\n";
            dictStream << "automation 85\n";
            dictStream << "automatic 80\n";
            dictStream << "computer 120\n";
            dictStream << "computing 110\n";
            dictStream << "compile 100\n";
            dictStream << "compiler 95\n";
            dictStream << "code 150\n";
            dictStream << "coding 140\n";
            dictStream << "coder 130\n";
            dictStream << "program 200\n";
            dictStream << "programming 190\n";
            dictStream << "programmer 180\n";
            dictStream << "python 170\n";
            dictStream << "java 165\n";
            dictStream << "javascript 160\n";
            dictStream << "data 180\n";
            dictStream << "database 175\n";
            dictStream << "structure 170\n";
            dictStream << "string 160\n";
            dictStream << "search 155\n";
            dictStream << "sort 150\n";
            dictStream << "tree 145\n";
            dictStream << "trie 140\n";
            dictStream << "hash 135\n";
            dictStream << "heap 130\n";
            dictStream << "array 125\n";
            dictStream << "list 120\n";
            dictStream.close();
            
            Utils::logInfo("Sample dictionary created!");
            wordsLoaded = checker.loadDictionary(dictFile);
        }
    }
    
    if (wordsLoaded == 0) {
        Utils::logError("Unable to proceed without dictionary");
        return 1;
    }
    
    Utils::logInfo("System ready! Type 'help' for commands or use menu.");
    std::cout << "\n";
    
    // Main loop
    while (true) {
        printMenu();
        
        std::cout << "Select option (1-6): ";
        std::string choice;
        std::getline(std::cin, choice);
        
        if (choice == "1") {
            handleAutocomplete(checker);
        }
        else if (choice == "2") {
            handleSpellCheck(checker);
        }
        else if (choice == "3") {
            handleFileProcessing(checker);
        }
        else if (choice == "4") {
            handleStatistics(checker);
        }
        else if (choice == "5") {
            checker.clearCache();
            Utils::logInfo("Cache cleared!");
            std::cout << "\n";
        }
        else if (choice == "6" || choice == "exit" || choice == "quit") {
            std::cout << "\n";
            Utils::printSeparator(60, '=');
            Utils::logInfo("Thank you for using Smart Autocomplete & Spell-Checker!");
            Utils::printSeparator(60, '=');
            std::cout << "\n";
            break;
        }
        else {
            Utils::logWarning("Invalid option. Please choose 1-6.");
            std::cout << "\n";
        }
    }
    
    return 0;
}
