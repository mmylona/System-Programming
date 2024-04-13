CC = gcc
CFLAGS = -pthread

all: poller pollSwayer

poller: poller.c
	$(CC) $(CFLAGS) -o poller poller.c

pollSwayer: pollSwayer.c
	$(CC) $(CFLAGS) -o pollSwayer pollSwayer.c

clean:
	rm -f poller pollSwayer
