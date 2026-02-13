// ===========================
// APPLICATION STATE
// ===========================

const state = {
    currentMode: 'autocomplete',
    lastQuery: '',
    lastResults: [],
    queryCount: 0,
    cacheHits: 0
};

// ===========================
// DOM ELEMENTS
// ===========================

const searchInput = document.getElementById('searchInput');
const searchBtn = document.getElementById('searchBtn');
const resultsContainer = document.getElementById('resultsContainer');
const queryModeSpan = document.getElementById('queryMode');
const queryTimeSpan = document.getElementById('queryTime');
const modeBtns = document.querySelectorAll('.mode-btn');
const dsUsedList = document.getElementById('dsUsedList');
const dictionaryPanel = document.getElementById('dictionaryPanel');
const loadingSpinner = document.getElementById('loadingSpinner');
const themeToggle = document.getElementById('themeToggle');

// Stats elements
const statTime = document.getElementById('statTime');
const statCache = document.getElementById('statCache');
const statCount = document.getElementById('statCount');
const statDict = document.getElementById('statDict');

// ===========================
// EVENT LISTENERS
// ===========================

// Theme toggle
if (themeToggle) {
    themeToggle.addEventListener('click', () => {
        document.body.classList.toggle('dark-mode');
        themeToggle.textContent = document.body.classList.contains('dark-mode') ? '☀️' : '🌙';
        localStorage.setItem('theme', document.body.classList.contains('dark-mode') ? 'dark' : 'light');
    });
    
    // Load saved theme
    const savedTheme = localStorage.getItem('theme');
    if (savedTheme === 'dark') {
        document.body.classList.add('dark-mode');
        themeToggle.textContent = '☀️';
    }
}

// Initialize particles
createParticles();

// Mode switching
modeBtns.forEach(btn => {
    btn.addEventListener('click', () => {
        modeBtns.forEach(b => b.classList.remove('active'));
        btn.classList.add('active');
        state.currentMode = btn.dataset.mode;
        queryModeSpan.textContent = `Mode: ${state.currentMode === 'autocomplete' ? 'Autocomplete' : 'Spell Check'}`;
        searchInput.placeholder = state.currentMode === 'autocomplete' 
            ? 'Type to search...' 
            : 'Enter word to check...';
    });
});

// Real-time autocomplete
searchInput.addEventListener('input', (e) => {
    if (state.currentMode === 'autocomplete' && e.target.value.length > 0) {
        performanceTimer.start();
        handleSearch(e.target.value);
    } else if (e.target.value.length === 0) {
        clearResults();
    }
});

// Manual search button
searchBtn.addEventListener('click', () => {
    const query = searchInput.value.trim();
    if (query) {
        handleSearch(query);
    }
});

// Enter key support
searchInput.addEventListener('keypress', (e) => {
    if (e.key === 'Enter') {
        const query = searchInput.value.trim();
        if (query) {
            performanceTimer.start();
            handleSearch(query);
        }
    }
});

// ===========================
// PARTICLE BACKGROUND
// ===========================

function createParticles() {
    const particlesContainer = document.getElementById('particles');
    if (!particlesContainer) return;
    
    const particleCount = 20;
    
    for (let i = 0; i < particleCount; i++) {
        const particle = document.createElement('div');
        particle.classList.add('particle');
        
        // Random position
        particle.style.left = `${Math.random() * 100}%`;
        particle.style.top = `${Math.random() * 100}%`;
        
        // Random size
        const size = Math.random() * 15 + 5;
        particle.style.width = `${size}px`;
        particle.style.height = `${size}px`;
        
        // Random animation delay
        particle.style.animationDelay = `${Math.random() * 10}s`;
        particle.style.animationDuration = `${Math.random() * 10 + 10}s`;
        
        particlesContainer.appendChild(particle);
    }
}

// ===========================
// CLEAR RESULTS
// ===========================

function clearResults() {
    resultsContainer.innerHTML = '<div class="placeholder">Start typing to see suggestions...</div>';
    if (dictionaryPanel) {
        dictionaryAPI.clear(dictionaryPanel);
    }
}

// ===========================
// MAIN SEARCH HANDLER
// ===========================

async function handleSearch(query) {
    state.lastQuery = query;
    state.queryCount++;
    
    const startTime = performance.now();
    
    try {
        // Write query to file
        await writeQueryToFile(query);
        
        // Trigger backend processing (in real deployment, this would call the C++ binary)
        // For demo purposes, we'll simulate with mock data
        const results = await processQuery(query);
        
        const endTime = performance.now();
        const timeTaken = (endTime - startTime).toFixed(2);
        
        // Update UI
        displayResults(results, timeTaken);
        updateStats(results, timeTaken);
        
    } catch (error) {
        console.error('Search error:', error);
        showError('Failed to process query');
    }
}

// ===========================
// BACKEND COMMUNICATION
// ===========================

