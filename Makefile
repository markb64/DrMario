.PHONY: clean

CFLAGS=-std=gnu89 -pedantic -Wall -g
LDFLAGS=-lSDL2 -lm

all: drmario

game.o: game.c game.h
	gcc $(CFLAGS) -c -o $@ $<

drmario.o: drmario.c drmario.h
	gcc $(CFLAGS) -c -o $@ $<

drmario_main.o: drmario_main.c drmario.h game.h
	gcc $(CFLAGS) -c -o $@ $<

drmario: drmario_main.o drmario.o game.o
	gcc -o $@ $^ $(LDFLAGS)

clean:
	rm drmario
	rm *.o
