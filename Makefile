# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: keishii <keishii@student.42tokyo.jp>       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/05/31 18:25:47 by miyuu             #+#    #+#              #
#    Updated: 2025/09/12 21:00:25 by keishii          ###   ########.fr        #
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
					Parser.cpp \
					Channel.cpp \
					Client.cpp \
					Database.cpp \
					Command/Command.cpp \
					Command/NickCommand.cpp \
					Command/PassCommand.cpp \
					Command/UserCommand.cpp \
					Command/PingCommand.cpp \
					Command/PongCommand.cpp \
					PrintLog.cpp \
					Command/PrivmsgCommand.cpp

HEADERS				:= \
					irc.hpp \
					Server.hpp \
					Parser.hpp \
					Channel.hpp \
					Client.hpp \
					Database.hpp \
					Command/Command.hpp \
					Command/NickCommand.hpp \
					Command/PassCommand.hpp \
					Command/UserCommand.hpp \
					Command/PingCommand.hpp \
					Command/PongCommand.hpp \
					PrintLog.hpp \
					Command/PrivmsgCommand.hpp

OBJ					:= \
					$(addprefix $(OBJ_DIR)/, \
					$(SRC:.cpp=.o))
# $(warning $(OBJ))

# **************************************************************************** #
# LIBRARIES & FRAMEWORKS




# **************************************************************************** #
# INCLUDES


INC_DIR			:= includes includes/Command
INCLUDES		:= $(addprefix -I,$(INC_DIR))
# $(warning $(INCLUDES))


# **************************************************************************** #
# RULES


all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXX_FLAGS) $(OBJ) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp #$(addprefix $(INC_DIR)/, $(HEADERS))
	@mkdir -p $(dir $@)
	$(CXX) $(CXX_FLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) -r $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

# test
test:
	./test/unit_test.sh

# **************************************************************************** #
# PHONY


.PHONY: all clean fclean re test
