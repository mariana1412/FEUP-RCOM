#include "dataLink.h"

int alarmSender = 1;

int llopen(int port, int status) {
    
    int fd = init(port, 1, 0, status);

    if (status == SENDER) {
        int recUA;

        for(int i = 0; i < 4; i++){

            if(sendSetFrame(fd) == -1){
                printf("Could not send SET Frame!\n");
                return -1;
            }

            alarmSender = 1;
            alarm(3);

            while(alarmSender){  
                recUA = receiveUAFrame(fd);
                if(recUA == 0){ 
                    alarm(0);
                    break;  
                } else if (recUA == -1) {
                    printf("Could not read from port!\n");
                    return -1;
                }
            }

            if(alarmSender){
                printf("Received UA Frame with success!\n");
                return fd;
            } else if (i < 3) {
                printf("Timeout number %d, trying again...\n", i+1);
            }
        }
    } else if (status == RECEIVER) {
        int recSet;

        recSet = receiveSetFrame(fd);
        if (recSet == 0) {
            printf("Received SET Frame with success!\n");
            sendUAFrame(fd);
            return fd;
        } else if (recSet == -1) {
            printf("Could not read from port!\n");
            return -1;
        }

    }

    return -1;
}

int llclose(int fd, int status){ 
    if (status == SENDER){
        //send disc, ua
    }
    else if (status == RECEIVER){
        //send disc
    }

    return closePort(fd, status);
}