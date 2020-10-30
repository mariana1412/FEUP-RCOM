#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include "macros.h"
#include "dataStructures.h"
#include "stateMachines.h"

extern int alarmSender;

void alarmHandler();

int SandWOpenClose(int fd, ControlCommand send, ControlCommand receive);

int sendOpenCloseFrame(int fd, ControlCommand command, int address);

int receiveOpenCloseFrame(int fd, ControlCommand command, int address);

int sendInfoFrame(int fd, int ns, char* info);

int receiveInfoFrame(int fd, int ns, char* info);

char* makeControlPacket(int control, int fileSize, char* fileName);

#endif /*PROTOCOL_H*/
