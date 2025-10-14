# Compiler and flags
CC = cc
CFLAGS = -Wall -Wextra -Werror
NAME = minishell

# Include
INCLUDE = -I./include

# Libft
LIBFT_PATH = ./libft
LIBFT = $(LIBFT_PATH)/libft.a

# Source files
SRCS =  src/lexer/lexer.c \
		src/lexer/lexer_utils.c \
		src/lexer/free_utils.c \
		src/utils/utils.c \
		src/parser/parser.c \
		src/parser/parser_utils.c \
		src/parser/free_utils.c \
		src/main.c

# Object files
OBJ_DIR = obj
OBJS = $(patsubst src/%.c,$(OBJ_DIR)/%.o,$(SRCS))

# Default target
all: $(NAME)

# Build pipex binary (regular)
$(NAME): $(OBJS) $(LIBFT)
	$(CC) $(CFLAGS) $(OBJS) -L $(LIBFT_PATH) -lft -lreadline -o $(NAME)

# Build libftprintf library if needed
$(LIBFT):
	$(MAKE) -C $(LIBFT_PATH) all

# Compile .c to .o
$(OBJ_DIR)/%.o: src/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

# Clean object files
clean:
	$(MAKE) -C $(LIBFT_PATH) clean
	rm -f $(OBJS)

# Full clean: remove objects and binary
fclean: clean
	$(MAKE) -C $(LIBFT_PATH) fclean
	rm -f $(NAME)

# Rebuild everything
re: fclean all

# Phony targets
.PHONY: all clean fclean re 