#ifndef PROTOCOL_H
#define PROTOCOL_H


#include "macros.h"
#include <signal.h>
#include <stdio.h>
#include <string.h>

typedef enum State{START, FLAG_RCV, A_RCV, C_RCV, BCC_OK, DATA, C2_RCV, BCC2_OK, STOP} State;
typedef enum ControlCommand{SET, DISC, UA, RR, REJ} ControlCommand;

extern int alarmSender;

void alarmHandler();

int SandWOpenClose(int fd, ControlCommand send, ControlCommand receive);

int changeStateS(State *state, unsigned char byte, ControlCommand command, unsigned char address);

int changeStateInfo(State *state, int ns, unsigned char byte);

int sendOpenCloseFrame(int fd, ControlCommand command, int address);

int receiveOpenCloseFrame(int fd, ControlCommand command, int address);

int sendRRFrame(int fd, int nr);

int receiveRRFrame(int fd, int *nr);

int sendInfoFrame(int fd, int ns, char* info);

int receiveInfoFrame (int fd, int ns, char* info);

char* makeControlPacket (int control, int fileSize, char* fileName);

#endif /*PROTOCOL_H*/
