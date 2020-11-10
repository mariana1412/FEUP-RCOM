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

static File *file;
static int N = 0;

void freeFile(){
    free(file->data);
    free(file->name);
    free(file->controlPacket);
    free(file);
}

int checkControlPacket(unsigned char *info, int size) {
    int i = 1;
    int index = 0;
    int l, j, k = 0;

    unsigned char* temp = (unsigned char*)malloc(MAX_PACKET_SIZE);
    if (temp == NULL) {
        printf("Could not allocate memory for temp!\n");
        return -1;
    }

    while(i < size)
    {
        if (info[i] == FILESIZE)
        {
            temp[k++] = info[i];
            i++;

            temp[k++] = info[i];
            l = info[i++];
            j = 0;

            while (j != l)
            {
                temp[k++] = info[i++];

                j++;
            }
        }
        else if (info[i] == FILENAME)
        {
            temp[k++] = info[i];
            i++;
            
            temp[k++] = info[i];
            l = info[i++];
            j = 0;

            while (j != l)
            {
                temp[k++] = info[i++];
                j++;
            }
            break;
        }
    }

    while (index < k) {
        if (file->controlPacket[index] != temp[index++]) {
            free(temp);
            return -1;
        }
    }

    free(temp);
    return 0;
}

int parseControlPacket(unsigned char *info, int size) {
    int i = 1;
    int index = 0;
    int l, j;

    while(i < size)
    {
        if (info[i] == FILESIZE) {
            file->controlPacket[index++] = info[i++];
            
            file->controlPacket[index++] = info[i];
            l = info[i++];
            j = 0;

            unsigned char* sizeString = (unsigned char*)malloc(l+1);
            if(sizeString == NULL){
                printf("Could not allocate memory for size!\n");
                free(file->name);
                free(file);
                return -1;
            }

            while (j != l)
            {
                file->controlPacket[index++] = info[i];
                sizeString[j] = info[i++];
                j++;
            }
            sizeString[j] = 0;
            file->size = atoi(sizeString);
            free(sizeString);

        } else if (info[i] == FILENAME) {
            file->controlPacket[index++] = info[i++];

            file->controlPacket[index++] = info[i];
            l = info[i++];
            j = 0;

            while (j != l)
            {
                file->controlPacket[index++] = info[i];
                file->name[j++] = info[i++];
            }
            file->name[j] = 0;
            break;
        }
    }
    printf("file->size = %ld\n", file->size);
    long int dataSize = file->size;
    file->data = (unsigned char*)malloc(file->size);
    if(file->data == NULL){
        printf("Could not allocate memory for file->data!\n");
        free(file->name);
        free(file);
        return -1;
    }

    return 0;
}

int parseDataPacket(unsigned char *info, int size)
{
    int index = 1;
    unsigned char byte = info[index++];

    printf("n: %d\n", N);

    if (byte != N)
    {
        printf("Sequence number of data packet is not correct!\n");
        return -1;
    }

    int L2 = info[index++];
    int L1 = info[index++];

    int K = 256 * L2 + L1;


    for (int i = 0; i < K; i++)
    {
        file->data[file->lastIndex++] = info[index++];
    }
    return 0;
}

int initFile(){

    file = malloc(sizeof(File));

    if(file == NULL) return -1;
    
    file->size = 0;
    file->lastIndex = 0;

    file->name = (unsigned char*)malloc(MAX_VALUE_SIZE);
    if(file->name == NULL){ 
        free(file);
        return -1;
    }

    file->controlPacket = (unsigned char*)malloc(MAX_PACKET_SIZE);
    if(file->controlPacket == NULL){
        free(file->name);
        free(file->data);
        free(file);
        return -1;
    }

    return 0;
}

