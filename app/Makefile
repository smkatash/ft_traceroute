# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: kanykei <kanykei@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/01/11 11:19:19 by kanykei           #+#    #+#              #
#    Updated: 2024/09/11 11:50:20 by kanykei          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# ================================= Target =================================== #
NAME		:= ft_traceroute

# ================================ Sources =================================== #

VPATH		:= src

SRC			:= main.c parser.c ft_traceroute.c socket.c utils.c

# ================================ Flags ===================================== #
OBJ			:= $(addprefix obj/,$(notdir $(SRC:.c=.o)))
CC			:= gcc
CFLAGS 		:= -Wall -Wextra -Werror #-fsanitize=address

# ============================== Libraries ==================================== #
INCFL		:=	-I./include # -I./libs/libft
#LIBS		:= 	-L./libs/libft -lft
#LIBFT		:=	libs/libft/libft.a

# ================================ Rules ===================================== #
.SILENT:

all: $(NAME) 

#$(CC) $(OBJ) $(LIBS) -o $(NAME)
$(NAME)		: $(OBJ) #$(LIBFT) 
			$(CC) $(OBJ) -o $(NAME)
			@echo "\033[1;32m ft_traceroute is compiled \033[0m"

obj/%.o		: %.c | obj
			$(CC) $(CFLAGS) $(INCFL) -c $< -o $@

obj			:
			mkdir obj

# $(LIBFT):
# 			- (cd libs/libft && make && make clean)

clean		:
			rm -rf obj
			@echo "deleting object files..."

fclean		: clean
			rm $(NAME)
			@echo "deleting ft_traceroute..."
			- (cd libs/libft && make fclean)

re			: clean all

.PHONY		:	all clean fclean re 
