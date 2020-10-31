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
    }

    struct stat fileInfo;
    stat("pinguim.gif", &fileInfo);

    unsigned char* StartPacket = makeControlPacket(START_BYTE, fileInfo.st_size, "pinguim.gif");
    StartPacket[strlen(StartPacket)] = 0;

    if (llwrite(fd, StartPacket, strlen(StartPacket)) == -1) {
        printf("Could not send Start Packet!");
        if(llclose(fd, SENDER) < 0){
            printf("llclose failed\n");
            exit(1);
        }
    }

    int fdFile = open("pinguim.gif", O_RDONLY);
    int charactersRead;
    char* fileBuffer;
    int sequenceN = 0;
    int ns = 1;

    while(charactersRead = read(fdFile, fileBuffer, IFRAME_MAX_SIZE - 10) != 0) {
        char* dataPacket = makeDataPacket(fileBuffer, sequenceN);
        dataPacket[strlen(dataPacket)] = ns;

        if (llwrite(fd, dataPacket, strlen(dataPacket)) == -1) {
            printf("Could not send Data Packet number %d", sequenceN);
            if(llclose(fd, SENDER) < 0){
                printf("llclose failed\n");
                exit(1);
            }
        }

        sequenceN++;
        ns = 1 - ns;
    }

    unsigned char* EndPacket = makeControlPacket(END_BYTE, fileInfo.st_size, "pinguim.gif");
    EndPacket[strlen(EndPacket)] = ns;

    if (llwrite(fd, EndPacket, strlen(EndPacket)) == -1) {
        printf("Could not send End Packet!");
        if(llclose(fd, SENDER) < 0){
            printf("llclose failed\n");
            exit(1);
        }
    }

    if(llclose(fd, SENDER) < 0){
        printf("llclose failed\n");
        exit(1);
    }

    return 0;
}