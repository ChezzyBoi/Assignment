.RECIPEPREFIX +=
CC = g++
CFLAGS = -Wall -lm -lrt -lwiringPi -lpthread

PROG = bin/*
OBJS = obj/*

default:
    mkdir -p bin obj
    $(CC) $(CFLAGS) -c src/Project.cpp -o obj/Project
    $(CC) $(CFLAGS) -c src/CurrentTime.c -o obj/CurrentTime
    $(CC) $(CFLAGS) obj/Project obj/CurrentTime -o bin/Project

run:
    sudo ./bin/Project

clean:
rm $(PROG) $(OBJS)