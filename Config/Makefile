# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: julnolle <julnolle@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/03/16 17:10:11 by julnolle          #+#    #+#              #
#    Updated: 2021/04/15 16:00:40 by julnolle         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = parser

# ---------------- CC ---------------- #

CC = clang++

CFLAGS		+=	-Wall
CFLAGS		+=	-Wextra

ifneq ($(err), no)
	CFLAGS +=	-Werror
endif

EXTRAFLAGS	=	-std=c++98

ifeq ($(leak), yes)
	SANITIZE	=	-g
else
	SANITIZE	=	-g3 -fsanitize=address,undefined
endif

# -------------- INCLUDES ------------ #

INCLUDES 		=	../includes/
UTILS	 		=	../Utils/

ALL_INC			= 	-I$(INCLUDES) -I$(UTILS)

HEAD += ConfParser.hpp
HEAD += HttpBlock.hpp
HEAD += ServerBlock.hpp
HEAD += LocationBlock.hpp

# ---------------- SRC --------------- #

SRCS += ConfParser.cpp
SRCS += HttpBlock.cpp
SRCS += ServerBlock.cpp
SRCS += LocationBlock.cpp
# SRCS += utils.cpp
SRCS += main.cpp

vpath %.cpp ./
# vpath %.cpp ../Utils/

# ---------------- OBJ --------------- #

DIR_OBJS	= ./objs/
OBJS		= $(patsubst %.cpp, $(DIR_OBJS)%.o, $(SRCS))


TEST		= $(patsubst %.cpp, %.hpp, $(SRCS))


# --------------- RULES -------------- #

all: $(NAME)

$(NAME):	$(OBJS)
			$(CC) $(CFLAGS) $(EXTRAFLAGS) $(SANITIZE) $(OBJS) $(ALL_INC) -o $@
# 			echo "$(_BOLD)$(_YELLOW)==> $@ linked$(_END)"

$(OBJS): 	$(DIR_OBJS)%.o: %.cpp $(HEAD) | $(DIR_OBJS)
			$(CC) $(CFLAGS) $(EXTRAFLAGS) $(SANITIZE) $(ALL_INC) -c $< -o $@
# 			echo "$(_BOLD)$(_GREEN)--> $@ made$(_END)"

$(DIR_OBJS):
	mkdir -p $@

clean:
	$(RM) -R $(DIR_OBJS)
# 	echo "$(_BOLD)$(_RED)-> $@ $(NAME) made$(_END)"

fclean: clean
	$(RM) $(NAME)
# 	echo "$(_BOLD)$(_RED)-> $@ $(NAME) made$(_END)"

re: fclean
	$(MAKE)

.PHONY:		all fast clean fclean re
# .SILENT:	$(NAME) $(OBJS) $(DIR_OBJS) clean fclean
