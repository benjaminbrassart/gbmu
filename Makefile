# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/10/07 11:07:29 by bbrassar          #+#    #+#              #
#    Updated: 2023/10/15 14:46:10 by bbrassar         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CURRENT_MAKEFILE := $(lastword $(MAKEFILE_LIST))

NAME := gbmu

CXX := clang++
CXXFLAGS := -Wall -Werror -Wextra -Wshadow -Wsuggest-override
CXXFLAGS += -c
CXXFLAGS += -g3
CXXFLAGS += -MMD -MP
CXXFLAGS += -Iinclude
CXXFLAGS += -std=c++20

LDLIBS :=
LDFLAGS :=

RM := rm -vf
MKDIR := mkdir -vp

DIR_SRC := src
DIR_OBJ := obj

SRC := main.cpp
SRC += mmu.cpp
SRC += format.cpp
SRC += exception.cpp
SRC += cpu.cpp
SRC += cartridge.cpp
SRC += io_registers.cpp
SRC += debugger.cpp
OBJ := $(SRC:%=$(DIR_OBJ)/%.o)
DEP := $(OBJ:.o=.d)

$(NAME): $(OBJ)
	$(CXX) $^ -o $@ $(LDLIBS) $(LDFLAGS)

$(OBJ): $(DIR_OBJ)/%.cpp.o: $(DIR_SRC)/%.cpp
	@$(MKDIR) $(@D)
	$(CXX) $(CXXFLAGS) $< -o $@

-include $(DEP)

.PHONY: all clean fclean re

all: $(NAME)

clean:
	@$(RM) $(OBJ) $(DEP)

fclean: clean
	@$(RM) $(NAME)

re:
	$(MAKE) -f $(CURRENT_MAKEFILE) fclean
	$(MAKE) -f $(CURRENT_MAKEFILE) all
