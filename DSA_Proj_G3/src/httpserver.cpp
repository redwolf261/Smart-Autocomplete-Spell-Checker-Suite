#include "httpserver.h"
#include "utils.h"
#include "apifetcher.h"
#include <iostream>
#include <sstream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

HTTPServer::HTTPServer(int serverPort, AutocompleteEngine* autoEngine, SpellChecker* spellCheck)
    : port(serverPort), running(false), autocomplete(autoEngine), spellchecker(spellCheck) {
}

HTTPServer::~HTTPServer() {
    stop();
}

HTTPServer::HTTPRequest HTTPServer::parseRequest(const std::string& rawRequest) {
    HTTPRequest req;
    std::istringstream stream(rawRequest);
    std::string line;

    // Parse request line (GET /path HTTP/1.1)
    if (std::getline(stream, line)) {
        std::istringstream lineStream(line);
        lineStream >> req.method >> req.path;
    }

    // Parse headers
    while (std::getline(stream, line) && line != "\r" && !line.empty()) {
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 2); // Skip ": "
            // Remove \r if present
            if (!value.empty() && value.back() == '\r') {
                value.pop_back();
            }
            req.headers[key] = value;
        }
    }

    // Parse body (rest of the stream)
    std::string body;
    while (std::getline(stream, line)) {
        body += line;
    }
    req.body = body;

    return req;
}

std::string HTTPServer::buildResponse(const HTTPResponse& response) {
    std::ostringstream oss;

    // Status line
    oss << "HTTP/1.1 " << response.statusCode << " " << response.statusText << "\r\n";

    // Headers
    for (const auto& header : response.headers) {
        oss << header.first << ": " << header.second << "\r\n";
    }

    // Blank line
    oss << "\r\n";

    // Body
    oss << response.body;

    return oss.str();
}

std::string HTTPServer::extractJSONValue(const std::string& json, const std::string& key) {
    std::string searchKey = "\"" + key + "\"";
    size_t pos = json.find(searchKey);
    
    if (pos == std::string::npos) {
        return "";
    }

    // Find the colon after the key
    size_t colonPos = json.find(':', pos);
    if (colonPos == std::string::npos) {
        return "";
    }

    // Skip whitespace and quotes
    size_t valueStart = colonPos + 1;
    while (valueStart < json.length() && (json[valueStart] == ' ' || json[valueStart] == '"')) {
        valueStart++;
    }

    // Find value end (quote or comma or })
    size_t valueEnd = valueStart;
    bool inQuotes = (json[colonPos + 1] == '"' || json[valueStart - 1] == '"');
    
    if (inQuotes) {
        while (valueEnd < json.length() && json[valueEnd] != '"') {
            valueEnd++;
        }
    } else {
        while (valueEnd < json.length() && json[valueEnd] != ',' && json[valueEnd] != '}') {
            valueEnd++;
        }
    }

    return json.substr(valueStart, valueEnd - valueStart);
}

std::string HTTPServer::buildJSONArray(const std::vector<std::string>& items) {
    std::ostringstream oss;
    oss << "[";
    
    for (size_t i = 0; i < items.size(); i++) {
        oss << "\"" << items[i] << "\"";
        if (i < items.size() - 1) {
            oss << ",";
        }
    }
    
    oss << "]";
    return oss.str();
}

std::string HTTPServer::buildJSONArray(const std::vector<std::pair<std::string, int>>& items) {
    std::ostringstream oss;
    oss << "[";
    
    for (size_t i = 0; i < items.size(); i++) {
        oss << "{\"word\":\"" << items[i].first << "\",\"score\":" << items[i].second << "}";
        if (i < items.size() - 1) {
            oss << ",";
        }
    }
    
    oss << "]";
    return oss.str();
}

