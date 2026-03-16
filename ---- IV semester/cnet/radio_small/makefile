.PHONY: all clean

all: sikradio-sender sikradio-receiver

sikradio-sender: sikradio-sender.c err.h common.h
		gcc -Wall -O2 -o $@ $^

sikradio-receiver:	sikradio-receiver.c err.h common.h
		gcc -Wall -O2 -o $@ $^ -lpthread

clean:
	rm -f sikradio-sender sikradio-receiver *.o