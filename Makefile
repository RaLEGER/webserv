# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rleger <rleger@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/04/19 14:22:30 by rleger            #+#    #+#              #
#    Updated: 2024/04/16 11:36:20 by rleger           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = War
BUILD_DIR = build

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
RM = rm -rf

# Directories
INC_DIRS = Parser Location Server RequestHandler Response CustomError CGIHandler
INC_FLAGS = $(addprefix -I,$(INC_DIRS))

# Headers and Source files
HEAD = $(wildcard $(addsuffix /*.hpp,$(INC_DIRS)))
SRCS = $(wildcard $(addsuffix /*.cpp,$(INC_DIRS))) main.cpp

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