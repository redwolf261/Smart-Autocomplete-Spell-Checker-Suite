#include "bktree.h"
#include <algorithm>

// ==================== BKNode Implementation ====================

BKNode::~BKNode() {
    for (auto& pair : children) {
        delete pair.second;
    }
}

// ==================== BKTree Implementation ====================

BKTree::BKTree() : root(nullptr) {
}

BKTree::~BKTree() {
    destroyTree(root);
}

void BKTree::destroyTree(BKNode* node) {
    if (node == nullptr) return;
    
    for (auto& pair : node->children) {
        destroyTree(pair.second);
    }
    
    delete node;
}

void BKTree::insert(const std::string& word) {
    if (word.empty()) return;
    
    if (root == nullptr) {
        root = new BKNode(word);
        return;
    }
    
    insertHelper(root, word);
}

void BKTree::insertHelper(BKNode* node, const std::string& word) {
    int distance = EditDistance::calculate(node->word, word);
    
    // If distance is 0, word already exists
    if (distance == 0) {
        return;
    }
    
    // If child with this distance doesn't exist, create it
    if (node->children.find(distance) == node->children.end()) {
        node->children[distance] = new BKNode(word);
    } else {
        // Recursively insert into the appropriate subtree
        insertHelper(node->children[distance], word);
    }
}

std::vector<std::pair<std::string, int>> BKTree::searchByDistance(
    const std::string& word, int maxDistance) {
    
    std::vector<std::pair<std::string, int>> results;
    
    if (root == nullptr) {
        return results;
    }
    
    searchHelper(root, word, maxDistance, results);
    
    // Sort by distance
    std::sort(results.begin(), results.end(),
              [](const auto& a, const auto& b) { return a.second < b.second; });
    
    return results;
}

void BKTree::searchHelper(BKNode* node, const std::string& word, int maxDistance,
                         std::vector<std::pair<std::string, int>>& results) {
    if (node == nullptr) return;
    
    int distance = EditDistance::calculate(node->word, word);
    
    // If within maxDistance, add to results
    if (distance <= maxDistance) {
        results.push_back({node->word, distance});
    }
    
    // Triangle inequality: only search children with distances in range
    // [distance - maxDistance, distance + maxDistance]
    for (int d = distance - maxDistance; d <= distance + maxDistance; d++) {
        if (d > 0 && node->children.find(d) != node->children.end()) {
            searchHelper(node->children[d], word, maxDistance, results);
        }
    }
}

std::vector<std::pair<std::string, int>> BKTree::findClosest(
    const std::string& word, int maxResults) {
    
    std::vector<std::pair<std::string, int>> results;
    
    // Start with distance 0 and gradually increase
    for (int distance = 0; distance <= 10 && results.size() < static_cast<size_t>(maxResults); distance++) {
        std::vector<std::pair<std::string, int>> candidates = searchByDistance(word, distance);
        
        for (const auto& candidate : candidates) {
            if (results.size() >= static_cast<size_t>(maxResults)) break;
            
            // Avoid duplicates
            bool found = false;
            for (const auto& existing : results) {
                if (existing.first == candidate.first) {
                    found = true;
                    break;
                }
            }
            
            if (!found) {
                results.push_back(candidate);
            }
        }
    }
    
    return results;
}

int BKTree::size() {
    return countNodes(root);
}

int BKTree::countNodes(BKNode* node) {
    if (node == nullptr) return 0;
    
    int count = 1;
    for (auto& pair : node->children) {
        count += countNodes(pair.second);
    }
    
    return count;
}

void BKTree::clear() {
    destroyTree(root);
    root = nullptr;
}
