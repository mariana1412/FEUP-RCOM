#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include "macros.h"
#include "dataStructures.h"
#include "stateMachines.h"

extern int alarmSender; //testar sem isto

void alarmHandler();

int SandWOpenClose(int fd, ControlCommand send, ControlCommand receive);

int sendOpenCloseFrame(int fd, ControlCommand command, int address);

int sendAckFrame(int fd, ControlCommand command, int r);

int receiveOpenCloseFrame(int fd, ControlCommand command, int address);

int sendInfoFrame(int fd, int ns, char* info, int length);

int receiveInfoFrame(int fd, char* info);

unsigned char* makeControlPacket(unsigned char control, int fileSize, char* fileName);

unsigned char* makeDataPacket(char *info, int N);

#endif /*PROTOCOL_H*/