async function writeQueryToFile(query) {
    // In production, this would write to ../data/query.txt
    // For browser-based demo, we'll use localStorage
    localStorage.setItem('query', query);
    console.log('Query written:', query);
}

// ===========================
// BACKEND COMMUNICATION (VERSION 3.0)
// ===========================

const API_URL = 'http://localhost:8080';

async function processQuery(query) {
    try {
        // Call backend API
        const response = await fetch(`${API_URL}/query`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({
                query: query.toLowerCase(),
                mode: state.currentMode
            })
        });
        
        if (!response.ok) {
            throw new Error(`HTTP ${response.status}: ${response.statusText}`);
        }
        
        const data = await response.json();
        console.log('Backend response:', data);
        
        // Transform backend response to frontend format
        if (state.currentMode === 'autocomplete') {
            return {
                query: data.query,
                suggestions: data.suggestions.map(s => 
                    typeof s === 'string' ? s : s.word
                ),
                count: data.count
            };
        } else {
            // Spell-check mode
            const corrections = data.suggestions.map(s => 
                typeof s === 'string' ? s : s.word
            );
            console.log('Extracted corrections:', corrections);
            return {
                query: data.query,
                isCorrect: corrections.length === 0 || corrections[0] === data.query,
                corrections: corrections,
                count: data.count
            };
        }
        
    } catch (error) {
        console.error('API Error:', error);
        
        // Fallback to simulation if server is down
        console.warn('Falling back to client-side simulation');
        await sleep(50);
        
        if (state.currentMode === 'autocomplete') {
            return simulateAutocomplete(query);
        } else {
            return simulateSpellCheck(query);
        }
    }
}

async function readOutputFile() {
    // In production, this would read ../data/output.json
    const output = localStorage.getItem('output');
    return output ? JSON.parse(output) : null;
}

// ===========================
// RESULT DISPLAY
// ===========================

function displayResults(results, timeTaken) {
    queryTimeSpan.textContent = `Time: ${timeTaken} ms`;
    
    if (state.currentMode === 'autocomplete') {
        displayAutocompleteResults(results);
    } else {
        displaySpellCheckResults(results);
    }
    
    // Update visualization
    visualizeTrieTraversal(state.lastQuery);
}

function displayAutocompleteResults(results) {
    resultsContainer.innerHTML = '';
    
    if (!results.suggestions || results.suggestions.length === 0) {
        resultsContainer.innerHTML = '<div class="placeholder">No suggestions found</div>';
        return;
    }
    
    results.suggestions.forEach((word, index) => {
        const item = document.createElement('div');
        item.className = 'result-item';
        item.style.animationDelay = `${index * 0.05}s`;
        
        item.innerHTML = `
            <span class="result-word">${word}</span>
            <div class="result-meta">
                <span class="result-badge">#${index + 1}</span>
            </div>
        `;
        
        // Click to fetch dictionary definition
        item.addEventListener('click', async () => {
            searchInput.value = word;
            updateFrequency(word);
            await fetchWordDefinition(word);
        });
        
        resultsContainer.appendChild(item);
    });
    
    // Stop timer
    const elapsed = performanceTimer.stop();
    if (queryTimeSpan) {
        queryTimeSpan.textContent = `Time: ${PerformanceTimer.formatTime(elapsed)}`;
    }
}

// ===========================
// DICTIONARY INTEGRATION
// ===========================

async function fetchWordDefinition(word) {
    if (!dictionaryPanel) return;
    
    try {
        // Show loading state
        dictionaryAPI.showLoading(dictionaryPanel);
        
        // Fetch definition from API
        const definition = await dictionaryAPI.fetchDefinition(word);
        
        // Render definition
        dictionaryAPI.renderDefinition(definition, dictionaryPanel);
        
        // Scroll to dictionary panel
        dictionaryPanel.scrollIntoView({ behavior: 'smooth', block: 'nearest' });
        
    } catch (error) {
        console.error('Dictionary fetch error:', error);
        dictionaryAPI.showError(dictionaryPanel, 'Could not fetch definition. Please try again.');
    }
}

