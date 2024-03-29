# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: teliet <teliet@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/09/29 20:41:39 by teliet            #+#    #+#              #
#    Updated: 2023/06/02 16:17:36 by teliet           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = webserv 

SRC = main.cpp Request/Request.cpp RequestHandler/RequestHandler.cpp Response/Response.cpp CGIHandler/CGIHandler.cpp

OBJ = $(SRC:.cpp=.o)

CC = c++

HEADERS = -I ./

FLAGS = -Wall -Wextra -Werror -std=c++98 -g3


all: ${NAME}
 
$(NAME): $(OBJ) 
	$(CC) $(FLAGS) $(OBJ) -o $(NAME)
	
debug: $(LIBS) clean
	$c(CC) $(SRC) $(HEADERS) -o $(NAME) 

%.o: %.cpp
	$(CC) $(HEADERS)  $(FLAGS) -c $< -o $@

clean:
	/bin/rm -f ${OBJ}

fclean: clean
	/bin/rm -f ${NAME}

re: fclean all