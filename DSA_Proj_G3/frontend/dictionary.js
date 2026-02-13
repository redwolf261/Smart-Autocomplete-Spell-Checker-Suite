// ===========================
// DICTIONARY.JS - Dictionary API Integration
// ===========================

const DICTIONARY_API_BASE = 'https://api.dictionaryapi.dev/api/v2/entries/en/';

class DictionaryAPI {
    constructor() {
        this.cache = new Map();
        this.currentAudio = null;
    }

    /**
     * Fetch word definition from Dictionary API
     * @param {string} word - Word to look up
     * @returns {Promise<Object>} Dictionary data
     */
    async fetchDefinition(word) {
        if (!word || word.trim().length === 0) {
            throw new Error('Invalid word');
        }

        const normalizedWord = word.toLowerCase().trim();

        // Check cache first
        if (this.cache.has(normalizedWord)) {
            return this.cache.get(normalizedWord);
        }

        try {
            const response = await fetch(`${DICTIONARY_API_BASE}${encodeURIComponent(normalizedWord)}`);
            
            if (!response.ok) {
                throw new Error(`API returned ${response.status}`);
            }

            const data = await response.json();
            const parsed = this.parseResponse(data);
            
            // Cache the result
            this.cache.set(normalizedWord, parsed);
            
            return parsed;
        } catch (error) {
            console.error('Dictionary API error:', error);
            throw error;
        }
    }

    /**
     * Parse Dictionary API response
     * @param {Array} data - API response data
     * @returns {Object} Parsed dictionary data
     */
    parseResponse(data) {
        if (!data || data.length === 0) {
            throw new Error('No definition found');
        }

        const entry = data[0];
        const result = {
            word: entry.word,
            phonetic: entry.phonetic || '',
            phonetics: [],
            meanings: [],
            audio: null
        };

        // Extract phonetics with audio
        if (entry.phonetics && entry.phonetics.length > 0) {
            for (const phonetic of entry.phonetics) {
                if (phonetic.audio && phonetic.audio.length > 0) {
                    result.audio = phonetic.audio;
                    result.phonetic = phonetic.text || result.phonetic;
                    break;
                }
            }
        }

        // Extract meanings
        if (entry.meanings && entry.meanings.length > 0) {
            for (const meaning of entry.meanings) {
                const meaningData = {
                    partOfSpeech: meaning.partOfSpeech,
                    definitions: []
                };

                if (meaning.definitions && meaning.definitions.length > 0) {
                    // Get first 3 definitions
                    for (let i = 0; i < Math.min(3, meaning.definitions.length); i++) {
                        const def = meaning.definitions[i];
                        meaningData.definitions.push({
                            definition: def.definition,
                            example: def.example || null,
                            synonyms: def.synonyms || []
                        });
                    }
                }

                result.meanings.push(meaningData);
            }
        }

        return result;
    }

    /**
     * Play pronunciation audio
     * @param {string} audioUrl - URL to audio file
     */
    playPronunciation(audioUrl) {
        if (!audioUrl) return;

        // Stop current audio if playing
        if (this.currentAudio) {
            this.currentAudio.pause();
            this.currentAudio = null;
        }

        this.currentAudio = new Audio(audioUrl);
        this.currentAudio.play().catch(error => {
            console.error('Audio playback error:', error);
        });
    }

    /**
     * Render definition in the dictionary panel
     * @param {Object} data - Parsed dictionary data
     * @param {HTMLElement} container - Container element
     */
    renderDefinition(data, container) {
        if (!data || !container) return;

        let html = `
            <div class="dictionary-content">
                <div class="dictionary-header">
                    <h3 class="word-title">${data.word}</h3>
                    ${data.phonetic ? `<span class="phonetic-text">${data.phonetic}</span>` : ''}
                    ${data.audio ? `
                        <button class="audio-btn" onclick="dictionaryAPI.playPronunciation('${data.audio}')">
                            🔊 Play
                        </button>
                    ` : ''}
                </div>
                
                <div class="dictionary-body">
        `;

        // Render meanings
        for (const meaning of data.meanings) {
            html += `
                <div class="meaning-section">
                    <h4 class="part-of-speech">${meaning.partOfSpeech}</h4>
                    <ol class="definitions-list">
            `;

            for (const def of meaning.definitions) {
                html += `
                    <li class="definition-item">
                        <p class="definition-text">${def.definition}</p>
                        ${def.example ? `<p class="example-text"><em>"${def.example}"</em></p>` : ''}
                    </li>
                `;
            }

            html += `
                    </ol>
                </div>
            `;
        }

        html += `
                </div>
            </div>
        `;

        container.innerHTML = html;
        container.classList.add('active');
    }

    /**
     * Show error message in dictionary panel
     * @param {HTMLElement} container - Container element
     * @param {string} message - Error message
     */
    showError(container, message = 'Definition not found') {
        if (!container) return;

        container.innerHTML = `
            <div class="dictionary-error">
                <div class="error-icon">📚</div>
                <h3>Oops!</h3>
                <p>${message}</p>
                <p class="error-subtext">Try another word or check your spelling.</p>
            </div>
        `;
        container.classList.add('active');
    }

    /**
     * Show loading state
     * @param {HTMLElement} container - Container element
     */
    showLoading(container) {
        if (!container) return;

        container.innerHTML = `
            <div class="dictionary-loading">
                <div class="loading-spinner"></div>
                <p>Fetching definition...</p>
            </div>
        `;
        container.classList.add('active');
    }

    /**
     * Clear dictionary panel
     * @param {HTMLElement} container - Container element
     */
    clear(container) {
        if (!container) return;
        container.innerHTML = '';
        container.classList.remove('active');
    }

    /**
     * Clear cache
     */
    clearCache() {
        this.cache.clear();
    }
}

// Create global dictionary API instance
const dictionaryAPI = new DictionaryAPI();