std::string HTTPServer::handleQuery(const std::string& requestBody) {
    try {
        // Extract query and mode from JSON
        std::string query = extractJSONValue(requestBody, "query");
        std::string mode = extractJSONValue(requestBody, "mode");

        if (query.empty()) {
            return "{\"error\":\"Missing query parameter\",\"suggestions\":[]}";
        }

        std::ostringstream response;
        response << "{";
        response << "\"query\":\"" << query << "\",";
        response << "\"mode\":\"" << mode << "\",";

        if (mode == "autocomplete") {
            // Fetch live autocomplete suggestions from Datamuse API
            Utils::logInfo("Fetching autocomplete from Datamuse API for: " + query);
            
            auto apiSuggestions = APIFetcher::fetchAutocompleteSuggestions(query);
            
            if (apiSuggestions.empty()) {
                // Fallback to local Trie if API fails
                Utils::logWarning("API failed, using local Trie");
                auto localSuggestions = autocomplete->getSuggestions(query);
                response << "\"suggestions\":" << buildJSONArray(localSuggestions) << ",";
                response << "\"count\":" << localSuggestions.size() << ",";
                response << "\"source\":\"local\"";
            } else {
                // Convert WordSuggestion to pairs for JSON builder
                std::vector<std::pair<std::string, int>> scored;
                for (size_t i = 0; i < apiSuggestions.size() && i < 10; i++) {
                    scored.push_back(std::make_pair(apiSuggestions[i].word, apiSuggestions[i].score));
                }
                response << "\"suggestions\":" << buildJSONArray(scored) << ",";
                response << "\"count\":" << scored.size() << ",";
                response << "\"source\":\"datamuse-api\"";
            }
            
        } else if (mode == "spellcheck") {
            // Check if word exists using DictionaryAPI
            Utils::logInfo("Checking spelling via DictionaryAPI for: " + query);
            
            auto definition = APIFetcher::fetchDefinition(query);
            bool isCorrect = definition.found;
            
            response << "\"isCorrect\":" << (isCorrect ? "true" : "false") << ",";
            
            if (!isCorrect) {
                // Fetch spelling suggestions from Datamuse API
                auto spellingCorrections = APIFetcher::fetchSpellingSuggestions(query);
                
                if (spellingCorrections.empty()) {
                    // Fallback to local BK-Tree
                    Utils::logWarning("API failed, using local BK-Tree");
                    auto localCorrections = spellchecker->getCorrections(query, 5);
                    std::vector<std::pair<std::string, int>> scored;
                    for (size_t i = 0; i < localCorrections.size(); i++) {
                        scored.push_back(std::make_pair(localCorrections[i], 10 - static_cast<int>(i)));
                    }
                    response << "\"suggestions\":" << buildJSONArray(scored) << ",";
                    response << "\"count\":" << localCorrections.size() << ",";
                    response << "\"source\":\"local\"";
                } else {
                    // Convert WordSuggestion to pairs for JSON builder
                    std::vector<std::pair<std::string, int>> scored;
                    for (size_t i = 0; i < spellingCorrections.size() && i < 5; i++) {
                        scored.push_back(std::make_pair(spellingCorrections[i].word, spellingCorrections[i].score));
                    }
                    response << "\"suggestions\":" << buildJSONArray(scored) << ",";
                    response << "\"count\":" << scored.size() << ",";
                    response << "\"source\":\"datamuse-api\"";
                }
            } else {
                response << "\"suggestions\":[],";
                response << "\"count\":0,";
                response << "\"source\":\"dictionaryapi\"";
            }
            
        } else {
            response << "\"error\":\"Invalid mode (use 'autocomplete' or 'spellcheck')\",";
            response << "\"suggestions\":[]";
        }

        response << "}";
        return response.str();
    } catch (const std::exception& e) {
        Utils::logError(std::string("Query error: ") + e.what());
        return "{\"error\":\"Internal server error\",\"suggestions\":[]}";
    }
}

std::string HTTPServer::handleHealth() {
    std::ostringstream response;
    response << "{";
    response << "\"status\":\"healthy\",";
    response << "\"service\":\"DSA Autocomplete API\",";
    response << "\"version\":\"3.0\",";
    response << "\"port\":" << port;
    response << "}";
    return response.str();
}

void HTTPServer::start() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        Utils::logError("WSAStartup failed: " + std::to_string(result));
        return;
    }

    // Create socket
    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET) {
        Utils::logError("Socket creation failed: " + std::to_string(WSAGetLastError()));
        WSACleanup();
        return;
    }

    // Bind socket
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        Utils::logError("Bind failed: " + std::to_string(WSAGetLastError()));
        closesocket(listenSocket);
        WSACleanup();
        return;
    }

    // Listen
    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        Utils::logError("Listen failed: " + std::to_string(WSAGetLastError()));
        closesocket(listenSocket);
        WSACleanup();
        return;
    }

    running = true;
    Utils::logInfo("HTTP Server started on http://localhost:" + std::to_string(port));
    Utils::logInfo("Press Ctrl+C to stop...");

    // Main server loop
    while (running) {
        // Accept connection
        SOCKET clientSocket = accept(listenSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            Utils::logError("Accept failed: " + std::to_string(WSAGetLastError()));
            continue;
        }

        // Receive request
        char buffer[4096];
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            std::string rawRequest(buffer);

            // Parse request
            HTTPRequest req = parseRequest(rawRequest);

            // Build response
            HTTPResponse res;
            res.headers["Content-Type"] = "application/json";
            res.headers["Access-Control-Allow-Origin"] = "*";  // CORS
            res.headers["Access-Control-Allow-Methods"] = "GET, POST, OPTIONS";
            res.headers["Access-Control-Allow-Headers"] = "Content-Type";

            // Handle OPTIONS preflight
            if (req.method == "OPTIONS") {
                res.statusCode = 200;
                res.statusText = "OK";
                res.body = "";
            }
            // Handle /query endpoint
            else if (req.path == "/query" || req.path == "/api/query") {
                res.statusCode = 200;
                res.statusText = "OK";
                res.body = handleQuery(req.body);
            }
            // Handle /health endpoint
            else if (req.path == "/health" || req.path == "/api/health") {
                res.statusCode = 200;
                res.statusText = "OK";
                res.body = handleHealth();
            }
            // 404 Not Found
            else {
                res.statusCode = 404;
                res.statusText = "Not Found";
                res.body = "{\"error\":\"Endpoint not found\"}";
            }

            // Send response
            std::string responseStr = buildResponse(res);
            send(clientSocket, responseStr.c_str(), responseStr.length(), 0);
        }

        // Close connection
        closesocket(clientSocket);
    }

    // Cleanup
    closesocket(listenSocket);
    WSACleanup();
    Utils::logInfo("HTTP Server stopped");
}

void HTTPServer::stop() {
    running = false;
}
