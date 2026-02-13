#include "editdistance.h"
#include <vector>
#include <algorithm>

int EditDistance::min3(int a, int b, int c) {
    return std::min(a, std::min(b, c));
}

int EditDistance::calculate(const std::string& word1, const std::string& word2) {
    int m = word1.length();
    int n = word2.length();
    
    // Create DP table
    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1));
    
    // Base cases: empty string transformations
    for (int i = 0; i <= m; i++) {
        dp[i][0] = i;  // Delete all characters from word1
    }
    
    for (int j = 0; j <= n; j++) {
        dp[0][j] = j;  // Insert all characters into word1
    }
    
    // Fill DP table
    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (tolower(word1[i-1]) == tolower(word2[j-1])) {
                // Characters match, no operation needed
                dp[i][j] = dp[i-1][j-1];
            } else {
                // Minimum of three operations:
                // 1. Replace: dp[i-1][j-1] + 1
                // 2. Delete:  dp[i-1][j] + 1
                // 3. Insert:  dp[i][j-1] + 1
                dp[i][j] = min3(
                    dp[i-1][j-1],  // Replace
                    dp[i-1][j],    // Delete
                    dp[i][j-1]     // Insert
                ) + 1;
            }
        }
    }
    
    return dp[m][n];
}

int EditDistance::calculateOptimized(const std::string& word1, const std::string& word2) {
    int m = word1.length();
    int n = word2.length();
    
    // Use only two rows instead of full matrix
    std::vector<int> prev(n + 1);
    std::vector<int> curr(n + 1);
    
    // Initialize first row
    for (int j = 0; j <= n; j++) {
        prev[j] = j;
    }
    
    // Fill row by row
    for (int i = 1; i <= m; i++) {
        curr[0] = i;
        
        for (int j = 1; j <= n; j++) {
            if (tolower(word1[i-1]) == tolower(word2[j-1])) {
                curr[j] = prev[j-1];
            } else {
                curr[j] = min3(
                    prev[j-1],   // Replace
                    prev[j],     // Delete
                    curr[j-1]    // Insert
                ) + 1;
            }
        }
        
        // Swap rows
        prev = curr;
    }
    
    return prev[n];
}

double EditDistance::similarity(const std::string& word1, const std::string& word2) {
    if (word1.empty() && word2.empty()) {
        return 1.0;
    }
    
    int distance = calculate(word1, word2);
    int maxLength = std::max(word1.length(), word2.length());
    
    return 1.0 - (static_cast<double>(distance) / maxLength);
}

bool EditDistance::isSimilar(const std::string& word1, const std::string& word2, int maxDistance) {
    return calculate(word1, word2) <= maxDistance;
}