function displaySpellCheckResults(results) {
    resultsContainer.innerHTML = '';
    
    const statusDiv = document.createElement('div');
    statusDiv.className = 'result-item';
    statusDiv.style.borderLeftColor = results.isCorrect ? '#4CAF50' : '#f44336';
    
    statusDiv.innerHTML = `
        <span class="result-word">${results.query}</span>
        <div class="result-meta">
            <span class="result-badge" style="background: ${results.isCorrect ? '#4CAF50' : '#f44336'}">
                ${results.isCorrect ? '✓ CORRECT' : '✗ INCORRECT'}
            </span>
        </div>
    `;
    
    resultsContainer.appendChild(statusDiv);
    
    if (!results.isCorrect && results.corrections && results.corrections.length > 0) {
        const correctionsTitle = document.createElement('div');
        correctionsTitle.style.padding = '15px 5px';
        correctionsTitle.style.fontWeight = 'bold';
        correctionsTitle.style.color = '#667eea';
        correctionsTitle.textContent = 'Suggested corrections:';
        resultsContainer.appendChild(correctionsTitle);
        
        results.corrections.forEach((word, index) => {
            const item = document.createElement('div');
            item.className = 'result-item';
            item.style.animationDelay = `${(index + 1) * 0.05}s`;
            
            item.innerHTML = `
                <span class="result-word">${word}</span>
                <div class="result-meta">
                    <span class="result-badge">Suggestion ${index + 1}</span>
                </div>
            `;
            
            item.addEventListener('click', () => {
                searchInput.value = word;
            });
            
            resultsContainer.appendChild(item);
        });
    }
}

// ===========================
// STATISTICS UPDATE
// ===========================

function updateStats(results, timeTaken) {
    statTime.textContent = `${timeTaken} ms`;
    statCache.textContent = results.cacheHit ? 'Yes ✓' : 'No';
    
    if (state.currentMode === 'autocomplete') {
        statCount.textContent = results.suggestions ? results.suggestions.length : 0;
    } else {
        statCount.textContent = results.corrections ? results.corrections.length : 0;
    }
    
    // Update data structures used
    if (results.dataStructuresUsed) {
        dsUsedList.innerHTML = results.dataStructuresUsed
            .map(ds => `<li>${ds}</li>`)
            .join('');
    }
    
    // Update cache hit rate
    if (results.cacheHit) {
        state.cacheHits++;
    }
}

// ===========================
// HELPER FUNCTIONS
// ===========================

function sleep(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}

function showError(message) {
    resultsContainer.innerHTML = `
        <div class="placeholder" style="color: #f44336;">
            ⚠️ ${message}
        </div>
    `;
}

function updateFrequency(word) {
    // In production, this would update the backend
    console.log('Frequency updated for:', word);
}

// ===========================
// MOCK DATA FOR DEMO
// ===========================

function simulateAutocomplete(prefix) {
    const dictionary = {
        'alg': ['algorithm', 'algebra', 'algebraic'],
        'aut': ['autocomplete', 'auto', 'automation', 'automatic', 'automotive'],
        'com': ['computer', 'computing', 'compile', 'compiler', 'complete'],
        'cod': ['code', 'coding', 'coder', 'codec'],
        'pro': ['program', 'programming', 'programmer', 'progress', 'project'],
        'pyt': ['python'],
        'jav': ['java', 'javascript'],
        'dat': ['data', 'database', 'datatype'],
        'str': ['string', 'structure', 'stream'],
        'sea': ['search', 'seal'],
        'sor': ['sort', 'sorted'],
        'tre': ['tree', 'trie'],
        'has': ['hash', 'hashtable'],
        'hea': ['heap', 'header'],
        'arr': ['array', 'arrange'],
        'lis': ['list', 'listen']
    };
    
    const prefixLower = prefix.toLowerCase().substring(0, 3);
    const suggestions = dictionary[prefixLower] || [];
    
    // Filter by full prefix
    const filtered = suggestions.filter(word => 
        word.toLowerCase().startsWith(prefix.toLowerCase())
    );
    
    return {
        type: 'autocomplete',
        query: prefix,
        suggestions: filtered,
        count: filtered.length,
        timeTaken: (Math.random() * 5 + 1).toFixed(2),
        cacheHit: Math.random() > 0.6,
        dataStructuresUsed: ['Trie', 'LRU Cache', 'Priority Queue']
    };
}

function simulateSpellCheck(word) {
    const commonWords = ['algorithm', 'autocomplete', 'computer', 'programming', 
                         'python', 'java', 'javascript', 'data', 'structure'];
    
    const isCorrect = commonWords.includes(word.toLowerCase());
    
    let corrections = [];
    if (!isCorrect) {
        // Simulate finding similar words
        corrections = commonWords
            .filter(w => Math.abs(w.length - word.length) <= 2)
            .slice(0, 3);
    }
    
    return {
        type: 'spellcheck',
        query: word,
        isCorrect: isCorrect,
        corrections: corrections,
        timeTaken: (Math.random() * 3 + 0.5).toFixed(2),
        dataStructuresUsed: ['Bloom Filter', 'BK-Tree', 'Edit Distance']
    };
}

// ===========================
// INITIALIZATION
// ===========================

window.addEventListener('load', () => {
    console.log('Smart Autocomplete & Spell-Checker Suite loaded');
    statDict.textContent = '1,250';
    
    // Show welcome message
    setTimeout(() => {
        resultsContainer.innerHTML = `
            <div class="placeholder">
                👋 Welcome! Start typing to see autocomplete suggestions
                <br><br>
                💡 Try: "pro", "aut", "com", "dat"
            </div>
        `;
    }, 500);
});
