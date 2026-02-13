#ifndef APIFETCHER_H
#define APIFETCHER_H

#include <string>
#include <vector>
#include <utility>

/**
 * API Fetcher Module
 * Fetches live word data from external APIs (Datamuse & Dictionary API)
 * Uses Windows API for HTTP requests (no external dependencies)
 */

namespace APIFetcher {

    // ==================== DATA STRUCTURES ====================
    
    struct WordSuggestion {
        std::string word;
        int score;
        
        WordSuggestion(const std::string& w, int s) : word(w), score(s) {}
    };

    struct DictionaryEntry {
        std::string word;
        std::string phonetic;
        std::string definition;
        std::string audioUrl;
        bool found;
        
        DictionaryEntry() : found(false) {}
    };

    // ==================== DATAMUSE API ====================
    
    /**
     * Fetch autocomplete suggestions from Datamuse API
     * API: https://api.datamuse.com/sug?s=<prefix>&max=10
     * @param prefix: Word prefix to search
     * @param maxResults: Maximum number of results (default: 10)
     * @return: Vector of word suggestions with scores
     */
    std::vector<WordSuggestion> fetchAutocompleteSuggestions(
        const std::string& prefix, 
        int maxResults = 10
    );

    /**
     * Fetch similar words for spell correction from Datamuse
     * API: https://api.datamuse.com/words?sp=<word>&max=10
     * @param word: Misspelled word
     * @param maxResults: Maximum number of results
     * @return: Vector of correction suggestions
     */
    std::vector<WordSuggestion> fetchSpellingSuggestions(
        const std::string& word,
        int maxResults = 10
    );

    // ==================== DICTIONARY API ====================
    
    /**
     * Fetch word definition from DictionaryAPI
     * API: https://api.dictionaryapi.dev/api/v2/entries/en/<word>
     * @param word: Word to look up
     * @return: DictionaryEntry with definition and phonetics
     */
    DictionaryEntry fetchDefinition(const std::string& word);

    // ==================== HTTP UTILITIES ====================
    
    /**
     * Make HTTP GET request using Windows API
     * @param url: Full URL to fetch
     * @return: Response body as string
     */
    std::string httpGet(const std::string& url);

    /**
     * Parse JSON response (lightweight parser for specific APIs)
     * @param json: JSON string
     * @return: Parsed data as key-value pairs
     */
    std::vector<std::pair<std::string, std::string>> parseJSON(const std::string& json);

    /**
     * Extract value from JSON by key
     * @param json: JSON string
     * @param key: Key to search for
     * @return: Value as string (empty if not found)
     */
    std::string extractJSONValue(const std::string& json, const std::string& key);

    /**
     * Extract array from JSON
     * @param json: JSON string
     * @param arrayName: Name of array field
     * @return: Vector of JSON objects as strings
     */
    std::vector<std::string> extractJSONArray(const std::string& json, const std::string& arrayName);

    // ==================== URL ENCODING ====================
    
    /**
     * URL encode a string for safe HTTP transmission
     * @param str: String to encode
     * @return: URL-encoded string
     */
    std::string urlEncode(const std::string& str);

    // ==================== ERROR HANDLING ====================
    
    /**
     * Check if API is available
     * @return: true if API responds, false otherwise
     */
    bool isAPIAvailable();

    /**
     * Get last error message
     * @return: Error description
     */
    std::string getLastError();

} // namespace APIFetcher

#endif // APIFETCHER_H
