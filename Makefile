PORT ?= 6667
NAME = ircserv
NAME2 = ircserv_sanitize
FLAGS = -Wall -Wextra -Werror -std=c++98 -g3 -MMD -MP
MEMFLAGS = -Wall -Wextra -Werror -std=c++98 -g3 -MMD -MP -fsanitize=address
CXX = c++
PATH_OBJS = ./objects/
SRC_DIR = srcs/
PASS = 89aX

SRCS = Client Message Server InputValidator SocketsManager
MAIN = main

OBJS = $(SRCS:%=$(PATH_OBJS)%.o)
MAIN_OBJ = $(MAIN:%=$(PATH_OBJS)%.o)

DEPS = $(OBJS:.o=.d) $(MAIN_OBJ:.o=.d)

.PHONY: all clean fclean re run test_connections

all: $(NAME)

run: all
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(NAME) $(PORT) $(PASS)

quickmemtest: fclean $(NAME2)
	./$(NAME2) $(PORT) $(PASS)

test_connections:
	@make test1 PORT=$(PORT) -C integration_tests/

$(NAME): $(OBJS) $(MAIN_OBJ)
	@mkdir -p $(dir $@)
	$(CXX) $(FLAGS) $^ -o $@
	@touch $@

$(PATH_OBJS)%.o: $(SRC_DIR)%.cpp
	@mkdir -p $(PATH_OBJS)
	$(CXX) $(FLAGS) -c $< -o $@

$(MAIN_OBJ): $(SRC_DIR)$(MAIN).cpp
	@mkdir -p $(PATH_OBJS)
	$(CXX) $(FLAGS) -c $< -o $@

$(NAME2): $(OBJS) $(MAIN_OBJ)
	@mkdir -p $(dir $@)
	$(CXX) $(MEMFLAGS) $^ -o $@
	@touch $@

clean:
	rm -rf $(PATH_OBJS)

fclean: clean
	rm -f $(NAME)
	rm -f $(NAME2)
	rm -f compile_commands.json

re: fclean all

-include $(DEPS)
