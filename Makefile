# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: minishell                                  +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/01/01 00:00:00 by minishell         #+#    #+#              #
#    Updated: 2024/01/01 00:00:00 by minishell        ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = minishell

CC = cc
CFLAGS = -Wall -Wextra -Werror
INCLUDES = -I includes -I libft

LIBFT_DIR = libft
LIBFT = $(LIBFT_DIR)/libft.a

SRCS = src/main.c \
       src/env/env.c \
       src/env/env_utils.c \
       src/lexer/lexer.c \
       src/lexer/lexer_utils.c \
       src/lexer/token.c \
       src/parser/parser.c \
       src/parser/parser_utils.c \
       src/expander/expander.c \
       src/expander/expander_utils.c \
       src/executor/executor.c \
       src/executor/executor_utils.c \
       src/executor/redirections.c \
       src/executor/pipe.c \
       src/builtins/builtins.c \
       src/builtins/builtin_echo.c \
       src/builtins/builtin_cd.c \
       src/builtins/builtin_pwd.c \
       src/builtins/builtin_export.c \
       src/builtins/builtin_unset.c \
       src/builtins/builtin_env.c \
       src/builtins/builtin_exit.c \
       src/signals/signals.c \
       src/termcap/termcap.c \
       src/termcap/termcap_keys.c \
       src/termcap/history.c

OBJS = $(SRCS:.c=.o)

LIBS = -lncurses

all: $(NAME)

$(NAME): $(LIBFT) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBFT) $(LIBS) -o $(NAME)

$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(MAKE) -C $(LIBFT_DIR) clean
	rm -f $(OBJS)

fclean: clean
	$(MAKE) -C $(LIBFT_DIR) fclean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
