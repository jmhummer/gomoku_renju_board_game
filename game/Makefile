CC = gcc
CFLAGS = -Wall -std=c99 -g

all: gomoku renju replay
.PHONY: all

gomoku: gomoku.o io.o board.o game.o

gomoku.o: gomoku.c game.h board.h io.h

renju: renju.o io.o board.o game.o

renju.o: renju.c game.h board.h io.h

replay: replay.o io.o board.o game.o

replay.o: replay.c game.h board.h io.h

board.o: board.c board.h

game.o: game.c game.h

io.o: io.c io.h

.PHONY: clean
clean: rm *.o temp
