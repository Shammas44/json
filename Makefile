# TIPS 
# ===========================
#
# enable core
# 	ulimit -c unlimited
#
# disable core
# 	ulimit -c 0
#
# debug thread with following compiler flag:
# 	-fsanitize=thread
#
# ===========================

# Library name
# ---------------------------
LIB_NAME = json

# FLAGS
# ---------------------------
CC = gcc
FLAGS = -Wall -Wextra -Werror -g

# to set the option: export OPTION=<prod|dev|test>; make <target>
ifeq ($(OPTION), prod)
	FLAGS = -Wall -Wextra -Werror
endif
ifeq ($(OPTION), dev)
	FLAGS = -Wall -Wextra -Werror -g
endif
ifeq ($(OPTION), test)
	FLAGS = -Wno-builtin-declaration-mismatch -Wno-implicit-function-declaration
endif

# Directories
# ---------------------------
SRC_DIR = src
BIN_DIR = bin

# Library sources
# ---------------------------
LIB_SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
LIB_OBJS = $(LIB_SRC_FILES:$(SRC_DIR)/%.c=$(SRC_DIR)/%.o)

# Main application source files
# ---------------------------
MAIN_SRC_FILES = main.c
MAIN_OBJS = $(MAIN_SRC_FILES:%.c=$(BIN_DIR)/%.o)

# Link against the library
# ---------------------------

# Add the include directory of the librairies to the include path
INC_LIB_DIR:=$(shell find -L lib -type d -name "include")
INC_STATIC_LIB:=$(foreach dir,$(INC_LIB_DIR),-I $(dir))

# Add the bin directory of the librairies to the include path
LIB_BIN_DIR:=$(shell find -L lib -type d -name "bin")
INC_STATIC_BIN:=$(foreach dir,$(LIB_BIN_DIR),-L $(dir))

# Add the librairies to the include path
LIBS_BASENAME:= $(shell find -L lib -mindepth 1 -maxdepth 1 -type d -exec basename {} \; 2>/dev/null)
INC_LIBS:=$(foreach dir,$(LIBS_BASENAME),-l $(dir))

EXTRA_LIBS = -l ssl -l crypto
LIBS = $(INC_STATIC_BIN) $(INC_LIBS) $(EXTRA_LIBS) 
LINK = -L $(BIN_DIR) -L /usr/local/lib -l $(LIB_NAME)
INC = -I src/include -I /usr/local/include $(INC_STATIC_LIB) 
LIB_TEST=-l criterion

SOURCES:=$(shell find ./src -type f -name "*.c" )
TEST_FILES = $(wildcard tests/*.c)
SOURCES_TEST=$(SOURCES) $(wildcard tests/*.c)
OBJS=$(SOURCES:%.c=%.o)
OBJS_TEST=$(OBJS) $(SOURCES_TEST:%.c=%.o)

# Phony targets
# ---------------------------
.PHONY: all library clean_o clean_bin clean bear main exec run_test

# Targets
# ---------------------------

#default target
all: library

truc:
	echo $(FLAGS)
# Generate compile_commands.json file for clangd
bear:
	make clean; bear -- make all

# Lib shortcut target
library: lib$(LIB_NAME).a

# Rule to build library
lib$(LIB_NAME).a: $(LIB_OBJS)
	ar rcs $(BIN_DIR)/$@ $^ 

# Rule to build library object files
%.o: %.c
	$(CC) $(FLAGS) $(INC) -c $< -o $@ 

test: $(TEST_FILES:.c=.o)
	$(CC) $(FLAGS) $^ -o $(BIN_DIR)/$@ $(LINK) $(INC_STATIC_BIN) $(INC_LIBS) $(LIB_TEST)  

# Rule to build main application
main: library main.o
	$(CC) $(FLAGS) $@.c -o $(BIN_DIR)/$@ $(INC) $(LINK) $(LIBS)

# Rule to build main application object files
main.o: main.c
	$(CC) $(FLAGS) $(INC) -c $< -o $@

# Rule to execute the main application
exec: main
	./bin/main

run_test: test
	./$(BIN_DIR)/test

# Remove every object files and binary files
clean: clean_o clean_bin

# Remove every object file in the project
clean_o: 
	@echo "cleaning..."
	find . -name "*.o" -delete

# Remove every exectuable file in the root directory
clean_bin:
	rm  -rf ./$(BIN_DIR)/*;

