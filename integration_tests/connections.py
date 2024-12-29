#!/usr/bin/env python3
import socket
import sys
import time
from typing import List

# ANSI color codes
GREEN = '\033[92m'
RED = '\033[91m'
RESET = '\033[0m'
BOLD = '\033[1m'

class IRCClient:
    def __init__(self, host: str, port: int):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.connect((host, port))
        # Read and print data from the server
        self.sock.settimeout(0.01)  # Set a timeout for non-blocking recv
        try:
            data = self.sock.recv(4096)
            if data:
                print(f"Server output: {data.decode('utf-8', errors='replace')}", end='\r')
        except socket.timeout:
            print("Server did not send any data")
            raise
        except Exception as e:
            print(f"Error receiving data: {e}")

def print_result(passed: bool, message: str):
    status = f"\n{GREEN}PASS{RESET}" if passed else f"\n{RED}FAIL{RESET}"
    print(f"{BOLD}{status}{RESET} {message}")

def test_select_limit(port: int):
    print(f"\nTesting IRC server on port {port}")
    print("select() should handle at least 1020 simultaneous connections (FD_SETSIZE - 3)\n")
    
    clients: List[IRCClient] = []
    for _ in range(1024):  # Try to go beyond the limit
        try:
            client = IRCClient('127.0.0.1', port)
            clients.append(client)
            time.sleep(0.01)
        except socket.timeout:
            connections = len(clients)
            if connections >= 1020:
                print_result(True, f"Server handled {connections} connections before failing")
                return True
            else:
                print_result(False, f"Server only handled {connections} connections before failing")
                return False


def main():
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} <port>")
        sys.exit(1)
    
    time.sleep(5)  # Wait for server to start
    try:
        port = int(sys.argv[1])
        if port < 1 or port > 65535:
            raise ValueError
    except ValueError:
        print("Error: Port must be a number between 1 and 65535")
        sys.exit(1)

    try:
        result = test_select_limit(port)
    except KeyboardInterrupt:
        print("\nTest interrupted by user")
        sys.exit(1)

    return result

if __name__ == "__main__":
    main()
