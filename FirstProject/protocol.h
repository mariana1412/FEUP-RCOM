#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "macros.h"
#include <signal.h>
#include <stdio.h>

typedef enum State{START, FLAG_RCV, A_RCV, C_RCV, BCC_OK, DATA, C2_RCV, BCC2_OK, STOP} State;

int changeStateSETUA(State *state, unsigned char byte, char command);

int changeStateInfo(State *state, int ns, unsigned char byte);

int sendSetFrame(int fd);

int receiveSetFrame(int fd);

int sendUAFrame(int fd);

int receiveUAFrame(int fd);

int sendInfoFrame(int fd, int ns, char* info);

int receiveInfoFrame (int fd, int ns, char* info);

#endif /*PROTOCOL_H*/
