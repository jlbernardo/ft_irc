NAME = irc
FLAGS = -Wall -Wextra -Werror -std=c++98 -g3 -MMD -MP
CXX = c++
PATH_OBJS = ./objects/

SRCS = Client Message Server
MAIN = main

OBJS = $(SRCS:%=$(PATH_OBJS)%.o)
MAIN_OBJ = $(MAIN:%=$(PATH_OBJS)%.o)

DEPS = $(OBJS:.o=.d) $(MAIN_OBJ:.o=.d)

all: $(NAME)

run:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(NAME)

$(NAME): $(OBJS) $(MAIN_OBJ)
	$(CXX) $(FLAGS) $^ -o $(NAME)

$(PATH_OBJS)%.o: %.cpp
	@mkdir -p $(PATH_OBJS)
	$(CXX) $(FLAGS) -c $< -o $@

$(MAIN_OBJ): $(MAIN).cpp
	@mkdir -p $(PATH_OBJS)
	$(CXX) $(FLAGS) -c $< -o $@

clean:
	rm -rf $(PATH_OBJS)

fclean: clean
	rm -f $(NAME)
	rm -f compile_commands.json

re: fclean all

-include $(DEPS)
