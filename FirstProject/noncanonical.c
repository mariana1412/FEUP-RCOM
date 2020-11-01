#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "dataLink.h"
#include "macros.h"
#include "dataStructures.h"

static File *file;
static int N = 0;

int parseInfo(unsigned char *info, int size)
{

    unsigned char byte = info[0];
    printf("PARSE INFO %d!\n", byte);

    switch (byte)
    {
    case START_BYTE:
        printf("START_BYTE\n");
        if (parseControlPacket(info, size) < 0)
            return -1;
        file->controlPacket = ++info; //confirmar
        break;

    case DATA_BYTE:
        printf("DATA_BYTE\n");
        if (parseDataPacket(info, size) < 0)
            return -1;
        N++;
        break;

    case END_BYTE:
        printf("END_BYTE\n");
        if (checkControlPacket(info, size) < 0)
        {
            printf("End Control Packet is not corret!\n");
            return -1;
        }
        return 1;
        break;

    default:
        break;
    }

    return 0;
}

int parseControlPacket(unsigned char *info, int size)
{

    int l, j;

    printf("parse control packet, size = %d\n", size);

    for (int i = 1; i < size; i++)
    {

        if (info[i++] == FILESIZE)
        {
            printf("FILESIZE \n");
            l = info[i++];
            j = 0;
            while (j != l)
            {
                printf("READING FILESIZE \n");
                file->size = file->size * 256 + info[i++];

                j++;
            }
        }
        else if (info[i++] == FILENAME)
        {
            printf("FILENAME \n");
            l = info[i++];
            j = 0;

            while (j != l)
            {
                printf("READING FILENAME \n");
                file->name[j++] = info[i++];
            }
            break;
        }
    }

    printf("start -> control packet is correct!\n");
    return 0;
}

int checkControlPacket(unsigned char *info, int size)
{

    for (int i = 1; i < size; i++)
    {
        if (info[i] != file->controlPacket[i])
            return -1;
    }

    printf("end -> control packet is correct!\n");
    return 0;
}

int parseDataPacket(unsigned char *info, int size)
{
    int index = 0;
    unsigned char byte = info[index++];

    if (byte != N)
    {
        printf("N is not right!\n");
        return -1;
    }

    int L2 = info[index++];
    int L1 = info[index++];

    int K = 256 * L2 + L1;

    if (index + K > file->size)
    {
        printf("index + K > size\n");
        return -1;
    }

    for (int i = 0; i < K; i++)
    {
        file->data[file->lastIndex++] = info[index++];
    }
}

int main(int argc, char **argv)
{

    if ((argc < 2) || ((strcmp("/dev/ttyS10", argv[1]) != 0) && (strcmp("/dev/ttyS11", argv[1]) != 0) && (strcmp("/dev/ttyS0", argv[1]) != 0) && (strcmp("/dev/ttyS1", argv[1]) != 0)))
    {
        printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
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

    int fd = llopen(port, RECEIVER);
    if (fd < 0)
    {
        printf("llopen failed\n");
        exit(1);
    }

    int size, totalSize, packets;
    int finished = FALSE;

    printf("finished llopen\n");

    file = malloc(sizeof(File));
    file->size = 0;
    file->lastIndex = 0;
    file->name = (unsigned char*)malloc(MAX_PACKET_SIZE); //ver isto
    file->data = (unsigned char*)malloc(IFRAME_SIZE); //ver isto
    file->controlPacket = (unsigned char*)malloc(MAX_PACKET_SIZE);

    while (!finished)
    {
        unsigned char *info = (unsigned char *)malloc(MAX_PACKET_SIZE);
        size = llread(fd, info);

        if (size < 0)
            break;
        totalSize += size;

        int result = parseInfo(info, size);
        
        if (result == 1)
            finished = TRUE;
        if (result == -1)
            break;

        packets++;
        free(info);
    }

    if (!finished)
        printf("cycle interrupted!\n");
    else
    {
        printf("Received %d bytes and %d packets.\n", totalSize, packets);

        int fileDescriptor = open(file->name, O_RDWR | O_CREAT, 0777);
        if (fileDescriptor < 0)
        {
            printf("Error opening file!\n");
            return -1;
        }

        if (write(fileDescriptor, file->data, file->size) < 0)
        {
            printf("Error writing to file!\n");
            return -1;
        }

        if (close(fd) < 0)
        {
            printf("Error closing file!\n");
            return -1;
        }
    }

    if (llclose(fd, RECEIVER) < 0)
    {
        printf("llclose failed\n");
        exit(1);
    }

    return 0;
}