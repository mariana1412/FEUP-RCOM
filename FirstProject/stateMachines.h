#ifndef STATEMACHINES_H
#define STATEMACHINES_H

#include <stdio.h>
#include <string.h>
#include "macros.h"
#include "dataStructures.h"

int changeStateS(State *state, unsigned char byte, ControlCommand command, unsigned char address);

int changeStateInfo(State *state, unsigned char byte, int fd);

int readControlPacket(int fd);

int checkControlPacket(int fd);

int readDataPacket(int fd);

File* getFile();

int getREJ();

#endif /*STATEMACHINES_H*/

