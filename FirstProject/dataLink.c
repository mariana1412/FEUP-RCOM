#include "dataLink.h"

int alarmSender = 1;

void alarmHandler(){
    alarmSender = 0;   
}

int aux(int fd){ //MUDAR NOME DISTO
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
            }
            else if (recUA == -1) {
                printf("Could not read from port!\n");
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

    (void) signal(SIGALRM, alarmHandler); //confirmar se isto fica aqui
    int fd = init(port, 1, 0, status);

    if (status == SENDER){
        int res = aux(fd);
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
            sendUAFrame(fd);
            return fd;

        }
        else if (recSet == -1) {
            printf("Could not read from port!\n");
            return -1;
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
    //confirmar se o outro ciclo com alarme está funcional pq este vai ser super parecido
    if (status == SENDER){
        //send disc, receive disc, send ua 
    }
    else if (status == RECEIVER){
        //send disc
    }
    else
        return -1;
    
    return closePort(fd, status);
}