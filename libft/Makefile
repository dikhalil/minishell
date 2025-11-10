# Source files
SRCS =	ft_isalpha.c \
      	ft_isdigit.c \
	ft_isalnum.c \
	ft_isascii.c \
	ft_isprint.c \
	ft_strlen.c \
	ft_memset.c \
	ft_bzero.c \
	ft_memcpy.c \
	ft_memmove.c \
	ft_strlcpy.c \
	ft_strlcat.c \
	ft_strcmp.c \
	ft_toupper.c \
	ft_tolower.c \
	ft_strchr.c \
	ft_strrchr.c \
	ft_strncmp.c \
	ft_memcmp.c \
	ft_memchr.c \
	ft_strnstr.c \
	ft_atoi.c \
	ft_calloc.c \
	ft_strdup.c \
	ft_substr.c \
	ft_strjoin.c \
	ft_strtrim.c \
	ft_split.c \
	ft_itoa.c \
	ft_strmapi.c \
	ft_striteri.c \
	ft_putchar_fd.c \
	ft_putstr_fd.c \
	ft_putendl_fd.c \
	ft_putnbr_fd.c \
	get_next_line.c\
	get_next_line_utils.c\
	ft_printf.c \
	ft_atoll.c \
	ft_handle_format.c \
	ft_putchr_o.c \
	ft_putstr_o.c \
	ft_putnbr_o.c \
	ft_puthex.c \
	ft_putptr.c \
	ft_lstadd_back_bonus.c \
    ft_lstadd_front_bonus.c \
    ft_lstclear_bonus.c \
    ft_lstdelone_bonus.c \
    ft_lstiter_bonus.c \
    ft_lstlast_bonus.c \
    ft_lstmap_bonus.c \
	ft_str_join_chr.c \
	ft_isnumber.c \
    ft_lstnew_bonus.c \
    ft_lstsize_bonus.c




# Object files (regular a)
OBJS = $(SRCS:.c=.o)

# Compiler and flags
CC = cc
CFLAGS = -Wall -Werror -Wextra
NAME = libft.a

# Default target: build the library
all: $(NAME)

# Build the main library from regular object files
$(NAME): $(OBJS)
	ar rcs $(NAME) $(OBJS)

# Compilation rule for object files
%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean: remove object files
clean:
	rm -f $(OBJS)

# Full clean: remove object files and the library
fclean: clean
	rm -f $(NAME)

# Rebuild everything: clean and then all
re: fclean all

# Phony targets
.PHONY: all clean fclean re
