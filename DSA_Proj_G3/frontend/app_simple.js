// Simple App.js for Smart Autocomplete
// Connects to Python backend server

const state = {
    currentMode: 'autocomplete',
    lastQuery: '',
    queryCount: 0
};

const API_BASE = 'http://localhost:8080/api';

// DOM Elements
const searchInput = document.getElementById('searchInput');
const searchBtn = document.getElementById('searchBtn');
const resultsContainer = document.getElementById('resultsContainer');
const modeBtns = document.querySelectorAll('.mode-btn');
const queryTimeSpan = document.getElementById('queryTime');
const statTime = document.getElementById('statTime');
const statCount = document.getElementById('statCount');

// Mode Switching
modeBtns.forEach(btn => {
    btn.addEventListener('click', () => {
        modeBtns.forEach(b => b.classList.remove('active'));
        btn.classList.add('active');
        state.currentMode = btn.dataset.mode;
        searchInput.placeholder = state.currentMode === 'autocomplete' 
            ? 'Type a sentence or word to search...' 
            : 'Enter sentence or word to check...';
        clearResults();
    });
});

// Real-time autocomplete
searchInput.addEventListener('input', (e) => {
    if (state.currentMode === 'autocomplete' && e.target.value.length > 0) {
        handleSearch(e.target.value);
    } else if (e.target.value.length === 0) {
        clearResults();
    }
});

// Search button
searchBtn.addEventListener('click', () => {
    const query = searchInput.value.trim();
    if (query) handleSearch(query);
});

// Enter key
searchInput.addEventListener('keypress', (e) => {
    if (e.key === 'Enter') {
        const query = searchInput.value.trim();
        if (query) handleSearch(query);
    }
});

// Main Search Handler
async function handleSearch(query) {
    state.lastQuery = query;
    state.queryCount++;
    
    try {
        const startTime = performance.now();
        let data;
        
        if (state.currentMode === 'autocomplete') {
            const response = await fetch(`${API_BASE}/autocomplete`, {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ query: query })
            });
            data = await response.json();
            displayAutocompleteResults(data);
        } else {
            const response = await fetch(`${API_BASE}/spellcheck`, {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ word: query })
            });
            data = await response.json();
            displaySpellCheckResults(data);
        }
        
        const endTime = performance.now();
        const timeTaken = (endTime - startTime).toFixed(2);
        
        // Update stats
        if (queryTimeSpan) queryTimeSpan.textContent = `Time: ${timeTaken} ms`;
        if (statTime) statTime.textContent = `${timeTaken} ms`;
        if (statCount) statCount.textContent = state.queryCount;
        
    } catch (error) {
        console.error('Search error:', error);
        showError('Server not available. Make sure webserver.py is running.');
    }
}

// Display Autocomplete Results
function displayAutocompleteResults(data) {
    resultsContainer.innerHTML = '';
    
    if (!data.suggestions || data.suggestions.length === 0) {
        resultsContainer.innerHTML = '<div class="placeholder">No suggestions found</div>';
        return;
    }
    
    data.suggestions.forEach((completion, index) => {
        const item = document.createElement('div');
        item.className = 'result-item';
        item.style.animationDelay = `${index * 0.05}s`;
        
        // Extract just the completed word for dictionary lookup
        const completedWords = completion.trim().split(' ');
        const lastWord = completedWords[completedWords.length - 1];
        
        item.innerHTML = `
            <span class="result-word">${completion}</span>
            <div class="result-meta">
                <span class="result-badge">#${index + 1}</span>
            </div>
        `;
        
        item.addEventListener('click', () => {
            searchInput.value = completion;
            showWordInDictionary(lastWord);
        });
        
        resultsContainer.appendChild(item);
    });
    
    // Update trie visualization with context
    const queryWords = state.lastQuery.split(' ');
    const isSentence = queryWords.length > 1;
    const lastQueryWord = queryWords[queryWords.length - 1];
    
    if (isSentence) {
        visualizeTrie(lastQueryWord, state.lastQuery, true);
    } else {
        visualizeTrie(lastQueryWord, state.lastQuery, false);
    }
}

// Show word/sentence details in dictionary panel
function showWordInDictionary(wordOrSentence) {
    const dictPanel = document.getElementById('dictionaryPanel');
    if (!dictPanel) return;
    
    // Check if it's a sentence (contains spaces) or single word
    const words = wordOrSentence.trim().split(/\s+/);
    const isSentence = words.length > 1;
    
    if (isSentence) {
        // Analyze the sentence
        const sentenceAnalysis = analyzeSentence(words);
        
        dictPanel.innerHTML = `
            <h2>📖 Sentence Analysis</h2>
            <div class="dictionary-content">
                <h3 class="dict-word">"${wordOrSentence}"</h3>
                <div class="dict-details">
                    <p><strong>Type:</strong> ${sentenceAnalysis.type}</p>
                    <p><strong>Word Count:</strong> ${words.length} words</p>
                    <p><strong>Character Count:</strong> ${wordOrSentence.length} characters</p>
                    <p><strong>Composition:</strong></p>
                    <ul style="margin: 5px 0 5px 20px; font-size: 0.9em;">
                        <li>Programming Terms: ${sentenceAnalysis.programmingWords} (${sentenceAnalysis.programmingPercentage}%)</li>
                        <li>General English: ${sentenceAnalysis.generalWords} (${sentenceAnalysis.generalPercentage}%)</li>
                    </ul>
                </div>
                <div class="dict-usage">
                    <p><strong>Words Breakdown:</strong></p>
                    <div style="max-height: 200px; overflow-y: auto;">
                        ${sentenceAnalysis.breakdown}
                    </div>
                </div>
            </div>
        `;
    } else {
        // Single word - show detailed categorization
        const wordInfo = categorizeWord(wordOrSentence);
        
        dictPanel.innerHTML = `
            <h2>📖 Dictionary</h2>
            <div class="dictionary-content">
                <h3 class="dict-word">${wordOrSentence}</h3>
                <div class="dict-details">
                    <p><strong>Type:</strong> ${wordInfo.type}</p>
                    <p><strong>Category:</strong> ${wordInfo.category}</p>
                    <p><strong>Subcategory:</strong> ${wordInfo.subcategory}</p>
                    <p><strong>Field:</strong> ${wordInfo.field}</p>
                    <p><strong>Level:</strong> ${wordInfo.level}</p>
                    <p><strong>Length:</strong> ${wordOrSentence.length} characters</p>
                    <p><strong>In Dictionary:</strong> ✓ Present</p>
                </div>
                <div class="dict-usage">
                    <p><strong>Usage Context:</strong></p>
                    <p style="font-size: 0.9em; color: #666; margin: 5px 0;">${wordInfo.usage}</p>
                    <p><strong>Example:</strong></p>
                    <code>${wordInfo.example}</code>
                </div>
            </div>
        `;
    }
}

