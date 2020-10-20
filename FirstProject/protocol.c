#include "protocol.h"

//volatile int FINISH = FALSE;
int alarmSender = 1;
int alarmReceiver = 1;

int changeState(State *state, unsigned char byte, char msg){ //mudar nome de msg -> S se SET, U se UA
    switch (*state)
    {
    case START:
        if(byte == FLAG){
            *state = FLAG_RCV;
            printf("state = start -> flag!!! \n");
        }
        break;
    
    case FLAG_RCV:
        if(byte == FLAG){
            printf("state = flag_rcv -> flag!!! \n");
        }
        else if(byte == SEND_REC){
            printf("state = flag_rcv -> a_rec!!! \n");
            *state = A_RCV;
        }
        else
        {
            printf("state = flag_rcv -> else!!! \n");
            *state = START;
        }
        
        break;

    case A_RCV:
        if((msg == 's' && (byte == SET_COMMAND)) || (msg == 'u' && (byte == UA_ANSWER))){
            printf("state = a_rcv -> set or ua!!! \n");
            *state = C_RCV;
        }  
        else if(byte == FLAG){
            printf("state = a_rcv -> flag!!! \n");
            *state = FLAG_RCV;
        }
        else {
            printf("state = a_rcv -> else!!! \n");
            *state = START;
        }

        break;
    
    case C_RCV:
        if((msg == 's' && (byte == (SEND_REC ^ SET_COMMAND))) || (msg == 'u' && (byte == (SEND_REC ^ UA_ANSWER)))){
            printf("state = c_rcv -> ^ !!! \n");
            *state = BCC_OK;
        }  
        else if(byte == FLAG){
            printf("state = c_rcv -> flag!!! \n");
            *state = FLAG_RCV;
        }
        else {
            printf("state = c_rcv -> else!!! \n");
            *state = START;
        }
        break;

    case BCC_OK:
        if(byte == FLAG){
            printf("state = bcc_ok -> flag!!! \n");
            *state = STOP;
        }
        else{
            printf("state = bcc_ok -> else!!! \n");
            *state = START;
        }
        break;
    }

    return 0;
}


int sendSetFrame(int fd){
    int res;
    char setFrame[S_FRAME_SIZE];

    setFrame[0] = FLAG;
    setFrame[1] = SEND_REC;
    setFrame[2] = SET_COMMAND;
    setFrame[3] = SEND_REC ^ SET_COMMAND;
    setFrame[4] = FLAG;

    res = write(fd, setFrame, S_FRAME_SIZE);

    if(res == -1) {
        printf("Could not send SET FRAME!\n");
        return -1;
    }

    printf("Set message sent! %d bytes written\n", res);

    return 0;
}

int receiveSetFrame(int fd){
    char buf[255];
    int res;

    State state = START;

    while (state != STOP && alarmReceiver == 1) {       
        res = read(fd, buf, 1);   
        if(res == 0) continue;
        if(res < 0) return -1;
              
        printf("%d\n", buf[0]);

        changeState(&state, buf[0], 's');     
    }
    
    if (alarmReceiver == 0) return 1;

    return 0;
}

int sendUAFrame(int fd) {
    int res;
    char uaFrame[S_FRAME_SIZE];

    uaFrame[0] = FLAG;
    uaFrame[1] = SEND_REC;
    uaFrame[2] = UA_ANSWER;
    uaFrame[3] = SEND_REC ^ UA_ANSWER;
    uaFrame[4] = FLAG;

    res = write(fd, uaFrame, S_FRAME_SIZE);

    if(res < 1) {
        printf("Could not send UA FRAME!\n");
        return -1;
    }

    printf("UA message sent! %d bytes written\n", res);

    return 0;
}

int receiveUAFrame(int fd){
    char buf[255];
    int res;

    State state = START;

    while (state != STOP && alarmSender == 1) {       /* loop for input */
        res = read(fd, buf, 1);   /* returns after 1 chars have been input */
        if(res == 0) continue;
        if(res < 0) return -1;

        printf("%d\n", buf[0]);

        changeState(&state, buf[0], 'u'); 
    }

    if (alarmSender == 0) return 1;

    return 0;    
}
