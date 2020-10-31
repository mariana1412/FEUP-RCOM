#include "dataLink.h"


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

//
//buffer-> carateres para transmitir, length -> comprimento de array de caracteres
int llwrite(int fd, char* buffer, int length){ //retorna nº de caracteres escritos, -1 quando erro
    char* startPacket = makeControlPacket()

    

    
}

//buffer-> array de carateres recebidos
int llread(int fd, char* buffer){ // retorna comprimento do array/nºcaracteres lidos
    //tratar de stuffing e unstuffing

    
    


    //ler mensagem I;
    //enviar mensagem com rr ou rej
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