// Analyze sentence composition
function analyzeSentence(words) {
    let programmingCount = 0;
    let generalCount = 0;
    let breakdown = '<div style="display: grid; gap: 5px;">';
    
    words.forEach((word, index) => {
        const cleanWord = word.replace(/[^a-zA-Z]/g, ''); // Remove punctuation
        if (!cleanWord) return;
        
        const wordInfo = categorizeWord(cleanWord);
        const isProgramming = wordInfo.field === 'Programming';
        
        if (isProgramming) {
            programmingCount++;
        } else {
            generalCount++;
        }
        
        const color = isProgramming ? '#6366f1' : '#8b5cf6';
        const icon = isProgramming ? '💻' : '📝';
        
        breakdown += `
            <div style="background: rgba(99, 102, 241, 0.1); padding: 5px 10px; border-radius: 5px; font-size: 0.85em;">
                ${icon} <strong>${cleanWord}</strong> → ${wordInfo.type}
                <span style="color: ${color}; font-size: 0.9em;"> (${wordInfo.field})</span>
            </div>
        `;
    });
    
    breakdown += '</div>';
    
    const total = programmingCount + generalCount;
    const programmingPercentage = total > 0 ? Math.round((programmingCount / total) * 100) : 0;
    const generalPercentage = total > 0 ? Math.round((generalCount / total) * 100) : 0;
    
    // Determine sentence type
    let type = 'Mixed Sentence';
    if (programmingPercentage > 70) {
        type = 'Technical/Programming Sentence';
    } else if (generalPercentage > 70) {
        type = 'General English Sentence';
    }
    
    return {
        type,
        programmingWords: programmingCount,
        generalWords: generalCount,
        programmingPercentage,
        generalPercentage,
        breakdown
    };
}

