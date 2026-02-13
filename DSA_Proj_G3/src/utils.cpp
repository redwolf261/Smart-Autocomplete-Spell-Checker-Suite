#include "utils.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <numeric>
#include <cmath>
#include <ctime>
#include <cctype>
#include <cstdlib>
#include <algorithm>

// ==================== Timer Implementation ====================

Utils::Timer::Timer() : running(false) {
}

void Utils::Timer::start() {
    startTime = std::chrono::high_resolution_clock::now();
    running = true;
}

void Utils::Timer::stop() {
    endTime = std::chrono::high_resolution_clock::now();
    running = false;
}

double Utils::Timer::elapsedMilliseconds() {
    auto end = running ? std::chrono::high_resolution_clock::now() : endTime;
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - startTime);
    return duration.count() / 1000.0;
}

double Utils::Timer::elapsedMicroseconds() {
    auto end = running ? std::chrono::high_resolution_clock::now() : endTime;
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - startTime);
    return duration.count();
}

void Utils::Timer::reset() {
    running = false;
}

// ==================== String Utilities ====================

std::string Utils::toLower(const std::string& str) {
    std::string result = str;
    for (char& c : result) {
        c = std::tolower(static_cast<unsigned char>(c));
    }
    return result;
}

std::string Utils::toUpper(const std::string& str) {
    std::string result = str;
    for (char& c : result) {
        c = std::toupper(static_cast<unsigned char>(c));
    }
    return result;
}

std::string Utils::trim(const std::string& str) {
    size_t start = 0;
    size_t end = str.length();
    
    while (start < end && std::isspace(static_cast<unsigned char>(str[start]))) start++;
    while (end > start && std::isspace(static_cast<unsigned char>(str[end - 1]))) end--;
    
    return str.substr(start, end - start);
}

bool Utils::isAlpha(const std::string& str) {
    for (char c : str) {
        if (!std::isalpha(static_cast<unsigned char>(c))) return false;
    }
    return !str.empty();
}

std::vector<std::string> Utils::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    
    return tokens;
}

std::string Utils::join(const std::vector<std::string>& vec, const std::string& delimiter) {
    if (vec.empty()) return "";
    
    std::string result = vec[0];
    for (size_t i = 1; i < vec.size(); i++) {
        result += delimiter + vec[i];
    }
    
    return result;
}

// ==================== Logging Utilities ====================

void Utils::log(const std::string& message, const std::string& level) {
    std::cout << "[" << getCurrentTimestamp() << "] [" << level << "] " << message << std::endl;
}

void Utils::logError(const std::string& message) {
    log(message, "ERROR");
}

void Utils::logWarning(const std::string& message) {
    log(message, "WARNING");
}

void Utils::logInfo(const std::string& message) {
    log(message, "INFO");
}

void Utils::logDebug(const std::string& message) {
    log(message, "DEBUG");
}

std::string Utils::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// ==================== Formatting Utilities ====================

std::string Utils::formatNumber(int number) {
    std::string numStr = std::to_string(number);
    std::string result;
    
    int count = 0;
    for (int i = numStr.length() - 1; i >= 0; i--) {
        if (count > 0 && count % 3 == 0) {
            result = "," + result;
        }
        result = numStr[i] + result;
        count++;
    }
    
    return result;
}

std::string Utils::formatDouble(double value, int precision) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(precision) << value;
    return ss.str();
}

// ==================== Statistics Utilities ====================

double Utils::average(const std::vector<double>& values) {
    if (values.empty()) return 0.0;
    
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    return sum / values.size();
}

double Utils::standardDeviation(const std::vector<double>& values) {
    if (values.empty()) return 0.0;
    
    double avg = average(values);
    double variance = 0.0;
    
    for (double value : values) {
        variance += (value - avg) * (value - avg);
    }
    
    variance /= values.size();
    return std::sqrt(variance);
}

double Utils::getMemoryUsageMB() {
    // This is a placeholder - actual implementation would be platform-specific
    return 0.0;
}

// ==================== Display Utilities ====================

void Utils::printSeparator(int length, char ch) {
    std::cout << std::string(length, ch) << std::endl;
}

void Utils::printHeader(const std::string& title) {
    printSeparator(60, '=');
    std::cout << "  " << title << std::endl;
    printSeparator(60, '=');
}

std::string Utils::randomString(int length) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyz";
    std::string result;
    
    for (int i = 0; i < length; i++) {
        result += charset[std::rand() % (sizeof(charset) - 1)];
    }
    
    return result;
}

bool Utils::startsWith(const std::string& str, const std::string& prefix) {
    if (str.length() < prefix.length()) return false;
    return str.substr(0, prefix.length()) == prefix;
}

bool Utils::endsWith(const std::string& str, const std::string& suffix) {
    if (str.length() < suffix.length()) return false;
    return str.substr(str.length() - suffix.length()) == suffix;
}
