#ifndef DATALINK_H
#define DATALINK_H

#pragma once

#include "protocol.h"
#include "port.h"

void alarmHandler();

int SandWOpenClose(int fd, ControlCommand send, ControlCommand receive);

int llopen(int port, int status);

int llwrite(int fd, char* buffer, int length);

int llread(int fd, char* buffer);

int llclose(int fd, int status);

#endif /*DATALINK_H*/