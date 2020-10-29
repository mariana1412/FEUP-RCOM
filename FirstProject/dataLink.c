#include "dataLink.h"

void alarmHandler(){
    alarmSender = 0; 
    return;  
}

int SandWOpenClose(int fd, ControlCommand send, ControlCommand receive) { //MUDAR NOME DISTO
    int rec;

    for(int i = 0; i < 4; i++){
        if(sendOpenCloseFrame(fd, SET, SEND_REC) == -1){
            printf("Could not send SET Frame!\n");
            return -1;
        } else {
            printf("Sent SET Frame!\n");
        }

        alarmSender = 1;
        alarm(3);

        while(alarmSender){
            rec = receiveUAFrame(fd);
            if(rec == 0){ 
                alarm(0);
                break;  
            } else if (rec < 0) {
                printf("Could not read from port! Code: %d\n", rec);
                return -1;
            }
        }

        if(alarmSender){
            printf("Received UA Frame with success!\n");
            return 0;
        }
        else if (i < 3) {
            printf("Timeout number %d, trying again...\n", i+1);
        }
    }
    return -1;
}


int llopen(int port, int status){
    // (void) signal(SIGALRM, alarmHandler); //confirmar se isto fica aqui

    struct sigaction newAction, oldAction;

    newAction.sa_handler = alarmHandler;
    sigemptyset(&newAction.sa_mask);
    newAction.sa_flags = 0;

    sigaction(SIGALRM, &newAction, &oldAction);
    // sigaction(SIGALRM, &newAction, NULL);

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
        int recSet = receiveSetFrame(fd);

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
    //tratar de stuffing e unstuffing
    //ler mensagem I;
    //enviar mensagem com rr ou rej
}

int llread(int fd, char* buffer){
    //tratar de stuffing e unstuffing
    //ciclo para tentativas com alarme:
    //enviar mensagem I;
    //receber mensagem e verificar se é rr ou rej
}

int llclose(int fd, int status){
    if (status == SENDER){
        //send disc, receive disc, send ua 
    }
    else if (status == RECEIVER){
        //send disc, receive ua
    }
    else
        return -1;
    
    return closePort(fd, status);
}