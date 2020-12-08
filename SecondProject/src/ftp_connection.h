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

/**
 * @brief Log in user, sending user name and password
 * 
 * @param socket_fd socket file descriptor
 * @param username  user name
 * @param password  user password
 * 
 * @return 0 on success; -1 on error
*/
int ftp_login(int socket_fd, char *username, char *password);

/**
 * @brief Enters passive mode and establishs a tcp connection with the new data port and ip address
 * 
 * @param socket_fd socket file descriptor
 * 
 * @return data socket descriptor on success; -1 on error
*/
int ftp_passive_mode(int socket_fd);

/**
 * @brief Resquests the download of the file
 * 
 * @param socket_fd socket file descriptor
 * @param path      file path
 * 
 * @return 0 on success; -1 on error
*/
int ftp_request_file(int socket_fd, const char* path);

/**
 * @brief Downloads file
 * 
 * @param socket_fd socket file descriptor
 * @param path      file path
 * @param filename  file name
 * 
 * @return 0 on success; -1 on error
*/
int ftp_download_file(int data_fd, const char *path, const char *filename);

#endif /*FTP_CONNECTION_H*/