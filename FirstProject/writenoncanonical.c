#pragma once

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "dataLink.h"


int main(int argc, char** argv){

    if((argc < 2) || ((strcmp("/dev/ttyS10", argv[1])!=0) && (strcmp("/dev/ttyS11", argv[1])!=0) && (strcmp("/dev/ttyS0", argv[1])!=0) && (strcmp("/dev/ttyS1", argv[1])!=0))) {
        printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
        exit(1);
    }

    int port;
    if(strcmp("/dev/ttyS10", argv[1]) == 0) port = COM10;
    if(strcmp("/dev/ttyS11", argv[1]) == 0) port = COM11;
    if(strcmp("/dev/ttyS0", argv[1]) == 0) port = COM0;
    if(strcmp("/dev/ttyS1", argv[1]) == 0) port = COM1;

    int fd = llopen(port, SENDER);
    if(fd < 0){
        printf("llopen failed\n");
        exit(1);
    } else {
        printf("Connection established with success!\n");
    }

    unsigned char* filename = "pinguim.gif";
    
    struct stat fileInfo;
    stat(filename, &fileInfo);

    unsigned char* StartPacket = (unsigned char*)malloc(MAX_PACKET_SIZE);
    if (StartPacket == NULL) {
        printf("Could not allocate memory for StartPacket!\n");
        if(llclose(fd, SENDER) < 0){
            printf("llclose failed\n");
            exit(1);
        }
        return 0;
    }

    int packet_size;

    packet_size = makeControlPacket(START_BYTE, fileInfo.st_size, filename, StartPacket);

    if (llwrite(fd, StartPacket, packet_size) == -1) {
        printf("Could not send Start Packet!\n");
        if(llclose(fd, SENDER) < 0){
            printf("llclose failed\n");
            exit(1);
        }
        return 0;
    }

    int fdFile = open(filename, O_RDONLY);
    if (fdFile == -1) {
        printf("Could not open file!\n");
        if(llclose(fd, SENDER) < 0){
            printf("llclose failed\n");
            exit(1);
        }
        return 0;
    }
    int charactersRead;
    int sequenceN = 0;

    unsigned char* fileBuffer = (unsigned char*)malloc(MAX_K);
    if (fileBuffer == NULL) {
        printf("Could not allocate memory for fileBuffer!\n");
        if(llclose(fd, SENDER) < 0){
            printf("llclose failed\n");
            exit(1);
        }
        return 0;
    }

    unsigned char* dataPacket = (unsigned char*)malloc(MAX_PACKET_SIZE);
    if (dataPacket == NULL) {
        printf("Could not allocate memory for dataPacket!\n");
        if(llclose(fd, SENDER) < 0){
            printf("llclose failed\n");
            exit(1);
        }
        return 0;
    }

    int size = 0;

    while (charactersRead = read(fdFile, fileBuffer, MAX_K)) {
        packet_size = makeDataPacket(fileBuffer, sequenceN, dataPacket, charactersRead);

        if (llwrite(fd, dataPacket, packet_size) == -1) {
            printf("Could not send Data Packet number %d\n", sequenceN);
            if(llclose(fd, SENDER) < 0){
                printf("llclose failed\n");
                exit(1);
            }
            return 0;
        }

        sequenceN++;
        size += charactersRead;
    }

    close(fdFile);

    unsigned char* EndPacket = (unsigned char*)malloc(MAX_PACKET_SIZE);
    if (EndPacket == NULL) {
        printf("Could not allocate memory for EndPacket!\n");
        if(llclose(fd, SENDER) < 0){
            printf("llclose failed\n");
            exit(1);
        }
        return 0;
    }
    
    packet_size = makeControlPacket(END_BYTE, fileInfo.st_size, "pinguim.gif", EndPacket);

    if (llwrite(fd, EndPacket, packet_size) == -1) {
        printf("Could not send End Packet!");
        if(llclose(fd, SENDER) < 0){
            printf("llclose failed\n");
            exit(1);
        }
        return 0;
    }

    printf("SIZE: %d, \n", size, fileInfo.st_size);

    if(llclose(fd, SENDER) < 0){
        printf("llclose failed\n");
        exit(1);
    }

    return 0;
}