# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rleger <rleger@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/04/19 14:22:30 by rleger            #+#    #+#              #
#    Updated: 2024/04/23 12:22:57 by rleger           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = 		webserv
BUILD_DIR = build
SRC_DIR =	src

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -g3
RM = rm -rf

# Directories
INC_DIRS = ServerFarm Parser Location Server RequestHandler Response CGIHandler Utils Request Router Socket
INC_FLAGS = $(addprefix -I$(SRC_DIR)/,$(INC_DIRS))

# Headers and Source files

SRCS := $(foreach dir,$(INC_DIRS),$(SRC_DIR)/$(dir)/$(dir).cpp)
SRCS += main.cpp

HEAD := $(foreach dir,$(INC_DIRS),$(SRC_DIR)/$(dir)/$(dir).hpp)

OBJS = $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(SRCS))

# Build rule
$(BUILD_DIR)/%.o: %.cpp $(HEAD)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INC_FLAGS) -c -o $@ $<

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INC_FLAGS) $(OBJS) -o $(NAME)

all: $(NAME)

clean:
	$(RM) $(BUILD_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
