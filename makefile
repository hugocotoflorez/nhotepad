CC = gcc
FLAGS = -Wall
NAME = nhotepad
SRC = main.c ui.c input.c file.c buffer.c
LIB = -L. -ltermstuff

all:
	$(CC) $(FLAGS) -o $(NAME) $(SRC) $(LIB)

debug:
	$(CC) $(FLAGS) -o $(NAME) $(SRC) $(LIB) -g

git:
	git add $(SRC) README.md makefile libtermstuff.a nh.h