// Deep categorization of words with granular classification
function categorizeWord(word) {
    const lowerWord = word.toLowerCase();
    
    // ===== PROGRAMMING & COMPUTER SCIENCE =====
    
    // Data Structures
    const dataStructures = [
        'array', 'list', 'stack', 'queue', 'heap', 'tree', 'trie', 'graph', 
        'hash', 'hashtable', 'map', 'set', 'vector', 'deque', 'priority',
        'linked', 'node', 'edge', 'vertex', 'bst', 'avl', 'redblack', 'btree'
    ];
    
    // Algorithms & Complexity
    const algorithms = [
        'algorithm', 'sort', 'search', 'binary', 'linear', 'quicksort', 'mergesort',
        'bubblesort', 'heapsort', 'radix', 'counting', 'bucket', 'insertion',
        'selection', 'dijkstra', 'bellman', 'ford', 'floyd', 'warshall', 'kruskal',
        'prim', 'topological', 'dfs', 'bfs', 'backtrack', 'greedy', 'dynamic',
        'recursion', 'iteration', 'complexity', 'bigoh', 'optimization'
    ];
    
    // Programming Languages
    const languages = [
        'python', 'java', 'javascript', 'typescript', 'cpp', 'csharp', 'ruby',
        'php', 'swift', 'kotlin', 'rust', 'go', 'scala', 'perl', 'lua', 'r',
        'matlab', 'fortran', 'cobol', 'assembly', 'lisp', 'haskell', 'ocaml'
    ];
    
    // Programming Concepts
    const programmingConcepts = [
        'function', 'method', 'class', 'object', 'variable', 'constant', 'pointer',
        'reference', 'parameter', 'argument', 'return', 'void', 'static', 'final',
        'abstract', 'interface', 'inheritance', 'polymorphism', 'encapsulation',
        'constructor', 'destructor', 'overload', 'override', 'template', 'generic',
        'lambda', 'closure', 'callback', 'async', 'await', 'promise', 'thread',
        'process', 'mutex', 'semaphore', 'deadlock', 'race', 'condition', 'code',
        'coding', 'program', 'programming', 'software', 'hardware', 'compile',
        'compiler', 'interpreter', 'execute', 'runtime', 'debug', 'debugger',
        'syntax', 'semantic', 'lexical', 'parse', 'parser', 'token', 'keyword',
        'operator', 'operand', 'expression', 'statement', 'declaration', 'definition',
        'scope', 'namespace', 'module', 'package', 'library', 'framework', 'api',
        'sdk', 'ide', 'editor', 'terminal', 'console', 'shell', 'script', 'macro',
        'recursion', 'iteration', 'loop', 'conditional', 'branch', 'exception',
        'error', 'warning', 'null', 'undefined', 'boolean', 'integer', 'float',
        'double', 'string', 'char', 'byte', 'bit', 'binary', 'hexadecimal', 'octal'
    ];
    
    // Web Development
    const webDev = [
        'html', 'css', 'dom', 'ajax', 'rest', 'api', 'http', 'https', 'ssl',
        'json', 'xml', 'react', 'angular', 'vue', 'node', 'express', 'django',
        'flask', 'spring', 'servlet', 'jsp', 'asp', 'php', 'laravel', 'rails',
        'frontend', 'backend', 'fullstack', 'responsive', 'bootstrap', 'jquery'
    ];
    
    // Database & Storage
    const database = [
        'database', 'sql', 'nosql', 'query', 'select', 'insert', 'update', 'delete',
        'join', 'index', 'primary', 'foreign', 'key', 'table', 'schema', 'view',
        'transaction', 'commit', 'rollback', 'mysql', 'postgresql', 'mongodb',
        'redis', 'cassandra', 'oracle', 'sqlite', 'firebase', 'crud'
    ];
    
    // System & Architecture
    const systemArch = [
        'server', 'client', 'cloud', 'docker', 'kubernetes', 'container', 'virtual',
        'microservice', 'monolith', 'scalability', 'load', 'balancer', 'cache',
        'cdn', 'proxy', 'gateway', 'firewall', 'network', 'protocol', 'tcp', 'udp',
        'ip', 'dns', 'subnet', 'router', 'switch', 'bandwidth', 'latency'
    ];
    
    // Software Engineering
    const softwareEng = [
        'agile', 'scrum', 'kanban', 'devops', 'cicd', 'git', 'github', 'gitlab',
        'version', 'control', 'branch', 'merge', 'commit', 'pull', 'push', 'clone',
        'fork', 'repository', 'testing', 'unit', 'integration', 'deployment',
        'debug', 'breakpoint', 'compile', 'build', 'runtime', 'syntax', 'semantic'
    ];
    
    // AI & Machine Learning
    const aiMl = [
        'machine', 'learning', 'deep', 'neural', 'network', 'tensorflow', 'pytorch',
        'scikit', 'pandas', 'numpy', 'classification', 'regression', 'clustering',
        'supervised', 'unsupervised', 'reinforcement', 'gradient', 'descent',
        'backpropagation', 'activation', 'layer', 'model', 'train', 'test', 'validate'
    ];
    
    // Operating Systems
    const os = [
        'operating', 'system', 'kernel', 'shell', 'bash', 'powershell', 'terminal',
        'command', 'linux', 'unix', 'windows', 'macos', 'ubuntu', 'debian', 'fedora',
        'process', 'thread', 'scheduler', 'memory', 'virtual', 'paging', 'swap'
    ];
    
    // Security & Cryptography
    const security = [
        'encryption', 'decryption', 'cryptography', 'hash', 'sha', 'md5', 'rsa',
        'aes', 'ssl', 'tls', 'certificate', 'authentication', 'authorization',
        'token', 'jwt', 'oauth', 'firewall', 'malware', 'virus', 'phishing',
        'vulnerability', 'exploit', 'penetration', 'security', 'password', 'salt'
    ];
    
    // ===== ENGLISH LANGUAGE CATEGORIES =====
    
    // Articles & Determiners
    const articles = ['a', 'an', 'the', 'this', 'that', 'these', 'those', 'some', 'any', 'each', 'every', 'all', 'both', 'many', 'few', 'several'];
    
    // Pronouns
    const pronouns = ['i', 'you', 'he', 'she', 'it', 'we', 'they', 'me', 'him', 'her', 'us', 'them', 'my', 'your', 'his', 'her', 'its', 'our', 'their', 'mine', 'yours', 'hers', 'ours', 'theirs', 'myself', 'yourself', 'himself', 'herself', 'itself', 'ourselves', 'yourselves', 'themselves'];
    
    // Prepositions
    const prepositions = ['in', 'on', 'at', 'by', 'for', 'with', 'from', 'to', 'of', 'about', 'above', 'below', 'under', 'over', 'between', 'among', 'through', 'during', 'before', 'after', 'since', 'until', 'within', 'without', 'toward', 'against', 'across', 'along', 'around', 'behind', 'beside', 'beyond', 'into', 'onto', 'upon'];
    
    // Conjunctions
    const conjunctions = ['and', 'or', 'but', 'so', 'yet', 'nor', 'for', 'because', 'although', 'though', 'unless', 'if', 'when', 'where', 'while', 'since', 'after', 'before', 'until', 'whether'];
    
    // Common Verbs
    const commonVerbs = ['be', 'is', 'am', 'are', 'was', 'were', 'been', 'being', 'have', 'has', 'had', 'do', 'does', 'did', 'done', 'go', 'went', 'gone', 'come', 'came', 'get', 'got', 'make', 'made', 'take', 'took', 'see', 'saw', 'seen', 'know', 'knew', 'known', 'think', 'thought', 'say', 'said', 'tell', 'told', 'give', 'gave', 'given', 'find', 'found', 'use', 'used', 'want', 'work', 'call', 'try', 'ask', 'need', 'feel', 'become', 'leave', 'put', 'mean', 'keep', 'let', 'begin', 'seem', 'help', 'show', 'hear', 'play', 'run', 'move', 'live', 'believe', 'bring', 'happen', 'write', 'sit', 'stand', 'lose', 'pay', 'meet', 'include', 'continue', 'set', 'learn', 'change', 'lead', 'understand', 'watch', 'follow', 'stop', 'create', 'speak', 'read', 'spend', 'grow', 'open', 'walk', 'win', 'teach', 'offer', 'remember', 'consider', 'appear', 'buy', 'serve', 'die', 'send', 'build', 'stay', 'fall', 'cut', 'reach', 'kill', 'raise', 'pass', 'sell', 'decide', 'return', 'explain', 'hope', 'develop', 'carry', 'break', 'receive', 'agree', 'support', 'hit', 'produce', 'eat', 'cover', 'catch', 'draw', 'choose', 'require', 'rank', 'like', 'love', 'hate', 'enjoy', 'prefer', 'start', 'finish', 'complete', 'achieve', 'succeed', 'fail', 'attempt', 'manage', 'organize', 'plan', 'prepare', 'arrange', 'order', 'sort', 'count', 'measure', 'compare', 'contrast', 'differ', 'vary', 'increase', 'decrease', 'improve', 'worsen', 'enhance', 'reduce', 'expand', 'contract', 'extend', 'shorten', 'lengthen', 'widen', 'narrow', 'deepen', 'shallow', 'strengthen', 'weaken'];
    
    // Emotions & Feelings
    const emotions = ['happy', 'sad', 'angry', 'excited', 'afraid', 'surprised', 'anxious', 'nervous', 'worried', 'calm', 'peaceful', 'joyful', 'cheerful', 'delighted', 'pleased', 'content', 'satisfied', 'grateful', 'thankful', 'proud', 'confident', 'embarrassed', 'ashamed', 'guilty', 'jealous', 'envious', 'lonely', 'bored', 'tired', 'energetic', 'enthusiastic', 'passionate', 'loving', 'caring', 'compassionate', 'sympathetic', 'frustrated', 'annoyed', 'irritated', 'furious', 'depressed', 'miserable', 'hopeless', 'optimistic', 'hopeful', 'disappointed', 'discouraged'];
    
    // Descriptive Adjectives
    const adjectives = ['good', 'bad', 'great', 'small', 'large', 'big', 'little', 'long', 'short', 'high', 'low', 'old', 'new', 'young', 'beautiful', 'ugly', 'pretty', 'handsome', 'attractive', 'nice', 'wonderful', 'terrible', 'awful', 'amazing', 'incredible', 'fantastic', 'excellent', 'perfect', 'horrible', 'lovely', 'pleasant', 'unpleasant', 'comfortable', 'uncomfortable', 'easy', 'difficult', 'hard', 'simple', 'complex', 'complicated', 'clear', 'unclear', 'bright', 'dark', 'light', 'heavy', 'strong', 'weak', 'powerful', 'fast', 'slow', 'quick', 'rapid', 'sudden', 'gradual', 'smooth', 'rough', 'soft', 'hard', 'gentle', 'harsh', 'kind', 'cruel', 'mean', 'friendly', 'unfriendly', 'polite', 'rude', 'honest', 'dishonest', 'true', 'false', 'real', 'fake', 'genuine', 'artificial', 'natural', 'normal', 'abnormal', 'unusual', 'strange', 'weird', 'odd', 'regular', 'irregular', 'common', 'rare', 'ordinary', 'extraordinary', 'special', 'general', 'specific', 'particular', 'certain', 'sure', 'possible', 'impossible', 'probable', 'likely', 'unlikely', 'necessary', 'unnecessary', 'important', 'unimportant', 'essential', 'significant', 'insignificant', 'major', 'minor', 'main', 'primary', 'secondary', 'principal', 'chief', 'basic', 'fundamental', 'advanced'];
    
    // Time & Frequency
    const timeWords = ['now', 'then', 'today', 'tomorrow', 'yesterday', 'always', 'never', 'sometimes', 'often', 'rarely', 'seldom', 'frequently', 'occasionally', 'constantly', 'continually', 'immediately', 'soon', 'later', 'early', 'late', 'already', 'still', 'yet', 'once', 'twice', 'again', 'ever', 'forever', 'nowadays', 'recently', 'currently', 'previously', 'formerly', 'eventually', 'finally', 'initially', 'originally'];
    
    // Quantity & Degree
    const quantityWords = ['all', 'some', 'any', 'none', 'many', 'much', 'few', 'little', 'several', 'enough', 'plenty', 'lots', 'more', 'most', 'less', 'least', 'very', 'too', 'so', 'quite', 'rather', 'pretty', 'fairly', 'extremely', 'incredibly', 'absolutely', 'completely', 'totally', 'partially', 'slightly', 'somewhat', 'almost', 'nearly', 'hardly', 'scarcely', 'barely'];
    
    // Common Nouns - People & Relationships
    const peopleNouns = ['person', 'people', 'man', 'woman', 'boy', 'girl', 'child', 'children', 'baby', 'adult', 'teenager', 'parent', 'mother', 'father', 'mom', 'dad', 'son', 'daughter', 'brother', 'sister', 'family', 'friend', 'neighbor', 'stranger', 'student', 'teacher', 'professor', 'doctor', 'nurse', 'engineer', 'lawyer', 'artist', 'writer', 'musician', 'actor', 'athlete', 'worker', 'employee', 'employer', 'manager', 'boss', 'colleague', 'partner', 'team', 'group', 'crowd', 'audience', 'community', 'society', 'public', 'citizen', 'resident', 'visitor', 'guest', 'host'];
    
    // Common Nouns - Places
    const placeNouns = ['place', 'world', 'country', 'city', 'town', 'village', 'home', 'house', 'apartment', 'building', 'room', 'office', 'school', 'university', 'college', 'hospital', 'church', 'temple', 'mosque', 'library', 'museum', 'theater', 'cinema', 'restaurant', 'cafe', 'hotel', 'store', 'shop', 'market', 'mall', 'bank', 'park', 'garden', 'street', 'road', 'highway', 'bridge', 'airport', 'station', 'port', 'beach', 'mountain', 'forest', 'river', 'lake', 'ocean', 'sea'];
    
    // Common Nouns - Things & Objects
    const thingNouns = ['thing', 'object', 'item', 'stuff', 'material', 'substance', 'matter', 'piece', 'part', 'element', 'component', 'unit', 'book', 'paper', 'document', 'letter', 'note', 'card', 'picture', 'photo', 'image', 'video', 'film', 'movie', 'song', 'music', 'sound', 'noise', 'voice', 'word', 'sentence', 'paragraph', 'text', 'message', 'email', 'phone', 'computer', 'machine', 'device', 'tool', 'instrument', 'equipment', 'furniture', 'table', 'chair', 'desk', 'bed', 'car', 'vehicle', 'bike', 'bicycle', 'bus', 'train', 'plane', 'ship', 'boat', 'clothes', 'shirt', 'pants', 'dress', 'shoe', 'hat', 'bag', 'box', 'container', 'bottle', 'cup', 'glass', 'plate', 'bowl', 'food', 'meal', 'breakfast', 'lunch', 'dinner', 'drink', 'water', 'coffee', 'tea', 'money', 'dollar', 'price', 'cost'];
    
    // Abstract Nouns
    const abstractNouns = ['idea', 'thought', 'opinion', 'belief', 'knowledge', 'information', 'truth', 'fact', 'theory', 'concept', 'principle', 'rule', 'law', 'right', 'freedom', 'liberty', 'justice', 'peace', 'war', 'love', 'hate', 'friendship', 'relationship', 'marriage', 'life', 'death', 'birth', 'health', 'disease', 'illness', 'pain', 'pleasure', 'happiness', 'sadness', 'joy', 'sorrow', 'success', 'failure', 'victory', 'defeat', 'power', 'strength', 'weakness', 'ability', 'skill', 'talent', 'quality', 'quantity', 'beauty', 'ugliness', 'good', 'evil', 'right', 'wrong', 'time', 'moment', 'period', 'age', 'era', 'past', 'present', 'future', 'beginning', 'end', 'start', 'finish', 'change', 'development', 'growth', 'progress', 'improvement', 'decline', 'problem', 'issue', 'question', 'answer', 'solution', 'reason', 'cause', 'effect', 'result', 'consequence', 'purpose', 'goal', 'aim', 'objective', 'plan', 'method', 'way', 'manner', 'style', 'form', 'shape', 'size', 'amount', 'number', 'level', 'degree', 'rate', 'speed', 'distance', 'space', 'area', 'region', 'position', 'location', 'direction', 'movement', 'action', 'activity', 'behavior', 'conduct', 'attitude', 'feeling', 'emotion', 'sense', 'experience', 'memory', 'imagination', 'dream', 'hope', 'fear', 'worry', 'concern', 'interest', 'attention', 'focus', 'concentration', 'race', 'range', 'ranges', 'ranking', 'competition', 'contest', 'match', 'game', 'sport', 'event', 'occasion', 'celebration', 'ceremony', 'ritual', 'tradition', 'custom', 'practice', 'habit', 'routine', 'pattern', 'trend', 'tendency', 'inclination', 'preference', 'choice', 'selection', 'option', 'alternative', 'possibility', 'chance', 'opportunity', 'prospect'];
    
    // Academic & Formal Words
    const academicWords = ['analysis', 'synthesis', 'evaluation', 'assessment', 'research', 'study', 'investigation', 'examination', 'observation', 'experiment', 'hypothesis', 'theory', 'evidence', 'proof', 'argument', 'conclusion', 'discussion', 'explanation', 'description', 'definition', 'classification', 'categorization', 'comparison', 'contrast', 'similarity', 'difference', 'relationship', 'correlation', 'causation', 'implication', 'significance', 'importance', 'relevance', 'application', 'implementation', 'methodology', 'approach', 'technique', 'procedure', 'process', 'system', 'structure', 'organization', 'framework', 'model', 'paradigm', 'perspective', 'viewpoint', 'interpretation', 'understanding', 'comprehension', 'perception', 'recognition', 'identification', 'distinction', 'differentiation', 'specification', 'determination', 'establishment', 'foundation', 'basis', 'principle', 'criterion', 'standard', 'requirement', 'condition', 'constraint', 'limitation', 'restriction', 'regulation', 'policy', 'guideline', 'recommendation', 'suggestion', 'proposal', 'initiative', 'strategy', 'tactic', 'measure', 'action', 'intervention', 'contribution', 'impact', 'influence', 'effect', 'outcome', 'consequence', 'achievement', 'accomplishment', 'attainment', 'acquisition', 'development', 'advancement', 'improvement', 'enhancement', 'modification', 'alteration', 'transformation', 'transition', 'evolution', 'revolution', 'innovation', 'creation', 'generation', 'production', 'construction', 'formation', 'establishment', 'institution', 'organization', 'association', 'corporation', 'enterprise', 'industry', 'sector', 'field', 'domain', 'area', 'sphere', 'realm', 'dimension', 'aspect', 'feature', 'characteristic', 'attribute', 'property', 'quality', 'nature', 'essence', 'substance', 'content', 'context', 'environment', 'setting', 'situation', 'circumstance', 'condition', 'state', 'status', 'position', 'role', 'function', 'purpose', 'objective', 'goal', 'aim', 'target', 'intention', 'motivation', 'incentive', 'stimulus', 'response', 'reaction', 'interaction', 'communication', 'expression', 'representation', 'manifestation', 'demonstration', 'illustration', 'exemplification', 'clarification', 'elucidation', 'explanation', 'justification', 'rationalization', 'validation', 'verification', 'confirmation', 'authentication', 'authorization', 'approval', 'acceptance', 'recognition', 'acknowledgment', 'appreciation', 'consideration', 'deliberation', 'contemplation', 'reflection', 'meditation', 'speculation', 'assumption', 'presumption', 'supposition', 'conjecture', 'inference', 'deduction', 'induction', 'reasoning', 'logic', 'rationality', 'coherence', 'consistency', 'compatibility', 'congruence', 'correspondence', 'alignment', 'integration', 'unification', 'consolidation', 'coordination', 'cooperation', 'collaboration', 'partnership', 'alliance', 'coalition', 'union', 'federation', 'confederation', 'association', 'affiliation', 'connection', 'linkage', 'relation', 'correlation', 'interdependence', 'interaction', 'reciprocity', 'mutuality', 'symmetry', 'balance', 'equilibrium', 'stability', 'consistency', 'continuity', 'persistence', 'permanence', 'duration', 'longevity', 'sustainability', 'viability', 'feasibility', 'practicality', 'utility', 'functionality', 'effectiveness', 'efficiency', 'productivity', 'performance', 'capability', 'capacity', 'potential', 'possibility', 'probability', 'likelihood', 'certainty', 'uncertainty', 'ambiguity', 'clarity', 'precision', 'accuracy', 'correctness', 'validity', 'reliability', 'credibility', 'authenticity', 'integrity', 'honesty', 'transparency', 'accountability', 'responsibility', 'obligation', 'duty', 'commitment', 'dedication', 'devotion', 'loyalty', 'allegiance', 'fidelity', 'faithfulness'];
    
    // Science & Mathematics
    const scienceMath = ['mathematics', 'algebra', 'geometry', 'calculus', 'statistics', 'probability', 'equation', 'formula', 'theorem', 'proof', 'axiom', 'variable', 'constant', 'coefficient', 'exponent', 'logarithm', 'derivative', 'integral', 'limit', 'infinity', 'zero', 'negative', 'positive', 'fraction', 'decimal', 'percentage', 'ratio', 'proportion', 'mean', 'median', 'mode', 'deviation', 'variance', 'correlation', 'regression', 'hypothesis', 'distribution', 'normal', 'uniform', 'binomial', 'poisson', 'physics', 'chemistry', 'biology', 'astronomy', 'geology', 'ecology', 'anatomy', 'physiology', 'genetics', 'evolution', 'organism', 'cell', 'molecule', 'atom', 'electron', 'proton', 'neutron', 'particle', 'wave', 'energy', 'force', 'motion', 'velocity', 'acceleration', 'gravity', 'mass', 'weight', 'density', 'pressure', 'temperature', 'heat', 'light', 'sound', 'electricity', 'magnetism', 'radiation', 'chemical', 'element', 'compound', 'mixture', 'solution', 'reaction', 'oxidation', 'reduction', 'acid', 'base', 'salt', 'catalyst', 'enzyme', 'protein', 'carbohydrate', 'lipid', 'nucleic', 'dna', 'rna', 'chromosome', 'gene', 'mutation', 'natural', 'selection', 'adaptation', 'species', 'population', 'ecosystem', 'habitat', 'environment', 'climate', 'weather', 'atmosphere', 'ocean', 'continent', 'planet', 'star', 'galaxy', 'universe', 'solar', 'lunar', 'eclipse', 'orbit', 'rotation', 'revolution'];
    
    // Business & Economics
    const businessEcon = ['business', 'company', 'corporation', 'enterprise', 'firm', 'organization', 'industry', 'market', 'economy', 'trade', 'commerce', 'finance', 'banking', 'investment', 'stock', 'share', 'bond', 'asset', 'liability', 'equity', 'capital', 'revenue', 'income', 'profit', 'loss', 'expense', 'cost', 'budget', 'price', 'value', 'worth', 'demand', 'supply', 'competition', 'monopoly', 'oligopoly', 'consumer', 'producer', 'customer', 'client', 'vendor', 'supplier', 'distributor', 'retailer', 'wholesaler', 'manufacturer', 'product', 'service', 'brand', 'marketing', 'advertising', 'promotion', 'sales', 'purchase', 'transaction', 'contract', 'agreement', 'negotiation', 'deal', 'merger', 'acquisition', 'partnership', 'franchise', 'license', 'patent', 'copyright', 'trademark', 'intellectual', 'property', 'innovation', 'entrepreneurship', 'startup', 'venture', 'risk', 'return', 'portfolio', 'diversification', 'insurance', 'pension', 'retirement', 'savings', 'loan', 'credit', 'debt', 'mortgage', 'interest', 'inflation', 'deflation', 'recession', 'depression', 'growth', 'development', 'prosperity', 'wealth', 'poverty', 'inequality', 'distribution', 'allocation', 'resource', 'scarcity', 'abundance', 'efficiency', 'productivity', 'output', 'input', 'labor', 'wage', 'salary', 'compensation', 'benefit', 'incentive', 'bonus', 'commission', 'dividend', 'tax', 'subsidy', 'tariff', 'quota', 'sanction', 'embargo', 'export', 'import', 'balance', 'surplus', 'deficit', 'exchange', 'currency', 'dollar', 'euro', 'pound', 'yen', 'bitcoin', 'cryptocurrency', 'blockchain', 'ledger', 'accounting', 'audit', 'compliance', 'regulation', 'governance', 'ethics', 'responsibility', 'sustainability'];
    
    // ===== CATEGORIZATION LOGIC =====
    
    // Check Data Structures (exact match)
    if (dataStructures.includes(lowerWord)) {
        return {
            type: 'Data Structure',
            category: 'Computer Science → Data Structures & Algorithms',
            subcategory: 'Abstract Data Type / Collection',
            example: `// Example: ${word} ds = new ${word.charAt(0).toUpperCase() + word.slice(1)}();`,
            field: 'Programming',
            level: 'Technical',
            usage: 'Used in algorithm design and software development'
        };
    }
    
    // Check Algorithms (exact match)
    if (algorithms.includes(lowerWord)) {
        return {
            type: 'Algorithm / Technique',
            category: 'Computer Science → Algorithms & Complexity',
            subcategory: 'Computational Method',
            example: `// Example: Implementing ${word} with O(n log n) complexity`,
            field: 'Programming',
            level: 'Technical',
            usage: 'Used in problem solving and optimization'
        };
    }
    
    // Check Programming Languages (exact match)
    if (languages.includes(lowerWord)) {
        return {
            type: 'Programming Language',
            category: 'Computer Science → Programming Languages',
            subcategory: 'Software Development Tool',
            example: `// Example: Writing code in ${word.charAt(0).toUpperCase() + word.slice(1)}`,
            field: 'Programming',
            level: 'Technical',
            usage: 'Used for software development and scripting'
        };
    }
    
    // Check Programming Concepts (exact match)
    if (programmingConcepts.includes(lowerWord)) {
        return {
            type: 'Programming Concept',
            category: 'Computer Science → Object-Oriented Programming',
            subcategory: 'Code Structure / Paradigm',
            example: `// Example: Defining a ${word} in your application`,
            field: 'Programming',
            level: 'Technical',
            usage: 'Core concept in software engineering'
        };
    }
    
    // Check Web Development (exact match)
    if (webDev.includes(lowerWord)) {
        return {
            type: 'Web Technology',
            category: 'Computer Science → Web Development',
            subcategory: 'Frontend / Backend Framework',
            example: `<!-- Example: Using ${word} in web development -->`,
            field: 'Programming',
            level: 'Technical',
            usage: 'Used in building web applications'
        };
    }
    
    // Check Database (exact match)
    if (database.includes(lowerWord)) {
        return {
            type: 'Database Concept',
            category: 'Computer Science → Database Systems',
            subcategory: 'Data Management / Query',
            example: `-- Example: ${word.toUpperCase()} operation on database`,
            field: 'Programming',
            level: 'Technical',
            usage: 'Used in data storage and retrieval'
        };
    }
    
    // Check System Architecture (exact match)
    if (systemArch.includes(lowerWord)) {
        return {
            type: 'System Component',
            category: 'Computer Science → System Architecture',
            subcategory: 'Infrastructure / Network',
            example: `// Example: Configuring ${word} for scalability`,
            field: 'Programming',
            level: 'Technical',
            usage: 'Used in system design and deployment'
        };
    }
    
    // Check Software Engineering (exact match)
    if (softwareEng.includes(lowerWord)) {
        return {
            type: 'Software Engineering Practice',
            category: 'Computer Science → Software Engineering',
            subcategory: 'Development Methodology',
            example: `// Example: Following ${word} best practices`,
            field: 'Programming',
            level: 'Technical',
            usage: 'Used in professional software development'
        };
    }
    
    // Check AI & Machine Learning (exact match)
    if (aiMl.includes(lowerWord)) {
        return {
            type: 'AI/ML Concept',
            category: 'Computer Science → Artificial Intelligence',
            subcategory: 'Machine Learning / Deep Learning',
            example: `# Example: Training a ${word} model`,
            field: 'Programming',
            level: 'Technical',
            usage: 'Used in AI and data science applications'
        };
    }
    
    // Check Operating Systems (exact match)
    if (os.includes(lowerWord)) {
        return {
            type: 'Operating System',
            category: 'Computer Science → Operating Systems',
            subcategory: 'System Software / Kernel',
            example: `$ Example: Running ${word} commands`,
            field: 'Programming',
            level: 'Technical',
            usage: 'Used in system administration and low-level programming'
        };
    }
    
    // Check Security (exact match)
    if (security.includes(lowerWord)) {
        return {
            type: 'Security Concept',
            category: 'Computer Science → Cybersecurity',
            subcategory: 'Encryption / Authentication',
            example: `// Example: Implementing ${word} for data protection`,
            field: 'Programming',
            level: 'Technical',
            usage: 'Used in securing applications and data'
        };
    }
    
    // ===== ENGLISH LANGUAGE CATEGORIZATION =====
    
    // Check Articles
    if (articles.includes(lowerWord)) {
        return {
            type: 'Article / Determiner',
            category: 'English Grammar → Determiners',
            subcategory: 'Function Word',
            example: `"${word.charAt(0).toUpperCase() + word.slice(1)} book is on the table."`,
            field: 'Language',
            level: 'Basic',
            usage: 'Used to specify or identify nouns'
        };
    }
    
    // Check Pronouns
    if (pronouns.includes(lowerWord)) {
        return {
            type: 'Pronoun',
            category: 'English Grammar → Pronouns',
            subcategory: 'Personal / Possessive / Reflexive',
            example: `"${word.charAt(0).toUpperCase() + word.slice(1)} went to the store."`,
            field: 'Language',
            level: 'Basic',
            usage: 'Used to replace nouns in sentences'
        };
    }
    
    // Check Prepositions
    if (prepositions.includes(lowerWord)) {
        return {
            type: 'Preposition',
            category: 'English Grammar → Prepositions',
            subcategory: 'Function Word - Position/Direction',
            example: `"The book is ${lowerWord} the table."`,
            field: 'Language',
            level: 'Basic',
            usage: 'Used to show relationships between words'
        };
    }
    
    // Check Conjunctions
    if (conjunctions.includes(lowerWord)) {
        return {
            type: 'Conjunction',
            category: 'English Grammar → Conjunctions',
            subcategory: 'Coordinating / Subordinating',
            example: `"I like tea ${lowerWord} coffee."`,
            field: 'Language',
            level: 'Basic',
            usage: 'Used to connect words, phrases, or clauses'
        };
    }
    
    // Check Common Verbs
    if (commonVerbs.includes(lowerWord)) {
        return {
            type: 'Common Verb',
            category: 'English Grammar → Verbs',
            subcategory: 'Action / State / Auxiliary',
            example: `"They ${lowerWord} to school every day."`,
            field: 'Language',
            level: 'Basic',
            usage: 'Expresses actions, states, or occurrences'
        };
    }
    
    // Check Emotions
    if (emotions.includes(lowerWord)) {
        return {
            type: 'Emotion / Feeling',
            category: 'English Vocabulary → Emotions & Psychology',
            subcategory: 'Affective State',
            example: `"She feels ${lowerWord} about the news."`,
            field: 'Language',
            level: 'Intermediate',
            usage: 'Describes emotional states and feelings'
        };
    }
    
    // Check Adjectives
    if (adjectives.includes(lowerWord)) {
        return {
            type: 'Descriptive Adjective',
            category: 'English Grammar → Adjectives',
            subcategory: 'Quality / Characteristic',
            example: `"That is a ${lowerWord} idea."`,
            field: 'Language',
            level: 'Intermediate',
            usage: 'Describes or modifies nouns'
        };
    }
    
    // Check Time Words
    if (timeWords.includes(lowerWord)) {
        return {
            type: 'Time Expression',
            category: 'English Vocabulary → Time & Frequency',
            subcategory: 'Temporal Adverb',
            example: `"I will do it ${lowerWord}."`,
            field: 'Language',
            level: 'Basic',
            usage: 'Indicates when or how often something occurs'
        };
    }
    
    // Check Quantity Words
    if (quantityWords.includes(lowerWord)) {
        return {
            type: 'Quantity / Degree Word',
            category: 'English Vocabulary → Quantity & Measurement',
            subcategory: 'Quantifier / Intensifier',
            example: `"There are ${lowerWord} people here."`,
            field: 'Language',
            level: 'Basic',
            usage: 'Expresses amount, degree, or intensity'
        };
    }
    
    // Check People Nouns
    if (peopleNouns.includes(lowerWord)) {
        return {
            type: 'Common Noun - Person',
            category: 'English Vocabulary → People & Relationships',
            subcategory: 'Concrete Noun - Human',
            example: `"The ${lowerWord} arrived early."`,
            field: 'Language',
            level: 'Basic',
            usage: 'Refers to people and human relationships'
        };
    }
    
    // Check Place Nouns
    if (placeNouns.includes(lowerWord)) {
        return {
            type: 'Common Noun - Place',
            category: 'English Vocabulary → Places & Locations',
            subcategory: 'Concrete Noun - Location',
            example: `"We visited the ${lowerWord} yesterday."`,
            field: 'Language',
            level: 'Basic',
            usage: 'Refers to physical locations and places'
        };
    }
    
    // Check Thing Nouns
    if (thingNouns.includes(lowerWord)) {
        return {
            type: 'Common Noun - Object',
            category: 'English Vocabulary → Objects & Things',
            subcategory: 'Concrete Noun - Physical Item',
            example: `"I bought a new ${lowerWord}."`,
            field: 'Language',
            level: 'Basic',
            usage: 'Refers to physical objects and items'
        };
    }
    
    // Check Abstract Nouns
    if (abstractNouns.includes(lowerWord)) {
        return {
            type: 'Abstract Noun',
            category: 'English Vocabulary → Abstract Concepts',
            subcategory: 'Intangible Concept / Idea',
            example: `"${word.charAt(0).toUpperCase() + word.slice(1)} is important in life."`,
            field: 'Language',
            level: 'Intermediate',
            usage: 'Refers to ideas, qualities, or states'
        };
    }
    
    // Check Academic Words
    if (academicWords.includes(lowerWord)) {
        return {
            type: 'Academic Vocabulary',
            category: 'English Vocabulary → Academic & Formal',
            subcategory: 'Scholarly / Professional',
            example: `"The research provides a comprehensive ${lowerWord} of the topic."`,
            field: 'Language',
            level: 'Advanced',
            usage: 'Used in academic writing and formal discourse'
        };
    }
    
    // Check Science & Math
    if (scienceMath.includes(lowerWord)) {
        return {
            type: 'Scientific Term',
            category: 'Science & Mathematics → STEM Vocabulary',
            subcategory: 'Technical / Scientific',
            example: `"The ${lowerWord} is fundamental to understanding this concept."`,
            field: 'Science',
            level: 'Technical',
            usage: 'Used in scientific and mathematical contexts'
        };
    }
    
    // Check Business & Economics
    if (businessEcon.includes(lowerWord)) {
        return {
            type: 'Business Term',
            category: 'Business & Economics → Commercial Vocabulary',
            subcategory: 'Financial / Commercial',
            example: `"The ${lowerWord} showed strong performance this quarter."`,
            field: 'Business',
            level: 'Professional',
            usage: 'Used in business and economic contexts'
        };
    }
    
    // Pattern-based categorization for words not in lists
    
    // Check for programming suffixes
    if (lowerWord.endsWith('able') || lowerWord.endsWith('ible')) {
        return {
            type: 'Adjective - Ability',
            category: 'English Grammar → Adjectives',
            subcategory: 'Capability / Possibility',
            example: `"This solution is ${lowerWord}."`,
            field: 'Language',
            level: 'Intermediate',
            usage: 'Indicates capability or possibility'
        };
    }
    
    if (lowerWord.endsWith('tion') || lowerWord.endsWith('sion')) {
        return {
            type: 'Abstract Noun - Process',
            category: 'English Vocabulary → Abstract Concepts',
            subcategory: 'Action / Process / State',
            example: `"The ${lowerWord} was completed successfully."`,
            field: 'Language',
            level: 'Advanced',
            usage: 'Describes actions, processes, or states'
        };
    }
    
    if (lowerWord.endsWith('ment')) {
        return {
            type: 'Abstract Noun - Result',
            category: 'English Vocabulary → Abstract Concepts',
            subcategory: 'Result / Condition',
            example: `"The ${lowerWord} brought positive changes."`,
            field: 'Language',
            level: 'Advanced',
            usage: 'Indicates result or condition of an action'
        };
    }
    
    if (lowerWord.endsWith('ness')) {
        return {
            type: 'Abstract Noun - Quality',
            category: 'English Vocabulary → Abstract Concepts',
            subcategory: 'Quality / State of Being',
            example: `"The ${lowerWord} of the situation is evident."`,
            field: 'Language',
            level: 'Intermediate',
            usage: 'Expresses quality or state of being'
        };
    }
    
    if (lowerWord.endsWith('ity') || lowerWord.endsWith('ty')) {
        return {
            type: 'Abstract Noun - Property',
            category: 'English Vocabulary → Abstract Concepts',
            subcategory: 'Property / Characteristic',
            example: `"The ${lowerWord} of this approach is clear."`,
            field: 'Language',
            level: 'Advanced',
            usage: 'Denotes property or characteristic'
        };
    }
    
    if (lowerWord.endsWith('ism')) {
        return {
            type: 'Abstract Noun - System',
            category: 'English Vocabulary → Ideology & Philosophy',
            subcategory: 'Belief System / Doctrine',
            example: `"The principles of ${lowerWord} are widely debated."`,
            field: 'Language',
            level: 'Advanced',
            usage: 'Refers to belief systems or doctrines'
        };
    }
    
    if (lowerWord.endsWith('ly')) {
        return {
            type: 'Adverb',
            category: 'English Grammar → Adverbs',
            subcategory: 'Manner / Degree / Time',
            example: `"She completed the task ${lowerWord}."`,
            field: 'Language',
            level: 'Intermediate',
            usage: 'Modifies verbs, adjectives, or other adverbs'
        };
    }
    
    if (lowerWord.endsWith('er') || lowerWord.endsWith('or')) {
        return {
            type: 'Agent Noun',
            category: 'English Vocabulary → People & Occupations',
            subcategory: 'Person Who Does / Device That Does',
            example: `"The ${lowerWord} completed the work efficiently."`,
            field: 'Language',
            level: 'Basic',
            usage: 'Refers to person or thing that performs action'
        };
    }
    
    if (lowerWord.endsWith('ing')) {
        return {
            type: 'Gerund / Present Participle',
            category: 'English Grammar → Verb Forms',
            subcategory: 'Verbal Noun / Continuous Form',
            example: `"${word.charAt(0).toUpperCase() + word.slice(1)} is an important skill."`,
            field: 'Language',
            level: 'Intermediate',
            usage: 'Functions as noun or shows continuous action'
        };
    }
    
    if (lowerWord.endsWith('ed')) {
        return {
            type: 'Past Participle / Adjective',
            category: 'English Grammar → Verb Forms',
            subcategory: 'Past Form / Descriptive',
            example: `"The ${lowerWord} version was better."`,
            field: 'Language',
            level: 'Intermediate',
            usage: 'Shows completed action or describes state'
        };
    }
    
    // Default categorization by word length and complexity
    if (lowerWord.length <= 3) {
        return {
            type: 'Short Word',
            category: 'English Vocabulary → Basic Vocabulary',
            subcategory: 'Common / Fundamental',
            example: `"The word '${lowerWord}' is frequently used."`,
            field: 'Language',
            level: 'Basic',
            usage: 'Part of fundamental English vocabulary'
        };
    } else if (lowerWord.length >= 12) {
        return {
            type: 'Complex Word',
            category: 'English Vocabulary → Advanced Vocabulary',
            subcategory: 'Sophisticated / Specialized',
            example: `"The term '${lowerWord}' requires careful consideration."`,
            field: 'Language',
            level: 'Advanced',
            usage: 'Used in formal or specialized contexts'
        };
    }
    
    // Final default
    return {
        type: 'General Vocabulary',
        category: 'English Vocabulary → General Words',
        subcategory: 'Common Usage',
        example: `"${word.charAt(0).toUpperCase() + word.slice(1)} is a useful word to know."`,
        field: 'Language',
        level: 'Intermediate',
        usage: 'Part of everyday English vocabulary'
    };
}

