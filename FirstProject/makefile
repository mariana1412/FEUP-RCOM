CC = gcc
CFLAGS = -Wall -Wextra -g

SENDER_OBJ = sender
SENDER_SRC = writenoncanonical.c

RECEIVER_OBJ = receiver
RECEIVER_SRC = noncanonical.c

AUX_OBJ = protocol
AUX_SRC = protocol.c


all: sender receiver

%.o: %.c 
	$(CC) $(CFLAGS) -c -o $@ $<

sender:
	$(CC) $(CFLAGS) $(AUX_SRC) $(SENDER_SRC) -o $(SENDER_OBJ)

receiver:
	$(CC) $(CFLAGS) $(AUX_SRC) $(RECEIVER_SRC) -o $(RECEIVER_OBJ)

clean:
	rm $(SENDER_OBJ) $(RECEIVER_OBJ) $(AUX_OBJ)