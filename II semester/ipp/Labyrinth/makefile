CC = gcc
CFLAGS = -Wall -Wextra -Wno-implicit-fallthrough -std=c17 -O2
LDFLAGS =

.PHONY: all clean

all: labyrinth

labyrinth: main.o iolib.o prolib.o lablib.o strct.o
	$(CC) $(LDFLAGS) -o $@ $^

iolib.o: iolib.c iolib.h
	$(CC) $(CFLAGS) -c $<

prolib.o: prolib.c prolib.h
	$(CC) $(CFLAGS) -c $<

lablib.o: lablib.c lablib.h
	$(CC) $(CFLAGS) -c $<

strct.o: strct.c strct.h
	$(CC) $(CFLAGS) -c $<

main.o: main.c iolib.h  prolib.h lablib.h strct.h
	$(CC) $(CFLAGS) -c $<

clean:
	-rm -fr iolib.o
	-rm -fr prolib.o
	-rm -fr lablib.o
	-rm -fr strct.o
	-rm -fr main.o
	-rm -fr labyrinth