// Visualize Trie structure
function visualizeTrie(prefix, fullQuery = '', isSentence = false) {
    const vizContainer = document.getElementById('vizContainer');
    if (!vizContainer) return;
    
    if (!prefix || prefix.length === 0) {
        vizContainer.innerHTML = `
            <div class="viz-placeholder">
                <p>🌳 Trie Traversal Animation</p>
                <p>Type to see the tree structure! ✨</p>
            </div>
        `;
        return;
    }
    
    // Create visual representation of trie traversal
    let trieHTML = '<div class="trie-visualization">';
    
    if (isSentence) {
        const words = fullQuery.split(' ');
        trieHTML += '<div class="trie-title">Sentence: "' + fullQuery + '"</div>';
        trieHTML += '<div class="trie-subtitle" style="font-size: 0.85em; color: #888; margin-bottom: 10px;">Autocompleting last word: "' + prefix + '"</div>';
    } else {
        trieHTML += '<div class="trie-title">Trie Path for: "' + prefix + '"</div>';
    }
    
    trieHTML += '<div class="trie-path">';
    
    // Show each character as a node
    trieHTML += '<div class="trie-node root">ROOT</div>';
    for (let i = 0; i < prefix.length; i++) {
        trieHTML += '<div class="trie-arrow">→</div>';
        trieHTML += `<div class="trie-node char">${prefix[i].toUpperCase()}</div>`;
    }
    
    trieHTML += '</div>';
    
    if (isSentence) {
        trieHTML += '<div class="trie-info">Word being completed: "' + prefix + '" (' + prefix.length + ' chars)</div>';
    } else {
        trieHTML += '<div class="trie-info">Path Length: ' + prefix.length + ' nodes</div>';
    }
    
    trieHTML += '<div class="trie-complexity">Time Complexity: O(' + prefix.length + ')</div>';
    trieHTML += '</div>';
    
    vizContainer.innerHTML = trieHTML;
}

