#ifndef FTP_CONNECTION_H
#define FTP_CONNECTION_H

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "socket.h"
#include "macros.h"

/**
 * @brief Establishs a TCP connection and checks server welcome message
 * 
 * @param port server port
 * @param ip_address ip address of server
 * 
 * @return Socket file descriptor on success; -1 on error
*/
int ftp_connect(int port, char *ip_address);

int ftp_login(int socket_fd, char *username, char *password);

int ftp_passive_mode(int socket_fd);

int ftp_request_file(int socket_fd, const char*path);

int ftp_download_file(int data_fd, const char *path, const char *filename);

#endif /*FTP_CONNECTION_H*/