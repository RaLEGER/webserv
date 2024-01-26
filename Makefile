# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rleger <rleger@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/04/19 14:22:30 by rleger            #+#    #+#              #
#    Updated: 2024/01/26 18:50:38 by rleger           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = War

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
RM = rm -f

HEAD = Parser/Parser.hpp 

SRCS = Parser/Parser.cpp \
		main.cpp

OBJS = ${SRCS:.cpp=.o}

.c.o:
	$(CXX) $(CXXFLAGS) -I ${HEAD} -c -o $@ $<

$(NAME): $(OBJS) $(HEAD)
	$(CXX) $(CXXFLAGS) $(OBJS) -I ${HEAD} -o $(NAME)

all: $(NAME)

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: clean all

.PHONY: all clean fclean re