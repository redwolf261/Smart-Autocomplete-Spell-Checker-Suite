#!/usr/bin/env python3
"""
Integration Bridge for Smart Autocomplete & Spell-Checker Suite

This script serves as an optional bridge between the frontend and backend,
providing a simple HTTP server that can:
1. Receive queries from the frontend
2. Write them to data/query.txt
3. Execute the C++ backend binary
4. Read results from data/output.json
5. Return results to the frontend

Usage:
    python bridge.py [--port PORT] [--binary PATH]

Alternative: For file-based integration without this bridge,
the frontend can directly monitor output.json after writing query.txt
"""

import json
import subprocess
import os
import sys
import time
from http.server import HTTPServer, BaseHTTPRequestHandler
from urllib.parse import parse_qs
import argparse

class IntegrationHandler(BaseHTTPRequestHandler):
    binary_path = "./autocomplete"
    data_dir = "data"
    
    def do_GET(self):
        """Handle GET requests"""
        if self.path == '/':
            self.send_html()
        elif self.path == '/status':
            self.send_status()
        else:
            self.send_404()
    
    def do_POST(self):
        """Handle POST requests for queries"""
        if self.path == '/query':
            self.handle_query()
        else:
            self.send_404()
    
    def handle_query(self):
        """Process a query through the backend"""
        try:
            # Read request body
            content_length = int(self.headers['Content-Length'])
            post_data = self.rfile.read(content_length)
            data = json.loads(post_data.decode('utf-8'))
            
            query = data.get('query', '')
            mode = data.get('mode', 'autocomplete')
            
            if not query:
                self.send_json({'error': 'No query provided'}, 400)
                return
            
            # Write query to file
            query_file = os.path.join(self.data_dir, 'query.txt')
            with open(query_file, 'w') as f:
                f.write(query)
            
            # Execute backend binary
            try:
                # Run with timeout
                result = subprocess.run(
                    [self.binary_path],
                    timeout=5,
                    capture_output=True,
                    text=True
                )
                
                # Read output
                output_file = os.path.join(self.data_dir, 'output.json')
                if os.path.exists(output_file):
                    with open(output_file, 'r') as f:
                        output_data = json.load(f)
                    self.send_json(output_data)
                else:
                    self.send_json({'error': 'No output generated'}, 500)
                    
            except subprocess.TimeoutExpired:
                self.send_json({'error': 'Backend timeout'}, 504)
            except FileNotFoundError:
                self.send_json({'error': 'Backend binary not found'}, 500)
                
        except Exception as e:
            self.send_json({'error': str(e)}, 500)
    
    def send_json(self, data, status=200):
        """Send JSON response"""
        self.send_response(status)
        self.send_header('Content-Type', 'application/json')
        self.send_header('Access-Control-Allow-Origin', '*')
        self.end_headers()
        self.wfile.write(json.dumps(data).encode('utf-8'))
    
    def send_html(self):
        """Send simple HTML page"""
        html = """
        <html>
        <head><title>Autocomplete Bridge</title></head>
        <body>
            <h1>Smart Autocomplete & Spell-Checker Bridge</h1>
            <p>Bridge server is running!</p>
            <p>POST queries to /query endpoint</p>
            <p>Check status at /status endpoint</p>
        </body>
        </html>
        """
        self.send_response(200)
        self.send_header('Content-Type', 'text/html')
        self.end_headers()
        self.wfile.write(html.encode('utf-8'))
    
    def send_status(self):
        """Send status information"""
        status = {
            'status': 'running',
            'binary': self.binary_path,
            'binary_exists': os.path.exists(self.binary_path),
            'data_dir': self.data_dir,
            'data_dir_exists': os.path.exists(self.data_dir)
        }
        self.send_json(status)
    
    def send_404(self):
        """Send 404 response"""
        self.send_response(404)
        self.end_headers()
        self.wfile.write(b'404 Not Found')
    
    def log_message(self, format, *args):
        """Custom log format"""
        sys.stderr.write(f"[{self.log_date_time_string()}] {format % args}\n")

def main():
    parser = argparse.ArgumentParser(description='Integration Bridge Server')
    parser.add_argument('--port', type=int, default=8000, help='Server port (default: 8000)')
    parser.add_argument('--binary', default='./autocomplete', help='Path to backend binary')
    parser.add_argument('--data-dir', default='data', help='Data directory path')
    
    args = parser.parse_args()
    
    IntegrationHandler.binary_path = args.binary
    IntegrationHandler.data_dir = args.data_dir
    
    # Create data directory if it doesn't exist
    os.makedirs(args.data_dir, exist_ok=True)
    
    server = HTTPServer(('localhost', args.port), IntegrationHandler)
    
    print(f"╔════════════════════════════════════════════════════╗")
    print(f"║  Smart Autocomplete Integration Bridge Server     ║")
    print(f"╚════════════════════════════════════════════════════╝")
    print(f"\n✓ Server running at http://localhost:{args.port}")
    print(f"✓ Backend binary: {args.binary}")
    print(f"✓ Data directory: {args.data_dir}")
    print(f"\nPress Ctrl+C to stop the server\n")
    
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print("\n\nShutting down server...")
        server.shutdown()
        print("Server stopped.")

if __name__ == '__main__':
    main()
