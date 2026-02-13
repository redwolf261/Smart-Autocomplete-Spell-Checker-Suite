#include "filehandler.h"
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <direct.h>  // For _mkdir on Windows

std::vector<std::pair<std::string, int>> FileHandler::readDictionary(const std::string& filename) {
    std::vector<std::pair<std::string, int>> words;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        return words;  // Return empty vector if file can't be opened
    }
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        // Check if line contains frequency (word frequency_count)
        size_t spacePos = line.find(' ');
        if (spacePos != std::string::npos) {
            std::string word = line.substr(0, spacePos);
            int frequency = std::stoi(line.substr(spacePos + 1));
            words.push_back({word, frequency});
        } else {
            // No frequency specified, default to 1
            words.push_back({line, 1});
        }
    }
    
    file.close();
    return words;
}

std::vector<std::string> FileHandler::readWordList(const std::string& filename) {
    std::vector<std::string> words;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        return words;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            words.push_back(line);
        }
    }
    
    file.close();
    return words;
}

std::string FileHandler::readQuery(const std::string& filename) {
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        return "";
    }
    
    std::string query;
    std::getline(file, query);
    
    file.close();
    return query;
}

void FileHandler::writeAutocompleteOutput(
    const std::string& filename,
    const std::string& query,
    const std::vector<std::string>& suggestions,
    double timeTaken,
    bool cacheHit,
    const std::vector<std::string>& dsUsed) {
    
    std::stringstream json;
    json << "{\n";
    json << "  \"type\": \"autocomplete\",\n";
    json << "  \"query\": \"" << escapeJSON(query) << "\",\n";
    json << "  \"suggestions\": " << vectorToJSON(suggestions) << ",\n";
    json << "  \"count\": " << suggestions.size() << ",\n";
    json << "  \"timeTaken\": " << timeTaken << ",\n";
    json << "  \"cacheHit\": " << (cacheHit ? "true" : "false") << ",\n";
    json << "  \"dataStructuresUsed\": " << vectorToJSON(dsUsed) << "\n";
    json << "}";
    
    writeJSON(filename, json.str());
}

void FileHandler::writeSpellCheckOutput(
    const std::string& filename,
    const std::string& query,
    bool isCorrect,
    const std::vector<std::string>& corrections,
    double timeTaken) {
    
    std::stringstream json;
    json << "{\n";
    json << "  \"type\": \"spellcheck\",\n";
    json << "  \"query\": \"" << escapeJSON(query) << "\",\n";
    json << "  \"isCorrect\": " << (isCorrect ? "true" : "false") << ",\n";
    json << "  \"corrections\": " << vectorToJSON(corrections) << ",\n";
    json << "  \"timeTaken\": " << timeTaken << "\n";
    json << "}";
    
    writeJSON(filename, json.str());
}

void FileHandler::writeJSON(const std::string& filename, const std::string& jsonContent) {
    std::ofstream file(filename);
    
    if (!file.is_open()) {
        return;
    }
    
    file << jsonContent;
    file.close();
}

bool FileHandler::fileExists(const std::string& filename) {
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}

bool FileHandler::createDirectory(const std::string& path) {
    // Platform-specific directory creation
    #ifdef _WIN32
        return _mkdir(path.c_str()) == 0;
    #else
        return mkdir(path.c_str(), 0755) == 0;
    #endif
}

std::string FileHandler::vectorToJSON(const std::vector<std::string>& vec) {
    if (vec.empty()) {
        return "[]";
    }
    
    std::stringstream json;
    json << "[";
    
    for (size_t i = 0; i < vec.size(); i++) {
        json << "\"" << escapeJSON(vec[i]) << "\"";
        if (i < vec.size() - 1) {
            json << ", ";
        }
    }
    
    json << "]";
    return json.str();
}

std::string FileHandler::escapeJSON(const std::string& str) {
    std::string result;
    
    for (char c : str) {
        switch (c) {
            case '"':  result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\b': result += "\\b";  break;
            case '\f': result += "\\f";  break;
            case '\n': result += "\\n";  break;
            case '\r': result += "\\r";  break;
            case '\t': result += "\\t";  break;
            default:   result += c;      break;
        }
    }
    
    return result;
}
