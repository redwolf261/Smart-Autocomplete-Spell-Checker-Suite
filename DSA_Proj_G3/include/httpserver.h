#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <string>
#include <functional>
#include <map>
#include "autocomplete.h"
#include "spellchecker.h"

/**
 * Lightweight HTTP Server for Version 3.0
 * Handles frontend requests and returns JSON responses
 * 
 * Features:
 * - GET/POST request handling
 * - JSON request/response parsing
 * - Integration with AutocompleteEngine and SpellChecker
 * - CORS support for local development
 */

class HTTPServer {
private:
    int port;
    bool running;
    AutocompleteEngine* autocomplete;
    SpellChecker* spellchecker;

    // Helper: Parse HTTP request
    struct HTTPRequest {
        std::string method;      // GET, POST
        std::string path;        // /query, /api/autocomplete
        std::map<std::string, std::string> headers;
        std::string body;
    };

    // Helper: Build HTTP response
    struct HTTPResponse {
        int statusCode;          // 200, 404, 500
        std::string statusText;  // OK, Not Found, Internal Server Error
        std::map<std::string, std::string> headers;
        std::string body;
    };

    /**
     * Parse incoming HTTP request
     * @param rawRequest: Raw HTTP request string
     * @return: Parsed HTTPRequest object
     */
    HTTPRequest parseRequest(const std::string& rawRequest);

    /**
     * Build HTTP response string
     * @param response: HTTPResponse object
     * @return: Formatted HTTP response string
     */
    std::string buildResponse(const HTTPResponse& response);

    /**
     * Handle /query endpoint
     * @param requestBody: JSON request body {query: "word", mode: "autocomplete"}
     * @return: JSON response with suggestions
     */
    std::string handleQuery(const std::string& requestBody);

    /**
     * Handle /health endpoint
     * @return: JSON health status
     */
    std::string handleHealth();

    /**
     * Extract JSON value by key
     * @param json: JSON string
     * @param key: Key to extract
     * @return: Value string
     */
    std::string extractJSONValue(const std::string& json, const std::string& key);

    /**
     * Build JSON array from vector of strings
     * @param items: Vector of strings
     * @return: JSON array string ["item1", "item2"]
     */
    std::string buildJSONArray(const std::vector<std::string>& items);

    /**
     * Build JSON array from vector of pairs
     * @param items: Vector of word-score pairs
     * @return: JSON array string [{"word": "hello", "score": 10}]
     */
    std::string buildJSONArray(const std::vector<std::pair<std::string, int>>& items);

public:
    /**
     * Constructor
     * @param serverPort: Port to listen on (default: 8080)
     * @param autoEngine: Pointer to AutocompleteEngine
     * @param spellCheck: Pointer to SpellChecker
     */
    HTTPServer(int serverPort, AutocompleteEngine* autoEngine, SpellChecker* spellCheck);

    /**
     * Destructor
     */
    ~HTTPServer();

    /**
     * Start the HTTP server
     * Blocks until stop() is called or Ctrl+C
     */
    void start();

    /**
     * Stop the HTTP server
     */
    void stop();

    /**
     * Check if server is running
     */
    bool isRunning() const { return running; }

    /**
     * Get server port
     */
    int getPort() const { return port; }
};

#endif // HTTPSERVER_H
