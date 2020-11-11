#ifndef APPSENDER_H
#define APPSENDER_H

#pragma once

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "dataLink.h"


void printInformation(long int filesize, unsigned char* filename);

/**
 * @brief Makes a control packet with all the information passed as arguments and stores it in packet
 * 
 * @param control START_BYTE/DATA_BYTE/END_BYTE
 * @param fileSize the size of the file
 * @param fileName the name of the file
 * @param packet the control packet
 * 
 * @return the number of bytes written in the packet
 * 
*/
int makeControlPacket(unsigned char control, long int fileSize, unsigned char *fileName, unsigned char *packet);

/**
 * @brief Makes a data packet with all the information passed as arguments and stores it in packet
 * 
 * @param info the info of the file
 * @param N the sequence number of the packet
 * @param length the number of bytes in info
 * @param packet the control packet
 * 
 * @return the number of bytes written in the packet
 * 
*/
int makeDataPacket(unsigned char *info, int N, unsigned char *packet, int length);

#endif /*APPSENDER_H*/