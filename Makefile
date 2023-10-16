# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/10/07 11:07:29 by bbrassar          #+#    #+#              #
#    Updated: 2023/10/16 13:20:08 by bbrassar         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CURRENT_MAKEFILE := $(lastword $(MAKEFILE_LIST))

# FEATURES := DISABLE_BOOT_ROM

NAME := gbmu

DIR_GLFW3 := glfw
NAME_GLFW3 := $(DIR_GLFW3)/build/src/libglfw3.a

CXX := clang++
CXXFLAGS := -Wall -Werror -Wextra -Wshadow -Wsuggest-override
CXXFLAGS += -c
CXXFLAGS += -g3
CXXFLAGS += -MMD -MP
CXXFLAGS += -Iinclude
CXXFLAGS += -I$(DIR_GLFW3)/include
CXXFLAGS += -std=c++20
CXXFLAGS += $(FEATURES:%=-DGBMU_%=1)
CXXFLAGS += $(shell pkg-config --cflags gl)

LDLIBS := $(shell pkg-config --libs-only-l gl)
LDLIBS += -lglfw3
LDLIBS += -ldl
LDLIBS += -lpthread
LDFLAGS := $(shell pkg-config --libs-only-L gl)
LDFLAGS += -Lglfw/build/src

RM := rm -vf
RMDIR := $(RM) -r
MKDIR := mkdir -vp

DIR_SRC := src
DIR_OBJ := obj

SRC := main.cpp
SRC += mmu.cpp
SRC += ppu.cpp
SRC += lcd.cpp
SRC += format.cpp
SRC += exception.cpp
SRC += renderer.cpp
SRC += cpu.cpp
SRC += cartridge.cpp
SRC += io_registers.cpp
SRC += debugger.cpp
OBJ := $(SRC:%=$(DIR_OBJ)/%.o)
DEP := $(OBJ:.o=.d)

$(NAME): $(OBJ) $(NAME_GLFW3)
	$(CXX) $(filter %.o,$^) -o $@ $(LDLIBS) $(LDFLAGS)

$(OBJ): $(DIR_OBJ)/%.cpp.o: $(DIR_SRC)/%.cpp $(CURRENT_MAKEFILE)
	@$(MKDIR) $(@D)
	$(CXX) $(CXXFLAGS) $< -o $@

-include $(DEP)

$(DIR_GLFW3):
	git submodule update --init $@

$(NAME_GLFW3): $(DIR_GLFW3)
	cmake -S $(DIR_GLFW3) -B $(DIR_GLFW3)/build
	cmake --build $(DIR_GLFW3)/build

.PHONY: all clean fclean re

all: $(NAME)

clean:
	@$(RM) $(OBJ) $(DEP)

fclean: clean
	@$(RM) $(NAME)
	@$(RMDIR) $(DIR_GLFW3)/build

re:
	$(MAKE) -f $(CURRENT_MAKEFILE) fclean
	$(MAKE) -f $(CURRENT_MAKEFILE) all
