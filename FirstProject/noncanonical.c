#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "protocol.h"
#include "dataLink.h"
#include "macros.h"

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

    char *info;
    int size = llread(fd, info);

    if (llclose(fd, RECEIVER) < 0)
    {
        printf("llclose failed\n");
        exit(1);
    }

    return 0;
}

//NÃO ESTÁ BEM!!!
int readDataPacket(char *info, int size)
{

    switch (byte)
    {

    case (START_BYTE):
        if (started == FALSE && (finished == FALSE))
        {
            printf("START -> Called function to read control packet!!! \n");
            if (readControlPacket(fd) == -1)
            {
                *state = START;
                printf("something went wrong -> readControlPacket\n");
            }
            else
                started = TRUE;
        }
        else
        {
            printf("Something is wrong: start/end byte!!! \n");
            started = TRUE;
            *state = START;
        }

        break;

    case (DATA_BYTE):
        if (started == FALSE)
        {
            printf("Data -> Haven't received start byte yet!\n");
            *state = START;
        }
        else
        { //TO DO: verificar se o N está a 0 ou não
            printf("Called function to read data packet!!! \n");
            started = TRUE;
        }

        break;

    case (END_BYTE):
        if (started == FALSE)
        {
            printf("End -> Haven't received start byte yet!\n");
            *state = START;
        }
        else if (checkControlPacket(fd) == 0)
        {
            printf("state = data -> c2_rcv!!! \n");
            finished = TRUE;
            *state = C2_RCV;
        }
        else
        {
            printf("Something is wrong: end byte!!! \n");
            finished = TRUE;
            *state = START;
        }
        break;
    }
}
