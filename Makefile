CC=gcc
CFLAGS=-Wall -ggdb

all: shootme shootmed

shootme: conf.o net.o sdl.o enemy.o
	$(CC) $(CFLAGS) shootme.c conf.o net.o sdl.o enemy.o -o shootme -lSDL -lSDL_ttf -lSDL_mixer
shootmed: conf.o net.o
	 $(CC) $(CFLAGS) shootmed.c conf.o net.o -o shootmed -lpthread

clean:
	rm -f shootme shootmed *.o
