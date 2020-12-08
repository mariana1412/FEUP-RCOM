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
 * @brief Reads server response
 * 
 * @param socket_fd socket file descriptor
 * @param response array where it will be stored the response message
 * 
 * @return response code on succes; -1 on error
*/
int read_response(int socket_fd, char* response);

/**
 * @brief Sends a message to server
 * 
 * @param socket_fd socket file descriptor
 * @param command message to send
 * 
 * @return 0 on succes; -1 on error
*/
int send_command(int socket_fd, char * command);

/**
 * @brief Sends a message to server and reads its response, checking if it is valid
 * 
 * @param socket_fd socket file descriptor
 * @param command message to send
 * @param response_code code that server will send on success
 * @param response array where it will be stored the response message
 * 
 * @return 0 on succes; -1 on error
*/
int send_command_receive_response(int socket_fd, char* command, int response_code, char *response);

#endif /*SOCKET_H*/