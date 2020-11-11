#ifndef APPRECEIVER_H
#define APPRECEIVER_H

#pragma once

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "dataLink.h"
#include "macros.h"
#include "dataStructures.h"

/**
 * @brief shows the connection information on the screen
 * 
 */
void printConnectionInfo();

/**
 * @brief shows the file information on the screen
 * 
 */
void printFileInformation(long int filesize, unsigned char* filename);

/**
 * @brief Initializes and allocates memory for the struct File and its members
 * 
 * @return 0 on success; -1 on error
*/
int initFile();

/**
 * @brief Free the memory previously allocated to the struct File 
*/
void freeFile();

/**
 * @brief Fowards info and its size to the respective function according to info's first byte
 * 
 * @param info the packet to parse
 * @param size the size of the packet to parse
 * 
 * @return 1 if it's an end packet; 0 if it's a start or data packet; -1 on error
*/
int parseInfo(unsigned char *info, int size);

/**
 * @brief Parses control packet (START), storing the file's information in the struct File
 * 
 * @param info the packet to parse
 * @param size the size of the packet to parse
 * 
 * @return 0 on success; -1 on error
*/
int parseControlPacket(unsigned char *info, int size);

/**
 * @brief Parses data packet, storing the file's information in the struct File
 * 
 * @param info the packet to parse
 * @param size the size of the packet to parse
 * 
 * @return 0 on success; -1 on error
*/
int parseDataPacket(unsigned char *info, int size);

/**
 * @brief Parses control packet (END) and compares it with the initial packet stored in controlPacket of the struct File
 * 
 * @param info the packet to parse
 * @param size the size of the packet to parse
 * 
 * @return 0 on success; -1 on error
*/
int checkControlPacket(unsigned char *info, int size);

#endif /*APPRECEIVER_H*/