// Display Spell Check Results
function displaySpellCheckResults(data) {
    resultsContainer.innerHTML = '';
    
    const statusDiv = document.createElement('div');
    statusDiv.className = 'result-item status-' + (data.correct ? 'correct' : 'incorrect');
    
    // For sentences, show which words are incorrect
    if (data.incorrect_words && data.incorrect_words.length > 0) {
        statusDiv.innerHTML = `
            <div class="spell-status">
                <span class="spell-word">${data.word}</span>
                <span class="spell-indicator">✗ ${data.incorrect_words.length} word(s) misspelled</span>
            </div>
            <div style="margin-top: 10px; font-size: 0.9em; color: #ff6b6b;">
                <strong>Incorrect:</strong> ${data.incorrect_words.join(', ')}
            </div>
        `;
    } else {
        statusDiv.innerHTML = `
            <div class="spell-status">
                <span class="spell-word">${data.word}</span>
                <span class="spell-indicator">${data.correct ? '✓ All words correct' : '✗ Incorrect'}</span>
            </div>
        `;
    }
    resultsContainer.appendChild(statusDiv);
    
    // Show suggestions for each incorrect word
    if (!data.correct && data.suggestions && Object.keys(data.suggestions).length > 0) {
        for (const [word, suggs] of Object.entries(data.suggestions)) {
            if (suggs.length > 0) {
                const wordHeader = document.createElement('div');
                wordHeader.className = 'suggestions-header';
                wordHeader.textContent = `Suggestions for "${word}":`;
                wordHeader.style.marginTop = '15px';
                resultsContainer.appendChild(wordHeader);
                
                suggs.forEach((suggestion, index) => {
                    const item = document.createElement('div');
                    item.className = 'result-item suggestion';
                    item.style.animationDelay = `${(index + 1) * 0.05}s`;
                    
                    item.innerHTML = `
                        <span class="result-word">${suggestion}</span>
                        <div class="result-meta">
                            <span class="result-badge">Replace ${word}</span>
                        </div>
                    `;
                    
                    item.addEventListener('click', () => {
                        // Replace the incorrect word in the sentence
                        const newText = data.word.replace(new RegExp('\\b' + word + '\\b', 'gi'), suggestion);
                        searchInput.value = newText;
                        handleSearch(newText);
                    });
                    
                    resultsContainer.appendChild(item);
                });
            }
        }
    } else if (!data.correct) {
        const noSuggestions = document.createElement('div');
        noSuggestions.className = 'placeholder';
        noSuggestions.textContent = 'No suggestions available';
        resultsContainer.appendChild(noSuggestions);
    }
}

// Clear Results
function clearResults() {
    resultsContainer.innerHTML = '<div class="placeholder">Start typing to see suggestions...</div>';
}

// Show Error
function showError(message) {
    resultsContainer.innerHTML = `<div class="error-message">⚠️ ${message}</div>`;
}

// Initialize
clearResults();
console.log('Smart Autocomplete initialized!');
