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


    //llwrite e llread!!!

    if(llclose(fd, SENDER) < 0){
        printf("llclose failed\n");
        exit(1);
    }

    return 0;
}