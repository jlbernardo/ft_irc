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

    def close(self):
        self.sock.close()

def print_result(passed: bool, message: str):
    status = f"\n{GREEN}PASS{RESET}" if passed else f"\n{RED}FAIL{RESET}"
    print(f"{BOLD}{status}{RESET} {message}")

def test_select_limit(port: int):
    print(f"\nTesting IRC server on port {port}")
    print("select() should handle at least 1024 simultaneous connections (FD_SETSIZE)\n")
    
    clients: List[IRCClient] = []
    try:
        for i in range(2048):  # Try to go one beyond the limit
            try:
                client = IRCClient('localhost', port)
                clients.append(client)
                time.sleep(0.01)
            except socket.error as e:
                connections = len(clients)
                if connections >= 1024:
                    print_result(True, f"Server handled {connections} connections before failing")
                else:
                    print_result(False, f"Server only handled {connections} connections before failing")
                return
    finally:
        print("\nCleaning up connections...")
        for client in clients:
            try:
                client.close()
            except:
                pass

def main():
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} <port>")
        sys.exit(1)
    
    try:
        port = int(sys.argv[1])
        if port < 1 or port > 65535:
            raise ValueError
    except ValueError:
        print("Error: Port must be a number between 1 and 65535")
        sys.exit(1)

    try:
        test_select_limit(port)
    except KeyboardInterrupt:
        print("\nTest interrupted by user")
        sys.exit(1)

if __name__ == "__main__":
    main()
