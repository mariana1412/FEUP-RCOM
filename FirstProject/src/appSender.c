#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "dataLink.h"

int main(int argc, char **argv)
{

    if ((argc != 3) || ((strcmp("/dev/ttyS10", argv[1]) != 0) && (strcmp("/dev/ttyS11", argv[1]) != 0) && (strcmp("/dev/ttyS0", argv[1]) != 0) && (strcmp("/dev/ttyS1", argv[1]) != 0)))
    {
        printf("Usage:\t./sender <SerialPort> <path>\n");
        exit(1);
    }

    int port;
    if (strcmp("/dev/ttyS10", argv[1]) == 0)
        port = COM10;
    if (strcmp("/dev/ttyS11", argv[1]) == 0)
        port = COM11;
    if (strcmp("/dev/ttyS0", argv[1]) == 0)
        port = COM0;
    if (strcmp("/dev/ttyS1", argv[1]) == 0)
        port = COM1;

    int fd = llopen(port, SENDER);
    if (fd < 0)
    {
        printf("llopen failed\n");
        exit(1);
    }
    else
    {
        printf("Connection established with success!\n");
    }


    unsigned char *path = argv[2];

    struct stat fileInfo;
    if(stat(path, &fileInfo) < 0){
        printf("stat failed!\n");
        if (closePort(fd, SENDER) < 0) printf("closePort failed\n");
        return -1;
    }

    unsigned char *StartPacket = (unsigned char *)malloc(MAX_PACKET_SIZE);
    if (StartPacket == NULL)
    {
        printf("Could not allocate memory for StartPacket!\n");
        if (closePort(fd, SENDER) < 0) printf("closePort failed\n");
        return -1;
    }

    int packet_size;

    unsigned char *filename = strrchr(path, '/');

    packet_size = makeControlPacket(START_BYTE, fileInfo.st_size, filename, StartPacket);

    if (llwrite(fd, StartPacket, packet_size) == -1)
    {
        free(StartPacket);
        printf("Could not send Start Packet!\n");
        if (closePort(fd, SENDER) < 0) printf("closePort failed\n");
        return -1;
    }

    int fdFile = open(path, O_RDONLY);
    if (fdFile == -1)
    {
        free(StartPacket);
        printf("Could not open file!\n");
        if (closePort(fd, SENDER) < 0) printf("closePort failed\n");
        return -1;
    }

    int charactersRead;
    long int sequenceN = 0;

    unsigned char *fileBuffer = (unsigned char *)malloc(MAX_K);
    if (fileBuffer == NULL)
    {
        free(StartPacket);
        printf("Could not allocate memory for fileBuffer!\n");
        if (closePort(fd, SENDER) < 0) printf("closePort failed\n");
        return -1;
    }

    unsigned char *dataPacket = (unsigned char *)malloc(MAX_PACKET_SIZE);
    if (dataPacket == NULL)
    {
        free(StartPacket);
        free(fileBuffer);
        printf("Could not allocate memory for dataPacket!\n");
        if (closePort(fd, SENDER) < 0) printf("closePort failed\n");
        return -1;
    }

    int size = 0;

    while (charactersRead = read(fdFile, fileBuffer, MAX_K))
    {
        packet_size = makeDataPacket(fileBuffer, sequenceN, dataPacket, charactersRead);

        if (llwrite(fd, dataPacket, packet_size) == -1)
        {
            free(StartPacket);
            free(fileBuffer);
            free(dataPacket);
            printf("Could not send Data Packet number %ld\n", sequenceN);
            if (closePort(fd, SENDER) < 0) printf("closePort failed\n");
            return -1;
        }

        sequenceN++;
        sequenceN %= 255;
        size += charactersRead;
    }

    close(fdFile);

    unsigned char *EndPacket = (unsigned char *)malloc(MAX_PACKET_SIZE);
    if (EndPacket == NULL)
    {
        free(StartPacket);
        free(fileBuffer);
        free(dataPacket);
        printf("Could not allocate memory for EndPacket!\n");
        if (closePort(fd, SENDER) < 0) printf("closePort failed\n");
        return -1;
    }

    packet_size = makeControlPacket(END_BYTE, fileInfo.st_size, filename, EndPacket);

    if (llwrite(fd, EndPacket, packet_size) == -1)
    {
        free(StartPacket);
        free(fileBuffer);
        free(dataPacket);
        free(EndPacket);
        printf("Could not send End Packet!\n");
        if (closePort(fd, SENDER) < 0) printf("closePort failed\n");
        return -1;
    }

    free(StartPacket);
    free(fileBuffer);
    free(dataPacket);
    free(EndPacket);

    if (llclose(fd, SENDER) < 0)
    {
        printf("llclose failed\n");
        exit(1);
    }

    return 0;
}