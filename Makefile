# tool macros
CC := gcc
CCFLAG :=  -I./src/includes -I/usr/include/SDL2 -std=c99 -g
LDFLAG := -lpthread -Llib -lSDL2 -lSDL2main -lSDL2_ttf -lSDL2_mixer -lSDL2_image -I./src/includes -g
DBGFLAG := -g
CCOBJFLAG := $(CCFLAG) -c

# path macros
BIN_PATH := bin
OBJ_PATH := obj
SRC_PATH := src

# compile macros
TARGET_NAME := panda
TARGET := $(BIN_PATH)/$(TARGET_NAME)
MAIN_SRC := main.c

# src files & obj filesjj
SRC_FILES := $(wildcard $(SRC_PATH)/*.c)
OBJ_FILES := $(patsubst $(SRC_PATH)/%.c,$(OBJ_PATH)/%.o,$(SRC_FILES))

# default rule
default: main

main: $(OBJ_FILES)
	gcc $(LDFLAG) -o $@ $^


$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c
	gcc $(CCFLAG) -c -o $@ $<

