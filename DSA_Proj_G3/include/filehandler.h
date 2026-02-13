#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <string>
#include <vector>

/**
 * FileHandler Utility
 * 
 * Purpose: Handle all file I/O operations for the spell checker system
 * - Read dictionary files
 * - Read query files
 * - Write output JSON files
 * - Parse and format data
 */
class FileHandler {
public:
    /**
     * Read dictionary file and return list of words
     * @param filename: Path to dictionary file (one word per line)
     * @return: Vector of words
     * 
     * Format: plain text, one word per line
     * Optional: word frequency_count (space-separated)
     */
    static std::vector<std::pair<std::string, int>> readDictionary(const std::string& filename);

    /**
     * Read simple word list (no frequencies)
     */
    static std::vector<std::string> readWordList(const std::string& filename);

    /**
     * Read query from file
     * @param filename: Path to query file
     * @return: Query string (prefix or word to check)
     */
    static std::string readQuery(const std::string& filename);

    /**
     * Write autocomplete results to JSON file
     * @param filename: Output file path
     * @param query: The original query
     * @param suggestions: List of suggested words
     * @param timeTaken: Time taken for the operation (milliseconds)
     * @param cacheHit: Whether result was from cache
     * @param dsUsed: Data structures used for this query
     */
    static void writeAutocompleteOutput(
        const std::string& filename,
        const std::string& query,
        const std::vector<std::string>& suggestions,
        double timeTaken,
        bool cacheHit,
        const std::vector<std::string>& dsUsed
    );

    /**
     * Write spell check results to JSON file
     * @param filename: Output file path
     * @param query: The original word to check
     * @param isCorrect: Whether the word is spelled correctly
     * @param corrections: List of suggested corrections (if misspelled)
     * @param timeTaken: Time taken for the operation (milliseconds)
     */
    static void writeSpellCheckOutput(
        const std::string& filename,
        const std::string& query,
        bool isCorrect,
        const std::vector<std::string>& corrections,
        double timeTaken
    );

    /**
     * Write general output to JSON file
     * @param filename: Output file path
     * @param jsonContent: JSON string content
     */
    static void writeJSON(const std::string& filename, const std::string& jsonContent);

    /**
     * Check if a file exists
     */
    static bool fileExists(const std::string& filename);

    /**
     * Create directory if it doesn't exist
     */
    static bool createDirectory(const std::string& path);

    /**
     * Convert vector of strings to JSON array string
     */
    static std::string vectorToJSON(const std::vector<std::string>& vec);

    /**
     * Escape string for JSON
     */
    static std::string escapeJSON(const std::string& str);
};

#endif // FILEHANDLER_H
