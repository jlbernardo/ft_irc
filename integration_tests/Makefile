# Default port if not specified
PORT ?= 6667

# Path to server executable
SERVER = ../ircserv

# Colors for output
GREEN := \033[32m
RESET := \033[0m
BOLD := \033[1m

test1: $(SERVER)
	@echo "$(BOLD)Starting IRC server with valgrind on port $(PORT)...$(RESET)"
	@(make quickmemtest -C ../ PORT=$(PORT) & server_pid=$$!; \
	echo "$(BOLD)Running connection test...$(RESET)"; \
	./connections.py $(PORT); \
	kill $$server_pid; \
	exit $$?)

$(SERVER):
	@make -C ../

# Usage help
help:
	@echo "$(BOLD)Usage:$(RESET)"
	@echo "  make test1 PORT=<port_number>"
	@echo "\n$(BOLD)Example:$(RESET)"
	@echo "  make test PORT=6667"

PHONY: test1 help
