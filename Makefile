PORT ?= 6667
NAME = ircserv
NAME_TEST = ircserv_test
FLAGS = -Wall -Wextra -Werror -I./include -std=c++98 -g3 -MMD -MP
TEST_FLAGS = $(FLAGS) -DTEST -fsanitize=address
CXX = c++
PATH_OBJS = ./objects/
PATH_TEST_OBJS = ./test_objects/
SRC_DIR = src/
UTILS_DIR = src/utils/
CMD_DIR = src/commands/
PASS = 89aX

SRC = Client Commands CommandsManager Server SocketsManager Channel
UTILS_SRC = Validator Logger
CMD_SRC = invite join kick mode nick pass privmsg quit topic user
MAIN = main

OBJS = $(SRC:%=$(PATH_OBJS)%.o)
MAIN_OBJ = $(MAIN:%=$(PATH_OBJS)%.o)
UTILS_OBJS = $(UTILS_SRC:%=$(PATH_OBJS)%.o)
CMD_OBJS = $(CMD_SRC:%=$(PATH_OBJS)%.o)
TEST_OBJS = $(SRC:%=$(PATH_TEST_OBJS)%.o)
TEST_MAIN_OBJ = $(MAIN:%=$(PATH_TEST_OBJS)%.o)

DEPS = $(OBJS:.o=.d) $(MAIN_OBJ:.o=.d) $(TEST_OBJS:.o=.d) $(TEST_MAIN_OBJ:.o=.d) $(UTILS_OBJS:.o=.d) $(CMD_OBJS:.o=.d)

all: $(NAME)

run: all
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(NAME) $(PORT) $(PASS)

test_connections: $(NAME_TEST)
	@make test1 PORT=$(PORT) -C integration_tests/

quickmemtest: $(NAME_TEST)
	@./$(NAME_TEST) $(PORT) $(PASS)

$(NAME): $(OBJS) $(MAIN_OBJ) $(UTILS_OBJS) $(CMD_OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(FLAGS) $^ -o $@
	@touch $@

$(NAME_TEST): $(TEST_OBJS) $(TEST_MAIN_OBJ)
	@mkdir -p $(dir $@)
	$(CXX) $(TEST_FLAGS) $^ -o $@
	@touch $@

$(PATH_OBJS)%.o: $(SRC_DIR)%.cpp
	@mkdir -p $(PATH_OBJS)
	$(CXX) $(FLAGS) -c $< -o $@

$(PATH_TEST_OBJS)%.o: $(SRC_DIR)%.cpp
	@mkdir -p $(PATH_TEST_OBJS)
	$(CXX) $(TEST_FLAGS) -c $< -o $@

$(PATH_OBJS)%.o: $(UTILS_DIR)%.cpp
	@mkdir -p $(PATH_OBJS)
	$(CXX) $(FLAGS) -c $< -o $@

$(PATH_OBJS)%.o: $(CMD_DIR)%.cpp
	@mkdir -p $(PATH_OBJS)
	$(CXX) $(FLAGS) -c $< -o $@

$(MAIN_OBJ): $(SRC_DIR)$(MAIN).cpp
	@mkdir -p $(PATH_OBJS)
	$(CXX) $(FLAGS) -c $< -o $@

$(TEST_MAIN_OBJ): $(SRC_DIR)$(MAIN).cpp
	@mkdir -p $(PATH_TEST_OBJS)
	$(CXX) $(TEST_FLAGS) -c $< -o $@

clean:
	rm -rf $(PATH_OBJS)
	rm -rf $(PATH_TEST_OBJS)

fclean: clean
	rm -f $(NAME)
	rm -f $(NAME_TEST)
	rm -f compile_commands.json

re: fclean all

-include $(DEPS)

.PHONY: all clean fclean re run test_connections
