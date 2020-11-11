#ifndef PORT_H
#define PORT_H

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "macros.h"

/**
 * @brief Opens a serial port, saving its attributes in the respective termios static struct
 * 
 * @param portInt the port to be opened is "/dev/ttySX", where X is portInt
 * @param vtime the character timer
 * @param vmin the minimum number of characters to receive before satisfying the read
 * @param status SENDER/RECEIVER
 * 
 * @return file descriptor of port on success; -1 on error
 */
int initPort(int portInt, int vtime, int vmin, int status);

/**
 * @brief Closes a serial port, setting its attributes from the respective termios static struct
 * 
 * @param fd the file descriptor of the port
 * @param status SENDER/RECEIVER
 * 
 * @return 0 on success; -1 on error
 */
int closePort(int fd, int status);

#endif /*PORT_H*/