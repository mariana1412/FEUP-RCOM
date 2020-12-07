#ifndef URL_H
#define URL_H

#pragma once

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <netdb.h> 
#include <sys/types.h>
#include <netinet/in.h> 
#include <arpa/inet.h>

#define MAX_STRING_SIZE 256

/**
 * @brief Struct where it is stored the url information
 */
typedef struct url_struct {
    char *user;
    char *password;
    char *host;
    char *url_path;
    char *filename;
    char *ip_address;

} url_struct;

/**
 * @brief Allocates memory for the struct url_struct and its members
 * 
 * @return struct that was created
*/
url_struct * createUrlStruct();

/**
 * @brief Parses url and saves its data 
 * 
 * @param url url to be parsed 
 * @param urlInfo struct where it will be saved the url's data
 * 
 * @return 0 on success; -1 on error
*/
int parse_file_url(char * url, url_struct *urlInfo);

/**
 * @brief Get IP Address given a host name
 * 
 * @param url struct to get host and to save the ip address
 * 
 * @return 0 on success; -1 on error
*/
int getIpAddress(url_struct *url);

#endif /*URL_H*/