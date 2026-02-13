// ===========================
// TRIE VISUALIZATION
// ===========================

class TrieVisualizer {
    constructor(containerSelector) {
        this.container = document.querySelector(containerSelector);
        this.nodes = [];
    }

    clear() {
        this.container.innerHTML = '';
        this.nodes = [];
    }

    visualizeTraversal(prefix) {
        this.clear();
        
        if (!prefix) {
            this.showPlaceholder();
            return;
        }

        // Create visual representation
        const vizDiv = document.createElement('div');
        vizDiv.style.padding = '20px';
        vizDiv.style.textAlign = 'center';

        // Title
        const title = document.createElement('div');
        title.style.marginBottom = '20px';
        title.style.fontSize = '1.1em';
        title.style.fontWeight = 'bold';
        title.style.color = '#667eea';
        title.textContent = `Trie Traversal: "${prefix}"`;
        vizDiv.appendChild(title);

        // Path visualization
        const pathDiv = document.createElement('div');
        pathDiv.style.display = 'flex';
        pathDiv.style.justifyContent = 'center';
        pathDiv.style.alignItems = 'center';
        pathDiv.style.flexWrap = 'wrap';
        pathDiv.style.gap = '10px';

        // Root node
        pathDiv.appendChild(this.createNode('ROOT', 'root'));
        
        // Arrow
        pathDiv.appendChild(this.createArrow());

        // Create nodes for each character in prefix
        for (let i = 0; i < prefix.length; i++) {
            const char = prefix[i].toUpperCase();
            const nodeType = i === prefix.length - 1 ? 'active' : 'visited';
            pathDiv.appendChild(this.createNode(char, nodeType));
            
            if (i < prefix.length - 1) {
                pathDiv.appendChild(this.createArrow());
            }
        }

        vizDiv.appendChild(pathDiv);

        // Statistics
        const stats = document.createElement('div');
        stats.style.marginTop = '30px';
        stats.style.padding = '15px';
        stats.style.background = '#f0f0f0';
        stats.style.borderRadius = '8px';
        stats.style.textAlign = 'left';
        
        stats.innerHTML = `
            <div style="margin-bottom: 8px;"><strong>Depth:</strong> ${prefix.length}</div>
            <div style="margin-bottom: 8px;"><strong>Nodes Traversed:</strong> ${prefix.length + 1}</div>
            <div style="margin-bottom: 8px;"><strong>Time Complexity:</strong> O(${prefix.length})</div>
            <div><strong>Space Complexity:</strong> O(1) for traversal</div>
        `;
        
        vizDiv.appendChild(stats);

        this.container.appendChild(vizDiv);

        // Animate nodes
        this.animateNodes();
    }

    createNode(label, type) {
        const node = document.createElement('div');
        node.className = `trie-node ${type}`;
        node.textContent = label;
        node.style.opacity = '0';
        this.nodes.push(node);
        return node;
    }

    createArrow() {
        const arrow = document.createElement('div');
        arrow.textContent = '→';
        arrow.style.fontSize = '24px';
        arrow.style.color = '#667eea';
        arrow.style.opacity = '0';
        this.nodes.push(arrow);
        return arrow;
    }

    animateNodes() {
        this.nodes.forEach((node, index) => {
            setTimeout(() => {
                node.style.transition = 'opacity 0.3s, transform 0.3s';
                node.style.opacity = '1';
            }, index * 100);
        });
    }

    showPlaceholder() {
        this.container.innerHTML = `
            <div class="viz-placeholder">
                <p>🌳 Trie Traversal</p>
                <p>Type something to see the path</p>
            </div>
        `;
    }
}

// ===========================
// CACHE VISUALIZATION
// ===========================

class CacheVisualizer {
    constructor() {
        this.entries = [];
        this.maxSize = 5;
    }

    addEntry(key, value) {
        // Remove if already exists (move to front)
        this.entries = this.entries.filter(e => e.key !== key);
        
        // Add to front
        this.entries.unshift({ key, value, timestamp: Date.now() });
        
        // Keep only maxSize entries
        if (this.entries.length > this.maxSize) {
            this.entries.pop();
        }
    }

    visualize() {
        // Could be used to show cache state in UI
        return this.entries;
    }
}

// ===========================
// BLOOM FILTER VISUALIZATION
// ===========================

class BloomFilterVisualizer {
    constructor(size = 20) {
        this.size = size;
        this.bits = new Array(size).fill(false);
    }

    addWord(word) {
        // Simulate hash functions
        const hash1 = this.simpleHash(word, 31) % this.size;
        const hash2 = this.simpleHash(word, 37) % this.size;
        const hash3 = this.simpleHash(word, 41) % this.size;
        
        this.bits[hash1] = true;
        this.bits[hash2] = true;
        this.bits[hash3] = true;
    }

    simpleHash(str, prime) {
        let hash = 0;
        for (let i = 0; i < str.length; i++) {
            hash = (hash * prime + str.charCodeAt(i)) % this.size;
        }
        return Math.abs(hash);
    }

    visualize() {
        return this.bits.map((bit, i) => ({
            index: i,
            value: bit ? 1 : 0,
            color: bit ? '#667eea' : '#ddd'
        }));
    }
}

// ===========================
// PRIORITY QUEUE VISUALIZATION
// ===========================

class PriorityQueueVisualizer {
    constructor() {
        this.heap = [];
    }

