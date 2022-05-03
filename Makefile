CC = gcc
DEFS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L
CFLAGS = -Wall -g -std=c99 -pedantic $(DEFS)

OBJECTS = cpair.o io.o

.PHONY: all clean
all: cpair


cpair: $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^ -lm

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<


cpair.o: cpair.c cpair.h io.h
io.o: io.c cpair.h 
clean:
	rm -rf *.o cpair objects
