#ifndef PORT_H
#define PORT_H

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "macros.h"

int initPort(int portInt, int vtime, int vmin, int status);
int closePort(int fd, int status);

#endif /*PORT_H*/