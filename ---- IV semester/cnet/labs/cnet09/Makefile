.PHONY: all clean

all: rawping spoofping myping

err.o: err.c
	gcc -c -Wall -O2 -o $@ $<

in_cksum.o: in_cksum.c
	gcc -c -Wall -O2 -o $@ $<

dropnobody.o: dropnobody.c
	gcc -c -Wall -O2 -o $@ $<

rawping.o: rawping.c
	gcc -c -Wall -O2 -o $@ $<

spoofping.o: spoofping.c
	gcc -c -Wall -O2 -o $@ $<

myping.o: myping.c
	gcc -c -Wall -O2 -o $@ $<

rawping: rawping.o err.o dropnobody.o in_cksum.o
	gcc -o $@ $^

spoofping: spoofping.o err.o dropnobody.o in_cksum.o
	gcc -o $@ $^

myping: myping.o err.o dropnobody.o in_cksum.o
	gcc -o $@ $^

clean:
	rm -rf myping rawping spoofping *.o
