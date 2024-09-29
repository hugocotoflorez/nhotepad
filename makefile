CC = gcc
FLAGS = -Wall
NAME = nhotepad
SRC = main.c ui.c input.c file.c buffer.c
LIB = -L. -ltermstuff

all:
	$(CC) $(FLAGS) -o $(NAME) $(SRC) $(LIB)

debug:
	$(CC) $(FLAGS) -o $(NAME) $(SRC) $(LIB) -g

add:
	git add $(SRC) README.md makefile libtermstuff.a nh.h

install: all
	install $(NAME) /usr/local/bin/

uninstall:
	rm /usr/local/bin/$(NAME)
