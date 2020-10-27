#include "protocol.h"

//volatile int FINISH = FALSE;
int alarmSender = 1;
int alarmReceiver = 1;
char bcc2Check = 0x0;

int changeStateS(State *state, unsigned char byte, ControlCommand command){
    int nr = -1;
    int isCorrect;

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
        isCorrect = FALSE;

        switch (command) {
            case SET:
                if (byte == SET_COMMAND) isCorrect = TRUE;
                break;
            case UA:
                if(byte == UA_ANSWER) isCorrect = TRUE;
                break;
            case RR:
                if (byte == 0x85) {
                    isCorrect = TRUE;
                    nr = 1;
                } else if (byte == 0x05) {
                    isCorrect = TRUE;
                    nr = 0;
                }
                break;
        }

        if(isCorrect){
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
        
        isCorrect = FALSE;

        switch (command) {
            case SET:
                if (byte == (SEND_REC ^ SET_COMMAND)) isCorrect = TRUE;
                break;
            case UA:
                if(byte == (SEND_REC ^ UA_ANSWER)) isCorrect = TRUE;
                break;
            case RR:
                
                break;
        }

        if(isCorrect){
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

    return nr;
}

int changeStateInfo(State *state, int ns, unsigned char byte) {
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
        if(byte == ns << 6){
            printf("state = a_rcv -> c_rcv!!! \n");
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
        if(byte == SEND_REC ^ (ns << 6)){
            printf("state = c_rcv -> bcc_ok !!! \n");
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
        if(byte == 0x02){
            printf("state = bcc_ok -> data!!! \n");
            *state = DATA;
        }
        else{
            printf("state = bcc_ok -> else!!! \n");
            *state = START;
        }
        break;
    case DATA:
        if(byte == 0x03){
            printf("state = data -> c2_rcv!!! \n");
            *state = C2_RCV;
        } else if (byte == FLAG) {
            printf("state = data -> flag!!! \n");
            *state = FLAG;
        } else {
            printf("received data byte!!! \n");
            bcc2Check = bcc2Check ^ byte;
        }
        break;
    case C2_RCV:
        if (byte == bcc2Check) {
            printf("state = c2_rcv -> bcc2_ok!!! \n");
            *state = BCC2_OK;
        } else if (byte == FLAG) {
            printf("state = c2_rcv -> flag!!! \n");
            *state = FLAG;
        }
        else {
            printf("state = c2_rcv -> else!!! \n");
            *state = START;
        }
        break;
    case BCC2_OK:
        if(byte == FLAG){
            printf("state = bcc2_ok -> flag!!! \n");
            *state = STOP;
        }
        else{
            printf("state = bcc2_ok -> else!!! \n");
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

    while (state != STOP) {       
        res = read(fd, buf, 1);   
        if(res == 0) continue;
        if(res < 0) return -1;
              
        printf("%d\n", buf[0]);

        changeStateS(&state, buf[0], SET);     
    }

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

        changeStateS(&state, buf[0], UA); 
    }

    if (alarmSender == 0) return 1;

    return 0;    
}

int sendRRFrame(int fd, int nr) {
    int res;
    char rrFrame[S_FRAME_SIZE];

    rrFrame[0] = FLAG;
    rrFrame[1] = SEND_REC;
    rrFrame[2] = RR(nr);
    rrFrame[3] = SEND_REC ^ RR(nr);
    rrFrame[4] = FLAG;

    res = write(fd, rrFrame, S_FRAME_SIZE);

    if(res < 1) {
        printf("Could not send RR FRAME!\n");
        return -1;
    }

    printf("RR message sent! %d bytes written\n", res);

    return 0;
}

int receiveRRFrame (int fd, int *nr) { 
    char buf[255];
    int res;
    int r;

    State state = START;

    while (state != STOP && alarmSender == 1) {       /* loop for input */
        res = read(fd, buf, 1);   /* returns after 1 chars have been input */
        if(res == 0) continue;
        if(res < 0) return -1;

        printf("%d\n", buf[0]);

        if(r = changeStateS(&state, buf[0], RR) != -1){
            *nr = r;
        }
    }

    if (alarmSender == 0) return 1;

    return 0;    
}

int sendInfoFrame(int fd, int ns, char* info) {
    int res;
    char* infoFrame;
    int infoLength = strlen(info);
    char bcc2 = 0x0;

    infoFrame[0] = FLAG;
    infoFrame[1] = SEND_REC;
    infoFrame[2] = ns << 6;
    infoFrame[3] = SEND_REC ^ infoFrame[2];
    infoFrame[4] = 0x02;

    for (int i = 0; i < infoLength; i++) {
        infoFrame[i + 5] = info[i];
        bcc2 = bcc2 ^ infoFrame[i + 5];
    }

    infoFrame[infoLength + 5] = 0x03;
    infoFrame[infoLength + 6] = bcc2;
    infoFrame[infoLength + 7] = FLAG;

    res = write(fd, infoFrame, 8+infoLength);

    if(res < 1) {
        printf("Could not send Information FRAME!\n");
        return -1;
    }

    printf("Information message sent! %d bytes written\n", res);

    return 0;
}

int receiveInfoFrame (int fd, int ns, char* info) {
    char buf[255];
    int res;
    int i = 0;

    State state = START;


    while (state != STOP) {       /* loop for input */
        res = read(fd, buf, 1);   /* returns after 1 chars have been input */

        if(res == 0) continue;
        if(res < 0) return -1;

        printf("%c\n", buf[0]);
        fflush(stdin);

        if (state == DATA) {
            info[i] = buf[0];
            i++;
        }
        changeStateInfo(&state, ns, buf[0]); 
    }
    
    return 0;    
}

char* makeControlPacket(int control, int fileSize, char* fileName) {
    char* packet;
    int index = 0;
    
    packet[index++] = control;

    packet[index++] = FILESIZE;
    packet[index++] = sizeof(fileSize);

    for (int i = 0; i < packet[2]; i++) {
        packet[index++] = ((unsigned char*) fileSize)[i];
    }
    
    packet[index++] = FILENAME;
    packet[index++] = sizeof(fileName)/sizeof(char); //confirmar se é ou não char*
    
    for (int j = 0; j < packet[index -1]; j++) {
        packet[index++] = fileName[j];
    }
    
    return packet;
}
