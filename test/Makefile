# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: keishii <keishii@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/05/31 18:25:47 by miyuu             #+#    #+#              #
#    Updated: 2025/09/27 16:07:47 by keishii          ###   ########.fr        #
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

MAIN				?=

SRC					:= \
					Server.cpp \
					Parser.cpp \
					Channel.cpp \
					Client.cpp \
					Database.cpp \
					Command/Command.cpp \
					Command/CapCommand.cpp \
					Command/NickCommand.cpp \
					Command/PassCommand.cpp \
					Command/UserCommand.cpp \
					Command/PrivmsgCommand.cpp \
					Command/JoinCommand.cpp \
					Command/InviteCommand.cpp \
					Command/PingCommand.cpp \
					Command/PongCommand.cpp \
					Command/TopicCommand.cpp \
					Command/ModeCommand.cpp \
					Command/QuitCommand.cpp \
					Utils.cpp \
					PrintLog.cpp \

HEADERS				:= \
					irc.hpp \
					Server.hpp \
					Parser.hpp \
					Channel.hpp \
					Client.hpp \
					Database.hpp \
					Command.hpp \
					CapCommand.hpp \
					NickCommand.hpp \
					PassCommand.hpp \
					UserCommand.hpp \
					PrivmsgCommand.hpp \
					JoinCommand.hpp \
					InviteCommand.hpp \
					PingCommand.hpp \
					PongCommand.hpp \
					TopicCommand.hpp \
					ModeCommand.hpp \
					QuitCommand.hpp \
					Utils.hpp \
					PrintLog.hpp \


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


all: check_main $(TEST_NAME)

check_main:
ifndef MAIN
	@echo "--------------------------------------------------------------------------"
	@echo "Error: main関数が指定されていません。"
	@echo "make re MAIN=ファイル名 でテストしたいファイルを指定してください。"
	@echo "--------------------------------------------------------------------------"
	@exit 1
endif

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


.PHONY: all clean fclean re check_main
