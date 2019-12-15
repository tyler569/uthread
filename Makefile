
CFLAGS=-Wall -g

all: uthread

libuthread.a: uthread.o
	ar rcs libuthread.a uthread.o

uthread: example.o libuthread.a
	$(CC) $(CFLAGS) -o uthread example.o libuthread.a