    insert(word, frequency) {
        this.heap.push({ word, frequency });
        this.heapifyUp(this.heap.length - 1);
    }

    heapifyUp(index) {
        if (index === 0) return;
        
        const parentIndex = Math.floor((index - 1) / 2);
        if (this.heap[index].frequency > this.heap[parentIndex].frequency) {
            [this.heap[index], this.heap[parentIndex]] = 
            [this.heap[parentIndex], this.heap[index]];
            this.heapifyUp(parentIndex);
        }
    }

    visualizeHeap() {
        // Create tree structure for visualization
        const levels = Math.ceil(Math.log2(this.heap.length + 1));
        return {
            levels,
            nodes: this.heap,
            maxFrequency: this.heap[0]?.frequency || 0
        };
    }
}

// ===========================
// MAIN VISUALIZATION FUNCTION
// ===========================

const trieViz = new TrieVisualizer('#vizContainer');
const cacheViz = new CacheVisualizer();
const bloomViz = new BloomFilterVisualizer();
const pqViz = new PriorityQueueVisualizer();

function visualizeTrieTraversal(prefix) {
    trieViz.visualizeTraversal(prefix);
}

function visualizeDataStructures(query, results) {
    // Update visualizations based on query and results
    
    // Trie traversal
    trieViz.visualizeTraversal(query);
    
    // Cache update
    cacheViz.addEntry(query, JSON.stringify(results));
    
    // Bloom filter update
    if (state.currentMode === 'spellcheck') {
        bloomViz.addWord(query);
    }
    
    // Priority queue for ranking
    if (results.suggestions) {
        results.suggestions.forEach((word, i) => {
            pqViz.insert(word, results.suggestions.length - i);
        });
    }
}

// ===========================
// EDIT DISTANCE VISUALIZATION
// ===========================

class EditDistanceVisualizer {
    static visualize(word1, word2) {
        const m = word1.length;
        const n = word2.length;
        
        // Create DP table
        const dp = Array(m + 1).fill(0).map(() => Array(n + 1).fill(0));
        
        for (let i = 0; i <= m; i++) dp[i][0] = i;
        for (let j = 0; j <= n; j++) dp[0][j] = j;
        
        for (let i = 1; i <= m; i++) {
            for (let j = 1; j <= n; j++) {
                if (word1[i-1] === word2[j-1]) {
                    dp[i][j] = dp[i-1][j-1];
                } else {
                    dp[i][j] = 1 + Math.min(
                        dp[i-1][j],    // Delete
                        dp[i][j-1],    // Insert
                        dp[i-1][j-1]   // Replace
                    );
                }
            }
        }
        
        return {
            distance: dp[m][n],
            table: dp,
            word1,
            word2
        };
    }

    static renderTable(container, result) {
        let html = '<table style="border-collapse: collapse; margin: 20px auto;">';
        
        // Header row
        html += '<tr><th></th><th></th>';
        for (let char of result.word2) {
            html += `<th style="padding: 8px; border: 1px solid #ddd;">${char}</th>`;
        }
        html += '</tr>';
        
        // Data rows
        for (let i = 0; i < result.table.length; i++) {
            html += '<tr>';
            html += `<th style="padding: 8px; border: 1px solid #ddd;">${i === 0 ? '' : result.word1[i-1]}</th>`;
            for (let j = 0; j < result.table[i].length; j++) {
                const value = result.table[i][j];
                const color = value === 0 ? '#4CAF50' : value <= 2 ? '#FFC107' : '#f44336';
                html += `<td style="padding: 8px; border: 1px solid #ddd; background: ${color}; color: white; text-align: center;">${value}</td>`;
            }
            html += '</tr>';
        }
        
        html += '</table>';
        html += `<div style="text-align: center; margin-top: 10px;">Edit Distance: <strong>${result.distance}</strong></div>`;
        
        container.innerHTML = html;
    }
}

// ===========================
// PERFORMANCE METRICS
// ===========================

class PerformanceMetrics {
    constructor() {
        this.queries = [];
        this.cacheHits = 0;
        this.cacheMisses = 0;
    }

    recordQuery(query, timeTaken, cacheHit) {
        this.queries.push({
            query,
            timeTaken,
            cacheHit,
            timestamp: Date.now()
        });

        if (cacheHit) {
            this.cacheHits++;
        } else {
            this.cacheMisses++;
        }
    }

    getAverageTime() {
        if (this.queries.length === 0) return 0;
        const sum = this.queries.reduce((acc, q) => acc + q.timeTaken, 0);
        return (sum / this.queries.length).toFixed(2);
    }

    getCacheHitRate() {
        const total = this.cacheHits + this.cacheMisses;
        if (total === 0) return 0;
        return ((this.cacheHits / total) * 100).toFixed(2);
    }

    getReport() {
        return {
            totalQueries: this.queries.length,
            averageTime: this.getAverageTime(),
            cacheHitRate: this.getCacheHitRate(),
            cacheHits: this.cacheHits,
            cacheMisses: this.cacheMisses
        };
    }
}

const performanceMetrics = new PerformanceMetrics();

// Export for use in app.js
window.visualizeTrieTraversal = visualizeTrieTraversal;
window.visualizeDataStructures = visualizeDataStructures;
window.EditDistanceVisualizer = EditDistanceVisualizer;
window.performanceMetrics = performanceMetrics;
