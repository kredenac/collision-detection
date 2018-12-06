CC=g++
CFLAGS=-c -lGL -lGLU -lglut -lm -Wall
LDLIBS=-lglut -lGLU -lGL -lm -Wall

main: main.o player.o keyboard.o collision.o draw.o miscfunc.o blocks.o
	$(CC) main.o player.o keyboard.o miscfunc.o draw.o collision.o blocks.o -o main $(LDLIBS)

main.o: src/main.c src/miscfunc.h
	$(CC) $(CFLAGS) src/main.c

player.o: src/player.c src/miscfunc.h
	$(CC) $(CFLAGS) src/player.c

keyboard.o: src/keyboard.c
	$(CC) $(CFLAGS) src/keyboard.c

collision.o: src/collision.c
	$(CC) $(CFLAGS) src/collision.c

draw.o: src/draw.c
	$(CC) $(CFLAGS) src/draw.c

miscfunc.o: src/miscfunc.c src/miscfunc.h
	$(CC) $(CFLAGS) src/miscfunc.c

blocks.o: src/blocks.c
	$(CC) $(CFLAGS) src/blocks.c
clean:
	rm -rf main
	rm -rf *.o
	rm -rf *~
