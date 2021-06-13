TARGET = daemon.out
FLAGS = -Wall -Wextra -Wpedantic -std=c11
SRC_DIR = src
INC_DIR = include

all: 
	gcc main.c $(SRC_DIR)/*.c -lglib-2.0 -I$(INC_DIR)

.PHONY: clear
clear:
	-rm *.o