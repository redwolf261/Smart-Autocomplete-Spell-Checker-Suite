#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <chrono>
#include <vector>

/**
 * Utility Functions
 * 
 * Purpose: Provide helper functions for:
 * - Timing/profiling
 * - String manipulation
 * - Logging
 * - Statistics
 */
class Utils {
public:
    /**
     * Timer class for measuring execution time
     */
    class Timer {
    private:
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::high_resolution_clock::time_point endTime;
        bool running;

    public:
        Timer();
        
        /**
         * Start the timer
         */
        void start();

        /**
         * Stop the timer
         */
        void stop();

        /**
         * Get elapsed time in milliseconds
         */
        double elapsedMilliseconds();

        /**
         * Get elapsed time in microseconds
         */
        double elapsedMicroseconds();

        /**
         * Reset the timer
         */
        void reset();
    };

    /**
     * Convert string to lowercase
     */
    static std::string toLower(const std::string& str);

    /**
     * Convert string to uppercase
     */
    static std::string toUpper(const std::string& str);

    /**
     * Trim whitespace from both ends of string
     */
    static std::string trim(const std::string& str);

    /**
     * Check if string contains only alphabetic characters
     */
    static bool isAlpha(const std::string& str);

    /**
     * Split string by delimiter
     */
    static std::vector<std::string> split(const std::string& str, char delimiter);

    /**
     * Join vector of strings with delimiter
     */
    static std::string join(const std::vector<std::string>& vec, const std::string& delimiter);

    /**
     * Log message with timestamp
     */
    static void log(const std::string& message, const std::string& level = "INFO");

    /**
     * Log error message
     */
    static void logError(const std::string& message);

    /**
     * Log warning message
     */
    static void logWarning(const std::string& message);

    /**
     * Log info message
     */
    static void logInfo(const std::string& message);

    /**
     * Log debug message
     */
    static void logDebug(const std::string& message);

    /**
     * Get current timestamp as string
     */
    static std::string getCurrentTimestamp();

    /**
     * Format number with commas (e.g., 1000000 -> "1,000,000")
     */
    static std::string formatNumber(int number);

    /**
     * Format double with specified precision
     */
    static std::string formatDouble(double value, int precision = 2);

    /**
     * Calculate average of vector of doubles
     */
    static double average(const std::vector<double>& values);

    /**
     * Calculate standard deviation
     */
    static double standardDeviation(const std::vector<double>& values);

    /**
     * Get memory usage in MB (approximate)
     */
    static double getMemoryUsageMB();

    /**
     * Print a separator line
     */
    static void printSeparator(int length = 50, char ch = '=');

    /**
     * Print a header with formatting
     */
    static void printHeader(const std::string& title);

    /**
     * Generate random string of specified length
     */
    static std::string randomString(int length);

    /**
     * Check if string starts with prefix
     */
    static bool startsWith(const std::string& str, const std::string& prefix);

    /**
     * Check if string ends with suffix
     */
    static bool endsWith(const std::string& str, const std::string& suffix);
};

#endif // UTILS_H
