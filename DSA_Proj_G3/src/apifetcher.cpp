#include "apifetcher.h"
#include <sstream>
#include <iomanip>
#include <windows.h>
#include <wininet.h>

#pragma comment(lib, "wininet.lib")

namespace APIFetcher {

    static std::string lastError;

    // ==================== HTTP UTILITIES ====================
    
    std::string httpGet(const std::string& url) {
        std::string result;
        lastError.clear();

        // Initialize WinINet
        HINTERNET hInternet = InternetOpenA(
            "SmartAutocomplete/1.0",
            INTERNET_OPEN_TYPE_PRECONFIG,
            NULL,
            NULL,
            0
        );

        if (!hInternet) {
            lastError = "Failed to initialize WinINet";
            return "";
        }

        // Open URL
        HINTERNET hConnect = InternetOpenUrlA(
            hInternet,
            url.c_str(),
            NULL,
            0,
            INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE,
            0
        );

        if (!hConnect) {
            lastError = "Failed to connect to URL";
            InternetCloseHandle(hInternet);
            return "";
        }

        // Read response
        char buffer[4096];
        DWORD bytesRead;

        while (InternetReadFile(hConnect, buffer, sizeof(buffer) - 1, &bytesRead) && bytesRead > 0) {
            buffer[bytesRead] = '\0';
            result += buffer;
        }

        // Cleanup
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);

        return result;
    }

    std::string urlEncode(const std::string& str) {
        std::ostringstream encoded;
        encoded.fill('0');
        encoded << std::hex;

        for (char c : str) {
            if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
                encoded << c;
            } else {
                encoded << std::uppercase;
                encoded << '%' << std::setw(2) << int((unsigned char)c);
                encoded << std::nouppercase;
            }
        }

        return encoded.str();
    }

    // ==================== JSON PARSING ====================
    
    std::string extractJSONValue(const std::string& json, const std::string& key) {
        std::string searchKey = "\"" + key + "\":";
        size_t pos = json.find(searchKey);
        
        if (pos == std::string::npos) {
            return "";
        }

        pos += searchKey.length();
        
        // Skip whitespace
        while (pos < json.length() && (json[pos] == ' ' || json[pos] == '\t')) {
            pos++;
        }

        if (pos >= json.length()) return "";

        // Check if value is a string
        if (json[pos] == '"') {
            pos++; // Skip opening quote
            size_t endPos = json.find('"', pos);
            if (endPos != std::string::npos) {
                return json.substr(pos, endPos - pos);
            }
        }
        // Check if value is a number
        else if (isdigit(json[pos]) || json[pos] == '-') {
            size_t endPos = pos;
            while (endPos < json.length() && 
                   (isdigit(json[endPos]) || json[endPos] == '.' || json[endPos] == '-')) {
                endPos++;
            }
            return json.substr(pos, endPos - pos);
        }

        return "";
    }

    std::vector<std::string> extractJSONArray(const std::string& json, const std::string& arrayName) {
        std::vector<std::string> result;
        
        std::string searchKey = "\"" + arrayName + "\":[";
        size_t startPos = json.find(searchKey);
        
        if (startPos == std::string::npos) {
            // Try without quotes (Datamuse returns bare arrays)
            startPos = json.find("[");
            if (startPos == std::string::npos) return result;
        } else {
            startPos += searchKey.length() - 1;
        }

        size_t endPos = json.find(']', startPos);
        if (endPos == std::string::npos) return result;

        std::string arrayContent = json.substr(startPos + 1, endPos - startPos - 1);

        // Parse objects in array
        size_t objStart = 0;
        int braceCount = 0;

        for (size_t i = 0; i < arrayContent.length(); i++) {
            if (arrayContent[i] == '{') {
                if (braceCount == 0) objStart = i;
                braceCount++;
            } else if (arrayContent[i] == '}') {
                braceCount--;
                if (braceCount == 0) {
                    result.push_back(arrayContent.substr(objStart, i - objStart + 1));
                }
            }
        }

        return result;
    }

    // ==================== DATAMUSE API ====================
    
    std::vector<WordSuggestion> fetchAutocompleteSuggestions(const std::string& prefix, int maxResults) {
        std::vector<WordSuggestion> suggestions;

        if (prefix.empty()) return suggestions;

        // Build URL
        std::string url = "https://api.datamuse.com/sug?s=" + urlEncode(prefix) + 
                         "&max=" + std::to_string(maxResults);

        // Fetch data
        std::string response = httpGet(url);
        
        if (response.empty()) {
            return suggestions;
        }

        // Parse JSON array
        std::vector<std::string> items = extractJSONArray(response, "");

        for (const std::string& item : items) {
            std::string word = extractJSONValue(item, "word");
            std::string scoreStr = extractJSONValue(item, "score");
            
            if (!word.empty()) {
                int score = scoreStr.empty() ? 100 : std::stoi(scoreStr);
                suggestions.push_back(WordSuggestion(word, score));
            }
        }

        return suggestions;
    }

    std::vector<WordSuggestion> fetchSpellingSuggestions(const std::string& word, int maxResults) {
        std::vector<WordSuggestion> suggestions;

        if (word.empty()) return suggestions;

        // Build URL (use spelling API endpoint)
        std::string url = "https://api.datamuse.com/words?sp=" + urlEncode(word) + 
                         "&max=" + std::to_string(maxResults);

        // Fetch data
        std::string response = httpGet(url);
        
        if (response.empty()) {
            return suggestions;
        }

        // Parse JSON array
        std::vector<std::string> items = extractJSONArray(response, "");

        for (const std::string& item : items) {
            std::string correctedWord = extractJSONValue(item, "word");
            std::string scoreStr = extractJSONValue(item, "score");
            
            if (!correctedWord.empty()) {
                int score = scoreStr.empty() ? 100 : std::stoi(scoreStr);
                suggestions.push_back(WordSuggestion(correctedWord, score));
            }
        }

        return suggestions;
    }

    // ==================== DICTIONARY API ====================
    
    DictionaryEntry fetchDefinition(const std::string& word) {
        DictionaryEntry entry;
        entry.word = word;

        if (word.empty()) return entry;

        // Build URL
        std::string url = "https://api.dictionaryapi.dev/api/v2/entries/en/" + urlEncode(word);

        // Fetch data
        std::string response = httpGet(url);
        
        if (response.empty() || response.find("\"title\":\"No Definitions Found\"") != std::string::npos) {
            return entry;
        }

        entry.found = true;

        // Extract phonetic
        entry.phonetic = extractJSONValue(response, "phonetic");

        // Extract audio URL
        entry.audioUrl = extractJSONValue(response, "audio");

        // Extract first definition
        size_t defStart = response.find("\"definition\":");
        if (defStart != std::string::npos) {
            entry.definition = extractJSONValue(response.substr(defStart), "definition");
        }

        return entry;
    }

    // ==================== ERROR HANDLING ====================
    
    bool isAPIAvailable() {
        std::string response = httpGet("https://api.datamuse.com/sug?s=test&max=1");
        return !response.empty();
    }

    std::string getLastError() {
        return lastError;
    }

} // namespace APIFetcher
