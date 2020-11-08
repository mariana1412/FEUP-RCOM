AUX = protocol.c dataLink.c port.c stateMachines.c

SENDER = appSender.c

RECEIVER = appReceiver.c

CC = gcc

all: sender receiver

sender:
	$(CC) -g -std=gnu99 -fsanitize=address -o sender $(SENDER) $(AUX)

receiver:
	$(CC) -g -std=gnu99 -fsanitize=address -o receiver $(RECEIVER) $(AUX)

clean:
	rm -f sender receiver ./imagesToReceive/*