# 🔍 Smart Autocomplete & Spell-Checker Suite

A high-performance autocomplete and spell-checking system built from scratch using custom data structures in C++. Features real-time suggestions, intelligent spell correction, and beautiful web-based visualization.

![C++](https://img.shields.io/badge/C++-17-blue.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)
![Status](https://img.shields.io/badge/status-active-success.svg)

## ✨ Features

- **⚡ Lightning-Fast Autocomplete**: Sub-millisecond query times using optimized Trie data structure
- **🔧 Intelligent Spell Correction**: Advanced BK-Tree algorithm for fuzzy string matching
- **💾 Smart Caching**: LRU Cache for frequently accessed queries (O(1) lookup)
- **🌸 Bloom Filter**: Efficient membership testing to reduce false lookups
- **📊 Priority Queue**: Intelligent result ranking based on word frequency
- **🎨 Beautiful Web Interface**: Modern, responsive UI with dark mode support
- **📈 Real-time Visualization**: Interactive Trie traversal animation
- **📊 Performance Metrics**: Live statistics and complexity analysis

## 🏗️ Architecture

### Core Data Structures

1. **Trie (Prefix Tree)** - O(m) prefix search where m = query length
2. **LRU Cache** - O(1) cached query retrieval
3. **Bloom Filter** - O(k) probabilistic membership test
4. **BK-Tree** - O(log n) edit distance-based search
5. **Priority Queue** - O(k log n) for top-k result ranking
6. **Hash Table** - O(1) frequency lookups

### Project Structure

```
Smart-Autocomplete-Spell-Checker-Suite/
├── src/                    # C++ source files
│   ├── main.cpp           # CLI application entry point
│   ├── trie.cpp           # Trie implementation
│   ├── bloomfilter.cpp    # Bloom filter
│   ├── bktree.cpp         # BK-Tree for spell checking
│   ├── lrucache.cpp       # LRU cache
│   ├── hashtable.cpp      # Hash table
│   ├── priorityqueue.cpp  # Priority queue
│   ├── spellchecker.cpp   # Spell checking engine
│   ├── filehandler.cpp    # File I/O operations
│   └── utils.cpp          # Utility functions
├── include/               # Header files
├── frontend/              # Web interface
│   ├── index.html        # Main UI
│   ├── style.css         # Styling
│   ├── app_simple.js     # Application logic
│   └── visualizer.js     # Trie visualization
├── data/                  # Dictionary files
│   └── dictionary.txt    # 5000+ words and phrases
├── tests/                 # Unit tests
├── build.bat             # Windows build script
├── Makefile              # Unix/Linux build
└── README.md             # This file
```

## 🚀 Quick Start

### Prerequisites

- **C++ Compiler**: g++ with C++17 support (MinGW for Windows, GCC/Clang for Linux/Mac)
- **Python 3.x**: For web server (optional)
- **Git**: For cloning the repository

### Installation

```bash
# Clone the repository
git clone https://github.com/redwolf261/Smart-Autocomplete-Spell-Checker-Suite.git
cd Smart-Autocomplete-Spell-Checker-Suite

# Build the project (Windows)
build.bat

# Build the project (Linux/Mac)
make

# Run the application
./autocomplete.exe          # Windows
./autocomplete              # Linux/Mac
```

### Running the Web Interface

```bash
# Start the web server
python webserver.py

# Open browser to http://localhost:8000/frontend/index.html
```

Or simply open `frontend/index.html` directly in your browser.

## 💻 Usage

### Command Line Interface

```bash
# Interactive mode
./autocomplete

# Enter words or prefixes to get suggestions
> hel
Suggestions: hello, help, helicopter, ...

# Spell check mode
> mispeled
Did you mean: misspelled?
```

### Web Interface

1. **Autocomplete Mode**: Type any prefix to see real-time suggestions
2. **Spell Check Mode**: Enter a word to check spelling and get corrections
3. **Visualization**: Watch the Trie traversal animation as you type
4. **Statistics**: View performance metrics and cache hit rates

## 🎯 Key Features Explained

### 1. Autocomplete Engine
- Uses Trie data structure for efficient prefix matching
- Ranks results by word frequency using Priority Queue
- Caches recent queries in LRU Cache for instant retrieval

### 2. Spell Checker
- Implements BK-Tree for edit distance calculations
- Suggests corrections within configurable edit distance
- Uses Bloom Filter to quickly eliminate non-words

### 3. Performance Optimization
- **Space-efficient**: Compressed Trie nodes
- **Time-efficient**: Multiple data structures working together
- **Memory-efficient**: LRU Cache prevents unbounded memory growth

## 📊 Performance Benchmarks

| Operation | Time Complexity | Actual Performance |
|-----------|----------------|-------------------|
| Trie Insert | O(m) | < 0.1ms |
| Prefix Search | O(m + k) | < 1ms |
| Cache Lookup | O(1) | < 0.01ms |
| Spell Check | O(log n) | < 5ms |
| Bloom Filter | O(k) | < 0.001ms |

*Tested on Intel Core i7 with 5000+ word dictionary*

## 🧪 Testing

```bash
# Run all tests (Windows)
RUN_ALL_TESTS.bat

# Run all tests (Linux/Mac)
make test

# Run specific tests
./build/test_trie
./build/test_editdistance
./build/test_sort
```

## 📚 Dictionary

The dictionary includes:
- 5000+ common English words
- Technical programming terms
- 130+ common phrases and idioms
- Frequency scores for intelligent ranking

Add custom words by editing `data/dictionary.txt`:
```
yourword 500
anotherword 300
```

## 🛠️ Configuration

Edit `src/main.cpp` to configure:
- **Cache Size**: `LRU_CACHE_SIZE` (default: 100)
- **Bloom Filter Size**: `BLOOM_SIZE` (default: 10000)
- **Max Suggestions**: `MAX_RESULTS` (default: 10)
- **Edit Distance**: `MAX_DISTANCE` (default: 2)

## 🎨 Customization

### Web Interface
- Modify `frontend/style.css` for styling
- Edit `frontend/app_simple.js` for behavior
- Dark mode toggle included by default

### Adding New Data Structures
1. Create header in `include/yourstructure.h`
2. Implement in `src/yourstructure.cpp`
3. Integrate in `src/spellchecker.cpp` or `src/autocomplete.cpp`

## 🤝 Contributing

Contributions are welcome! Please follow these steps:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## 📝 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 👨‍💻 Author

**Gandharv Shinde**
- GitHub: [@redwolf261](https://github.com/redwolf261)

## 🙏 Acknowledgments

- Inspired by modern search engines and IDE autocomplete features
- Built as a Data Structures and Algorithms project (DSA Project G3)
- Thanks to all contributors and testers

## 📞 Support

If you encounter any issues or have questions:
- Open an [Issue](https://github.com/redwolf261/Smart-Autocomplete-Spell-Checker-Suite/issues)
- Check existing issues for solutions
- Contribute improvements via Pull Requests

## 🗺️ Roadmap

- [ ] Add support for multi-language dictionaries
- [ ] Implement fuzzy phonetic matching (Soundex/Metaphone)
- [ ] Add API server mode with REST endpoints
- [ ] Mobile-responsive improvements
- [ ] Context-aware suggestions
- [ ] Machine learning-based ranking

## ⭐ Star History

If you find this project useful, please consider giving it a star! ⭐

---

**Built with ❤️ using C++17 and Vanilla JavaScript**
