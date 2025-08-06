# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: miyuu <miyuu@student.42.fr>                +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/05/31 18:25:47 by miyuu             #+#    #+#              #
#    Updated: 2025/08/06 18:23:31 by miyuu            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# **************************************************************************** #
# VARIABLES


TEST_NAME			:= test_irc

CXX					:= c++
CXX_FLAGS			:= -Wall -Wextra -Werror -std=c++98 -pedantic

SRC_DIR				:= ../src
OBJ_DIR				:= obj
TEST_DIR			:= .

# **************************************************************************** #
# SOURCES

MAIN				?= main.cpp

SRC					:= \
					Server.cpp \
					Command.cpp \
					NickCommand.cpp \
					PassCommand.cpp \
					UserCommand.cpp \

HEADERS				:= \
					irc.hpp \
					Server.hpp \
					Channel.hpp \
					Client.hpp \
					Database.hpp \
					Command.hpp \
					NickCommand.hpp \
					PassCommand.hpp \
					UserCommand.hpp


OBJ					:= $(addprefix $(OBJ_DIR)/src/, $(SRC:.cpp=.o)) \
					$(addprefix $(OBJ_DIR)/test/, $(MAIN:.cpp=.o))


# **************************************************************************** #
# LIBRARIES & FRAMEWORKS




# **************************************************************************** #
# INCLUDES


INC_DIR			:= ../includes
INCLUDES		:= -I$(INC_DIR)


# **************************************************************************** #
# RULES


all: $(TEST_NAME)

$(TEST_NAME): $(OBJ)
	$(CXX) $(CXX_FLAGS) $(OBJ) -o $@

$(OBJ_DIR)/src/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXX_FLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/test/%.o: $(TEST_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXX_FLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) -r $(OBJ_DIR)

fclean: clean
	$(RM) $(TEST_NAME)

re: fclean all


# **************************************************************************** #
# PHONY


.PHONY: all clean fclean re
