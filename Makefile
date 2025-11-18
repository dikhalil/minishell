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
		src/lexer/lexer_extract_word.c \
		src/lexer/lexer_token_utils.c \
		src/lexer/lexer_word_utils.c \
		src/lexer/lexer_char_utils.c \
		src/parser/parser.c \
		src/parser/parser_cmd_utils.c \
		src/parser/parser_arg_utils.c \
		src/parser/parser_redir_utils.c \
		src/parser/parser_free_utils.c \
		src/heredoc/heredoc.c\
		src/heredoc/heredoc_utils.c\
		src/expand/expand_extract.c \
		src/expand/expand_str.c \
		src/env/env.c \
		src/env/shlvl.c \
		src/clean/cleanup.c \
		src/signals/signals.c \
		src/signals/child_signal.c \
		src/shell/shell.c \
		src/shell/main.c\
		src/executor/executor.c \
		src/executor/executor_utils.c \
		src/executor/builtins/cd.c\
		src/executor/builtins/cd_utils.c\
		src/executor/builtins/echo.c\
		src/executor/builtins/env.c\
		src/executor/builtins/exit.c\
		src/executor/builtins/unset.c\
		src/executor/builtins/export.c\
		src/executor/builtins/export_env.c\
		src/executor/builtins/export_utils.c\
		src/executor/builtins/pwd.c\
		src/executor/check_cmd.c \
		src/executor/check_cmd_utils.c \
		src/executor/get_path.c \
		src/executor/delete_node.c \
		src/executor/execute_program.c \
		src/executor/build_cmd_args.c \
		src/executor/fork_and_execute.c \
		src/executor/envp_to_list.c \
		src/executor/execution.c \
		src/executor/execution_utils.c

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
