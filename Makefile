# Folders and Names
NAME		=	webserv

SRCDIR		=	.

SRCS		=	webserver/main.cpp \
				webserver/server/srcs/WebServer.cpp \
				webserver/server/srcs/SendData.cpp \
				webserver/server/srcs/HandleData.cpp \
				webserver/server/srcs/server_utils.cpp \
				webserver/server/srcs/HandleCgi.cpp \
				webserver/server/srcs/Response.cpp \
				webserver/server/srcs/parse.cpp \
				webserver/server/srcs/config_utils.cpp \
				webserver/server/srcs/Config.cpp \
				webserver/server/srcs/Location.cpp \

OBJSDIR		=	objs

OBJS		=	$(SRCS:%.cpp=$(OBJSDIR)/%.o)

# Compiler options
CC			=	g++
CFLAGS		=	-g -Wall -Wextra -Werror -std=c++98

all: ${NAME}

${NAME}: ${OBJS}
	@echo "Compiling $@"
	@${CC} ${CFLAGS} -o ${NAME} ${OBJS}

${OBJS}: ${OBJSDIR}/%.o: ${SRCDIR}/%.cpp
	@mkdir -p ${@D}
	${CC} ${CFLAGS} -c $< -o $@

clean:
	rm -rf ./objs
	rm -rf form.txt

fclean: clean
	rm -rf ${NAME}

re: fclean all

.PHONY: all clean fclean re
