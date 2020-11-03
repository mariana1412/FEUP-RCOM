#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
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

int sendInfoFrame(int fd, int ns, unsigned char *info, int length);

int receiveInfoFrame(int fd, unsigned char *info);

int makeControlPacket(unsigned char control, long int fileSize, unsigned char *fileName, unsigned char *packet);

int makeDataPacket(unsigned char *info, int N, unsigned char *packet, int length);

#endif /*PROTOCOL_H*/
