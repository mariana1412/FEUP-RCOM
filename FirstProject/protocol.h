#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include "macros.h"
#include "dataStructures.h"
#include "stateMachines.h"

extern int alarmSender; //testar sem isto

/**
 * Catches a specific signal and the value of alarmSender becomes 0 
*/
void alarmHandler();

/**
 * Sends a message and waits for response, using a Stop and Wait mechanism
 * @param fd file descriptor of port
 * @param send command to be sent
 * @param receive command to be received
 * 
 * @return 0 on success; -1 on error
 */
int SandWOpenClose(int fd, ControlCommand send, ControlCommand receive);

int sendOpenCloseFrame(int fd, ControlCommand command, int address);

int sendAckFrame(int fd, ControlCommand command, int r);

int receiveOpenCloseFrame(int fd, ControlCommand command, int address);

int receiveAckFrame(int fd, int ns);

int sendInfoFrame(int fd, int ns, char* info, int length);

int receiveInfoFrame(int fd, char* info);

unsigned char* makeControlPacket(unsigned char control, int fileSize, char* fileName);

unsigned char* makeDataPacket(char *info, int N);

#endif /*PROTOCOL_H*/
