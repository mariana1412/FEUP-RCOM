#ifndef DATALINK_H
#define DATALINK_H

#include "macros.h"
#include "port.h"

void alarmHandler();

int aux(int fd);
int llopen(int port, int status);

int llwrite(int fd, char* buffer, int length);

int llread(int fd, char* buffer);

int llclose(int fd, int status);

#endif /*DATALINK_H*/