int parseInfo(unsigned char *info, int size) {

    unsigned char byte = info[0];
    printf("packet byte: %d", byte);

    switch (byte)
    {
    case START_BYTE:
        if (parseControlPacket(info, size) < 0) return -1;
        printf("parsed start\n");
        break;

    case DATA_BYTE:
        if (parseDataPacket(info, size) < 0){
            freeFile();
            return -1;
        }
        N++;
        N %= 255;
        break;

    case END_BYTE:
        printf("checking end packet\n");
        if (checkControlPacket(info, size) < 0)
        {
            freeFile();
            printf("End Control Packet is not correct!\n");
            return -1;
        }
        return 1;
        break;

    default:
        break;
    }

    return 0;
}

int main(int argc, char **argv)
{

    if ((argc != 2) || ((strcmp("/dev/ttyS10", argv[1]) != 0) && (strcmp("/dev/ttyS11", argv[1]) != 0) && (strcmp("/dev/ttyS0", argv[1]) != 0) && (strcmp("/dev/ttyS1", argv[1]) != 0)))
    {
        printf("Usage:\t./sender <SerialPort>\n");
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

    int size, totalSize = 0;
    int packets = 0;
    int finished = FALSE;

    if (initFile() < 0){
        printf("Could not allocate memory for file!\n");
        if (closePort(fd, RECEIVER) < 0) printf("closePort failed\n");
        return -1;
    }

    
    time_t t;
    srand((unsigned) time(&t));

    struct timeval beginTime, endTime;
    gettimeofday(&beginTime, NULL);

    while (!finished)
    {
        unsigned char *info = (unsigned char *)malloc(MAX_PACKET_SIZE);

        if(info == NULL){
            free(file->name);
            free(file);
            printf("Could not allocate memory for info!\n");
            if (closePort(fd, RECEIVER) < 0) printf("closePort failed\n");
            return -1;
        }

        size = llread(fd, info);

        if (size < 0) {
            free(info);
            continue;
        } else if(size == 0) {
            printf("Timeout, closing.\n");
            free(info);
            freeFile();
            if (closePort(fd, RECEIVER) < 0) printf("closePort failed\n");
            return -1;
        }
        totalSize += size;

        int result = parseInfo(info, size);
        
        if (result == 1)
            finished = TRUE;
        if (result == -1)
            break;

        packets++;

        free(info);
    }

    if (!finished){
        printf("Data reception interrupted!\n");
        if (closePort(fd, RECEIVER) < 0) printf("closePort failed\n");
        return -1;
    }
    else
    {
        gettimeofday(&endTime, NULL);
    
        double elapsed = (endTime.tv_sec - beginTime.tv_sec) * 1e6;
        elapsed = (elapsed + (endTime.tv_usec - beginTime.tv_usec)) * 1e-6;

        printf("Elapsed: %.5lf seconds\n", elapsed);

        printf("Received %d bytes and %d packets.\n", totalSize, packets);

        unsigned char* filename = (unsigned char*)malloc(MAX_VALUE_SIZE);
        sprintf(filename, "./imagesToReceive%s", file->name);

        int fileDescriptor = open(filename, O_RDWR | O_CREAT, 0777);
        if (fileDescriptor < 0)
        {
            free(filename);
            freeFile();
            printf("Error opening file!\n");
            if (closePort(fd, RECEIVER) < 0) printf("closePort failed\n");
            return -1;
        }

        if (write(fileDescriptor, file->data, file->size) < 0)
        {
            free(filename);
            freeFile();
            printf("Error writing to file!\n");
            if (closePort(fd, RECEIVER) < 0) printf("closePort failed\n");
            return -1;
        }

        if (close(fileDescriptor) < 0)
        {
            free(filename);
            freeFile();
            printf("Error closing file!\n");
            if (closePort(fd, RECEIVER) < 0) printf("closePort failed\n");
            return -1;
        }

        free(filename);
    }

    freeFile();

    if (llclose(fd, RECEIVER) < 0)
    {
        printf("llclose failed\n");
        exit(1);
    }
    
    return 0;
}