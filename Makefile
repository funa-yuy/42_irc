# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: miyuu <miyuu@student.42.fr>                +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/05/31 18:25:47 by miyuu             #+#    #+#              #
#    Updated: 2025/08/05 13:28:37 by miyuu            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# **************************************************************************** #
# VARIABLES


NAME				:= ircserv

CXX					:= c++
CXX_FLAGS			:= -Wall -Wextra -Werror -std=c++98 -pedantic

SRC_DIR				:= src
OBJ_DIR				:= obj


# **************************************************************************** #
# SOURCES

SRC					:= \
					main.cpp \
					Server.cpp \
					Command.cpp \
					NickCommand.cpp \
					PassCommand.cpp \
					UserCommand.cpp

HEADERS				:= \
					Server.hpp \
					Channel.hpp \
					Client.hpp \
					Database.hpp \
					Command.hpp \
					NickCommand.hpp \
					PassCommand.hpp \
					UserCommand.hpp

OBJ					:= \
					$(addprefix $(OBJ_DIR)/, $(SRC:.cpp=.o))


# **************************************************************************** #
# LIBRARIES & FRAMEWORKS




# **************************************************************************** #
# INCLUDES


INC_DIR			:= includes
INCLUDES		:= -I$(INC_DIR)


# **************************************************************************** #
# RULES


all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXX_FLAGS) $(OBJ) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(addprefix $(INC_DIR)/, $(HEADERS))
	@mkdir -p $(dir $@)
	$(CXX) $(CXX_FLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) -r $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all


# **************************************************************************** #
# PHONY


.PHONY: all clean fclean re
