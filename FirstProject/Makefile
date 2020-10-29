AUX = protocol.c dataLink.c port.c

SENDER = writenoncanonical.c

RECEIVER = noncanonical.c

CC = gcc

all: sender receiver

sender:
	$(CC) -o sender $(SENDER) $(AUX)

receiver:
	$(CC) -o receiver $(RECEIVER) $(AUX)

clean:
	rm -f sender receiver