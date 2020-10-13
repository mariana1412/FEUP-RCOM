#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "macros.h"

int sendSetFrame(int fd);

int receiveSetFrame(int fd);

int sendUAFrame(int fd);

int receiveUAFrame(int fd);

#endif /*MACROS_H*/
