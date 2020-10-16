#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "macros.h"

typedef enum State{START, FLAG_RCV, A_RCV, C_RCV, BCC_OK, STOP} State;

int sendSetFrame(int fd);

int receiveSetFrame(int fd);

int sendUAFrame(int fd);

int receiveUAFrame(int fd);

#endif /*PROTOCOL_H*/
