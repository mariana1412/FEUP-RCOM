#ifndef DATALINK_H
#define DATALINK_H

#pragma once

#include "protocol.h"
#include "port.h"
#include <unistd.h>

/**
 * Establishes a connection between devices, using a Stop and Wait mechanism.
 * 
 * @param port the port to be opened
 * @param status SENDER/RECEIVER
 * 
 * @return the file descriptor to be written on / read from on success, -1 otherwise
 */
int llopen(int port, int status);

/**
 * Sends an array of characters to the other device, using a Stop and Wait mechanism
 * 
 * @param fd the file descriptor
 * @param buffer the array to be sent
 * @param length the array length
 * 
 * @return the amount of bytes sent; -1 on error
 */
int llwrite(int fd, char *buffer, int length);

/**
 * Receives an array of characters from the other device, using a Stop and Wait mechanism.
 * 
 * @param fd the file descriptor
 * @param buffer the array to store the data received
 * 
 * @return the amount of bytes received; -1 on error
 */
int llread(int fd, char *buffer);

/**
 * Closes the connection previously established, using a Stop and Wait mechanism.
 * 
 * @param fd the file descriptor to be closed
 * @param status SENDER/RECEIVER
 * 
 * @return 0 on success, -1 otherwise
 */
int llclose(int fd, int status);

#endif /*DATALINK_H*/