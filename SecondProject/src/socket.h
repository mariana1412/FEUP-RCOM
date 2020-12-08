#ifndef SOCKET_H
#define SOCKET_H

#pragma once

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>
#include <string.h>

#include "macros.h"

/**
 * @brief Establish a TCP connection
 * 
 * @param port server port
 * @param ip_address ip address of server
 * 
 * @return Socket file descriptor on success; -1 on error
*/
int socket_establish_connection(int port, char* ip_address);


/**
 * 
*/
int read_response(int socket_fd, char* response);

/**
 * 
*/
int send_command_receive_response(int socket_fd, char* command, int response_code, char *response);

#endif /*SOCKET_H*/