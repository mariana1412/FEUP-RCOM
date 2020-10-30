AUX = protocol.c dataLink.c port.c stateMachines.c

SENDER = writenoncanonical.c

RECEIVER = noncanonical.c

CC = gcc

all: sender receiver

sender:
	$(CC) -g -std=gnu99 -o sender $(SENDER) $(AUX)

receiver:
	$(CC) -g -std=gnu99 -o receiver $(RECEIVER) $(AUX)

clean:
	rm -f sender receiver