#!/usr/bin/env python3
"""
Simple HTTP Server for Smart Autocomplete Frontend
Serves the frontend files and provides API endpoints
"""

import http.server
import socketserver
import json
import subprocess
import os
from urllib.parse import parse_qs, urlparse
import tempfile

PORT = 8080
FRONTEND_DIR = "frontend"
BACKEND_EXE = "autocomplete.exe"

class AutocompleteHandler(http.server.SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=FRONTEND_DIR, **kwargs)
    
    def do_GET(self):
        """Handle GET requests"""
        parsed = urlparse(self.path)
        
        if parsed.path == '/api/status':
            self.send_json({'status': 'online', 'backend': os.path.exists(BACKEND_EXE)})
        else:
            super().do_GET()
    
    def do_POST(self):
        """Handle POST requests for autocomplete and spell check"""
        if self.path == '/api/autocomplete':
            self.handle_autocomplete()
        elif self.path == '/api/spellcheck':
            self.handle_spellcheck()
        else:
            self.send_error(404, "Endpoint not found")
    
    def handle_autocomplete(self):
        """Process autocomplete request"""
        try:
            content_length = int(self.headers['Content-Length'])
            post_data = self.rfile.read(content_length)
            data = json.loads(post_data.decode('utf-8'))
            
            query = data.get('query', '').strip()
            
            if not query:
                self.send_json({'error': 'No query provided'}, 400)
                return
            
            import time
            start = time.time()
            
            # Handle both sentences and single words
            # Get suggestions for the last word in the query
            words = query.split()
            if words:
                last_word = words[-1]
                suggestions = self.get_autocomplete_suggestions(last_word)
                
                # If query has multiple words, prepend the prefix
                if len(words) > 1:
                    prefix = ' '.join(words[:-1]) + ' '
                    suggestions = [prefix + s for s in suggestions]
            else:
                suggestions = []
            
            elapsed = (time.time() - start) * 1000  # ms
            
            self.send_json({
                'query': query,
                'suggestions': suggestions,
                'time_ms': round(elapsed, 2),
                'count': len(suggestions)
            })
            
        except Exception as e:
            self.send_json({'error': str(e)}, 500)
    
    def handle_spellcheck(self):
        """Process spell check request"""
        try:
            content_length = int(self.headers['Content-Length'])
            post_data = self.rfile.read(content_length)
            data = json.loads(post_data.decode('utf-8'))
            
            text = data.get('word', '').strip()
            
            if not text:
                self.send_json({'error': 'No text provided'}, 400)
                return
            
            import time
            start = time.time()
            
            # Handle sentences by checking each word
            words = text.split()
            incorrect_words = []
            all_suggestions = {}
            
            for word in words:
                # Skip punctuation-only "words"
                clean_word = ''.join(c for c in word if c.isalnum())
                if not clean_word:
                    continue
                    
                result = self.check_spelling(clean_word)
                if not result['correct']:
                    incorrect_words.append(clean_word)
                    all_suggestions[clean_word] = result['suggestions']
            
            is_correct = len(incorrect_words) == 0
            
            elapsed = (time.time() - start) * 1000  # ms
            
            self.send_json({
                'word': text,
                'correct': is_correct,
                'incorrect_words': incorrect_words,
                'suggestions': all_suggestions if not is_correct else {},
                'time_ms': round(elapsed, 2)
            })
            
        except Exception as e:
            self.send_json({'error': str(e)}, 500)
    
    def get_autocomplete_suggestions(self, prefix):
        """Get suggestions for a prefix (simplified)"""
        # Read dictionary and filter
        words = []
        try:
            with open('data/dictionary.txt', 'r') as f:
                for line in f:
                    parts = line.strip().split()
                    if parts and parts[0].lower().startswith(prefix.lower()):
                        words.append(parts[0])
        except:
            pass
        
        return words[:10]  # Top 10
    
    def check_spelling(self, word):
        """Check spelling of a word"""
        try:
            with open('data/dictionary.txt', 'r') as f:
                valid_words = [line.strip().split()[0].lower() for line in f if line.strip()]
            
            is_correct = word.lower() in valid_words
            
            if is_correct:
                return {'correct': True, 'suggestions': []}
            else:
                # Simple fuzzy matching
                suggestions = [w for w in valid_words if self.edit_distance(word.lower(), w) <= 2][:5]
                return {'correct': False, 'suggestions': suggestions}
        except:
            return {'correct': False, 'suggestions': []}
    
    def edit_distance(self, s1, s2):
        """Calculate Levenshtein distance"""
        if len(s1) < len(s2):
            return self.edit_distance(s2, s1)
        
        if len(s2) == 0:
            return len(s1)
        
        previous_row = range(len(s2) + 1)
        for i, c1 in enumerate(s1):
            current_row = [i + 1]
            for j, c2 in enumerate(s2):
                insertions = previous_row[j + 1] + 1
                deletions = current_row[j] + 1
                substitutions = previous_row[j] + (c1 != c2)
                current_row.append(min(insertions, deletions, substitutions))
            previous_row = current_row
        
        return previous_row[-1]
    
    def send_json(self, data, status=200):
        """Send JSON response"""
        self.send_response(status)
        self.send_header('Content-Type', 'application/json')
        self.send_header('Access-Control-Allow-Origin', '*')
        self.end_headers()
        self.wfile.write(json.dumps(data).encode())
    
    def log_message(self, format, *args):
        """Custom log format"""
        print(f"[{self.log_date_time_string()}] {format % args}")

def main():
    # Change to DSA_Proj_G3 directory
    os.chdir(os.path.dirname(os.path.abspath(__file__)))
    
    print("="*60)
    print("  Smart Autocomplete & Spell-Checker Web Server")
    print("="*60)
    print(f"\n  Server starting on http://localhost:{PORT}")
    print(f"  Frontend directory: {FRONTEND_DIR}")
    print(f"  Backend executable: {BACKEND_EXE}")
    print(f"\n  Open your browser and go to:")
    print(f"  → http://localhost:{PORT}/index.html")
    print(f"\n  Press Ctrl+C to stop the server")
    print("="*60)
    print()
    
    with socketserver.TCPServer(("", PORT), AutocompleteHandler) as httpd:
        try:
            httpd.serve_forever()
        except KeyboardInterrupt:
            print("\n\nServer stopped.")

if __name__ == "__main__":
    main()
