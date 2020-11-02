#include "dataLink.h"

static int ns = 0;

int llopen(int port, int status){
    struct sigaction newAction, oldAction;

    newAction.sa_handler = alarmHandler;
    sigemptyset(&newAction.sa_mask);
    newAction.sa_flags = 0;

    sigaction(SIGALRM, &newAction, &oldAction);

    int fd = initPort(port, 0, 0, status);
    
    int res;

    if (status == SENDER){
        res = SandWOpenClose(fd, SET, UA);
        if(res == 0) return fd;
        else if(res == -1){
            printf("Could not receive UA Frame!\n");
            return -1;
        }
    }
    else if (status == RECEIVER){
        int recSet = receiveOpenCloseFrame(fd, SET, SEND_REC);

        if (recSet == 0) {
            printf("Received SET Frame with success!\n");
            res = sendOpenCloseFrame(fd, UA, SEND_REC);
            if (res == 0) {
                printf("Sent UA Frame!\n");
                return fd;
            } else {
                printf("Could not send UA Frame!\n");
            }
        }
        else if (recSet == -1) {
            printf("Could not read from port!\n");
        }
    }
    return -1;
}

int llwrite(int fd, char* buffer, int length){
    struct sigaction newAction, oldAction;

    newAction.sa_handler = alarmHandler;
    sigemptyset(&newAction.sa_mask);
    newAction.sa_flags = 0;

    sigaction(SIGALRM, &newAction, &oldAction);

    int bytesSent;
    int response;

    for (int i = 0; i < 4; i++) {
        bytesSent = sendInfoFrame(fd, ns, buffer, length);
        if (bytesSent == -1) {
            printf("Could not send I Frame! Attempt number %d\n", i+1);
            return -1;
        } else {
            printf("Sent I Frame! Attempt number %d\n", i+1);
        }

        alarmSender = 1;
        alarm(3);

        while (alarmSender) {
            response = receiveAckFrame(fd, 1-ns);
            if (response < 0) {
                printf("Could not read ACK Frame!\n");
            } else {
                alarm(0);
                break;
            }
        }

        if (alarmSender) {
            if (response == 0) {
                printf("ACK Frame received with success!\n");
                ns = 1-ns;
                return bytesSent;
            } else if (i < 3) {
                printf("Rejected, trying again...\n", i+1);
            }
        } else if (i < 3) {
            printf("Timeout number %d, trying again...\n", i+1);
        }
    }

    return -1;
}


int llread(int fd, char* buffer){
    int receive = receiveInfoFrame(fd, buffer);

    if(getREJ() || (receive < 0)){
        printf("send rej message\n");
        sendAckFrame(fd, REJ, receive);
    }
    else{
        printf("send rr message!\n");
        sendAckFrame(fd, RR, 1-receive);
    }
    
    return IFRAME_SIZE;
}

int llclose(int fd, int status){
    struct sigaction newAction, oldAction;

    newAction.sa_handler = alarmHandler;
    sigemptyset(&newAction.sa_mask);
    newAction.sa_flags = 0;

    sigaction(SIGALRM, &newAction, &oldAction);

    int res;

    if (status == SENDER){
        res = SandWOpenClose(fd, DISC, DISC);
        if(res == 0) {
            printf("Received DISC Frame with success!\n");
            res = sendOpenCloseFrame(fd, UA, SEND_REC);
            if (res == 0) {
                printf("Sent UA Frame!\n");
            } else {
                printf("Could not send UA Frame!\n");
                return -1;
            }
        }
        else if(res == -1){
            printf("Could not receive DISC Frame!\n");
            return -1;
        }
    }
    else if (status == RECEIVER){
        int recDISC = receiveOpenCloseFrame(fd, DISC, SEND_REC);

        if (recDISC == 0) {
            printf("Received DISC Frame with success!\n");
            res = SandWOpenClose(fd, DISC, UA);
            if (res == 0) {
                printf("Received UA Frame with success!\n");
            } else if (res == -1) {
            printf("Could not receive UA Frame!\n");
            return -1;
            }
        }
        else if (recDISC == -1) {
            printf("Could not read from port!\n");
            return -1;
        }
    }
    
    return closePort(fd, status);
}