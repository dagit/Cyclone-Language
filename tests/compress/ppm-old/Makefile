CC = gcc
#CFLAGS = -g -Wall -ansi
CLFAGS = -O2 -Wall -ansi

all:	encode decode

encode: encode.c model.o arithcode.o
	$(CC) $(CFLAGS) -o encode encode.c model.o arithcode.o
decode: decode.c model.o arithcode.o
	$(CC) $(CFLAGS) -o decode decode.c model.o arithcode.o

clean: 
	rm *.o encode decode
