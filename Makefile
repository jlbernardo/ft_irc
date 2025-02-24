# _*_ Makefile _*_

# BASIC VARIABLES
NAME = ircserv
FLAGS = -Wall -Wextra -Werror -I./include -std=c++98 -g3 -MMD -MP
CXX = clang++

# TEST VARIABLES
TEST_NAME = $(NAME)_test
TEST_OBJ_PATH = test_objects/
TEST_FLAGS = $(FLAGS) -DTEST -fsanitize=address
PORT ?= 4242
PASS = 2508

# SOURCE FILES
MAIN = main
UTILS = Validator Logger utils
CLASSES = Client Commands CommandsManager Server SocketsManager Channel
COMMANDS = invite join kick mode nick pass privmsg quit topic user who part

# PATHS
SRC_PATH = src/
OBJ_PATH = objects/
UTL_PATH = src/utils/
CMD_PATH = src/commands/

# BASIC OBJECTS
MAIN_O = $(MAIN:%=$(OBJ_PATH)%.o)
UTILS_O = $(UTILS:%=$(OBJ_PATH)%.o)
CLASSES_O = $(CLASSES:%=$(OBJ_PATH)%.o)
COMMANDS_O = $(COMMANDS:%=$(OBJ_PATH)%.o)

# TEST OBJECTS
TEST_MAIN_O = $(MAIN:%=$(TEST_OBJ_PATH)%.o)
TEST_CLASSES_O = $(CLASSES:%=$(TEST_OBJ_PATH)%.o)

# DEPENDENCIES
DEPS = $(CLASSES_O:.o=.d) $(MAIN_O:.o=.d) $(CLASSES_O:.o=.d) $(TEST_MAIN_O:.o=.d) $(UTILS_O:.o=.d) $(COMMANDS_O:.o=.d)

# COLOR CODES
DFL = \033[0m
GRN = \033[32;1m
BLU = \033[34;1m
RED = \033[31;1m


# BASIC RULES

all: $(NAME)

run: all
	@echo "$(BLU) Starting IRC Server... \n$(DFL)"
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(NAME) $(PORT) $(PASS)

clean:
	@rm -rf $(OBJ_PATH)
	@echo "$(RED) [EXCLUDED] $(OBJ_PATH)$(DFL)"
	@rm -rf $(TEST_OBJ_PATH)
	@echo "$(RED) [EXCLUDED] $(TEST_OBJ_PATH)$(DFL)"

fclean: clean
	@rm -f $(NAME)
	@echo "$(RED) [EXCLUDED] $(NAME)$(DFL)"
	@rm -f $(TEST_NAME)
	@echo "$(RED) [EXCLUDED] $(TEST_NAME)$(DFL)"
	@rm -f compile_commands.json

re: fclean all



# TEST RULES

test_connections: $(TEST_NAME)
	@echo "$(BLU)\n Testing connections... $(DFL)"
	@make test1 PORT=$(PORT) -C integration_tests/

quickmemtest: $(TEST_NAME)
	@echo "$(BLU)\n Testing memory... $(DFL)"
	@./$(TEST_NAME) $(PORT) $(PASS)



# BASIC COMPILATION RULES

$(NAME): $(MAIN_O) $(CLASSES_O) $(COMMANDS_O) $(UTILS_O)
	@mkdir -p $(dir $@)
	@$(CXX) $(FLAGS) $^ -o $@
	@touch $@
	@echo "$(BLU)\n All done!  ദ്ദി(• ˕ •マ.ᐟ$(DFL)"

$(MAIN_O): $(SRC_PATH)$(MAIN).cpp
	@mkdir -p $(OBJ_PATH)
	@$(CXX) $(FLAGS) -c $< -o $@
	@echo "$(GRN) [COMPILED]" $@ "$(DFL)"

$(OBJ_PATH)%.o: $(SRC_PATH)%.cpp
	@mkdir -p $(OBJ_PATH)
	@$(CXX) $(FLAGS) -c $< -o $@
	@echo "$(GRN) [COMPILED]" $@ "$(DFL)"

$(OBJ_PATH)%.o: $(UTL_PATH)%.cpp
	@mkdir -p $(OBJ_PATH)
	@$(CXX) $(FLAGS) -c $< -o $@
	@echo "$(GRN) [COMPILED]" $@ "$(DFL)"

$(OBJ_PATH)%.o: $(CMD_PATH)%.cpp
	@mkdir -p $(OBJ_PATH)
	@$(CXX) $(FLAGS) -c $< -o $@
	@echo "$(GRN) [COMPILED]" $@ "$(DFL)"


# TEST COMPILATION RULES

$(TEST_NAME): $(CLASSES_O) $(TEST_MAIN_O)
	@mkdir -p $(dir $@)
	@$(CXX) $(TEST_FLAGS) $^ -o $@
	@touch $@
	@echo "$(GRN) [COMPILED]" $@ "$(DFL)"

$(TEST_MAIN_O): $(SRC_PATH)$(MAIN).cpp
	@mkdir -p $(TEST_OBJ_PATH)
	@$(CXX) $(TEST_FLAGS) -c $< -o $@
	@echo "$(GRN) [COMPILED]" $@ "$(DFL)"

$(TEST_OBJ_PATH)%.o: $(SRC_PATH)%.cpp
	@mkdir -p $(TEST_OBJ_PATH)
	@$(CXX) $(TEST_FLAGS) -c $< -o $@
	@echo "$(GRN) [COMPILED]" $@ "$(DFL)"


# INCLUDE FLAG
-include $(DEPS)

# PHONY RULE
.PHONY: all clean fclean re run test_connections
