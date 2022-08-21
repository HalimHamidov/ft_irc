NAME = ircserv
CXX = c++
FLAGS = -Wall -Wextra -Werror -std=c++98
RM = rm -f
SRCS = srcs/main.cpp \
		 srcs/bot.cpp \
		 srcs/channel.cpp \
		 srcs/command.cpp \
		 srcs/sending_messages.cpp \
		 srcs/server.cpp \
		 srcs/user.cpp 

OBJS = $(SRCS:.cpp=.o)

%.o: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

${NAME}: $(OBJS)
		$(CXX) $(FLAGS) -o $(NAME) $(OBJS)

all : ${NAME}

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) ${NAME}

re: fclean all

.PHONY:	all clean re fclean re
