	
NAME = webserv

# -------------- COLORS -------------- #

RED			= \033[1;31m
GREEN		= \033[1;32m
YELLOW		= \033[1;33m
END			= \033[1;0m

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

INCLUDES 		=	./includes/
SERVER_DIR 		=	./Server/
REQUEST_DIR 	=	./Request/
CONF_DIR 		=	./Config/
UTILS_DIR 		=	./Utils/

ALL_INC			+= 	-I$(INCLUDES)
ALL_INC			+= 	-I$(SERVER_DIR)
ALL_INC			+= 	-I$(REQUEST_DIR)
ALL_INC			+= 	-I$(CONF_DIR)
ALL_INC			+= 	-I$(UTILS_DIR)
# ALL_INC			+= 	-I$(LIBFT_DIR)includes/


# ---------------- SRC --------------- #
# Add files to compile here

# main sources
SRCS += webserv.cpp

# core sources
SRCS += Server.cpp
SRCS += Response.cpp
SRCS += request_class.cpp
SRCS += request_class_store.cpp
SRCS += request_class_utils.cpp

# Config Parser sources
SRCS += ConfParser.cpp
SRCS += HttpBlock.cpp
SRCS += ServerBlock.cpp
SRCS += LocationBlock.cpp

# Utils sources
SRCS += get_next_line.cpp
SRCS += get_next_line_utils.cpp
SRCS += ip_manipulation.cpp
SRCS += utils.cpp


# Add directories containing files to compile here
vpath %.cpp $(SERVER_DIR)
vpath %.cpp $(REQUEST_DIR)
vpath %.cpp $(UTILS_DIR)
vpath %.cpp $(CONF_DIR)


# ---------------- OBJ --------------- #

DIR_OBJS	= ./objs/
OBJS		= $(patsubst %.cpp, $(DIR_OBJS)%.o, $(SRCS))

# ---------------- LIB --------------- #

LIBFT_DIR 	= ./libft/
LIBFT 		= $(LIBFT_DIR)libft.a
LIB_LINK	= -L$(LIBFT_DIR) -lft


all: $(NAME)

fast:
	$(MAKE) re -j8

$(NAME):	$(OBJS)
			$(CC) $(CFLAGS) $(EXTRAFLAGS) $(SANITIZE) $(OBJS) $(ALL_INC) -o $@
			echo "$(GREEN)==> $@ linked$(END)"

$(OBJS): 	$(DIR_OBJS)%.o: %.cpp | $(DIR_OBJS)
			$(CC) $(CFLAGS) $(EXTRAFLAGS) $(SANITIZE) $(ALL_INC) -c $< -o $@
			echo "$(YELLOW)--> $@ made$(END)"

$(DIR_OBJS):
	mkdir -p $@

$(LIBFT):
	echo "$(YELLOW)--> Creating $@...$(END)"
	$(MAKE) -C $(LIBFT_DIR)
	echo "$(GREEN)==> $@ linked$(END)"

clean:
# 	$(MAKE) clean -C $(LIBFT_DIR)
	$(RM) -R $(DIR_OBJS)
# 	echo "$(RED)-> $@ $(NAME) made$(END)"

fclean: clean
# 	$(MAKE) fclean -C $(LIBFT_DIR)
	$(RM) $(NAME)
# 	echo "$(RED)-> $@ $(NAME) made$(END)"

re: fclean
	$(MAKE)

.PHONY:		all fast clean fclean re
.SILENT:	$(NAME) $(OBJS) $(DIR_OBJS) $(LIBFT) # clean fclean
