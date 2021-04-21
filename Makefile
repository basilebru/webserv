	
NAME = webserv

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
CONF_DIR 		=	./Config_inheritance/
UTILS_DIR 		=	./Utils/

ALL_INC			+= 	-I$(INCLUDES)
ALL_INC			+= 	-I$(SERVER_DIR)
ALL_INC			+= 	-I$(REQUEST_DIR)
ALL_INC			+= 	-I$(CONF_DIR)
ALL_INC			+= 	-I$(UTILS_DIR)


# ---------------- SRC --------------- #
# Add files to compile here

# main sources
SRCS += webserv.cpp

# core sources
SRCS += Server.cpp
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


all: $(NAME)

fast:
	$(MAKE) re -j8

$(NAME):	$(OBJS)
			$(CC) $(CFLAGS) $(EXTRAFLAGS) $(SANITIZE) $(OBJS) $(ALL_INC) -o $@
# 			echo "$(_BOLD)$(_YELLOW)==> $@ linked$(_END)"

$(OBJS): 	$(DIR_OBJS)%.o: %.cpp | $(DIR_OBJS)
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
