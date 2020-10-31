#include "protocol.h"

int alarmSender = 1;

void alarmHandler(){
    alarmSender = 0; 
    return;  
}

int SandWOpenClose(int fd, ControlCommand send, ControlCommand receive) {
    int rec;

    for(int i = 0; i < 4; i++){
        if(sendOpenCloseFrame(fd, send, SEND_REC) == -1){
            printf("Could not send Frame!\n");
            return -1;
        } else {
            printf("Sent Frame!\n");
        }

        alarmSender = 1;
        alarm(3);

        while(alarmSender){
            rec = receiveOpenCloseFrame(fd, receive, SEND_REC);
            if(rec == 0){ 
                alarm(0);
                break;  
            } else if (rec < 0) {
                printf("Could not read from port! Code: %d\n", rec);
                return -1;
            }
        }

        if(alarmSender){
            return 0;
        }
        else if (i < 3) {
            printf("Timeout number %d, trying again...\n", i+1);
        }
    }
    return -1;
}

int sendOpenCloseFrame(int fd, ControlCommand command, int address) {
    int res;
    char frame[S_FRAME_SIZE];

    frame[0] = FLAG;
    frame[1] = address;
    frame[4] = FLAG;

    switch(command) {
        case SET:
            frame[2] = SET_COMMAND;
            frame[3] = address ^ SET_COMMAND;
            break;
        case DISC:
            frame[2] = DISC_COMMAND;
            frame[3] = address ^ DISC_COMMAND;
            break;
        case UA:
            frame[2] = UA_COMMAND;
            frame[3] = address ^ UA_COMMAND;
            break;
        default:
            break;
    }

    res = write(fd, frame, S_FRAME_SIZE);

    if (res == -1) return -1;
    
    return 0;
}

int receiveOpenCloseFrame(int fd, ControlCommand command, int address) {
    char buf[255];
    int res;

    State state = START;

    while (state != STOP && alarmSender == 1) {       
        res = read(fd, buf, 1);   
        if(res == 0) continue;
        if(res < 0) return -1;
              
        printf("%d\n", buf[0]);

        changeStateS(&state, buf[0], command, address);     
    }

    return 0;
}

int sendInfoFrame(int fd, int ns, char* info) {
    int res;
    unsigned char* infoFrame;
    int infoLength = strlen(info);
    unsigned char bcc2 = 0x0;

    infoFrame[0] = FLAG;
    infoFrame[1] = SEND_REC;
    infoFrame[2] = ns << 6;
    infoFrame[3] = SEND_REC ^ infoFrame[2];

    for (int i = 0; i < infoLength; i++) {
        infoFrame[i + 4] = info[i];
        bcc2 = bcc2 ^ infoFrame[i + 4];
    }

    infoFrame[infoLength + 4] = bcc2;
    infoFrame[infoLength + 5] = FLAG;

    res = write(fd, infoFrame, 8+infoLength);

    if(res < 1) {
        printf("Could not send Information FRAME!\n");
        return -1;
    }

    printf("Information message sent! %d bytes written\n", res);

    return 0;
}

int receiveInfoFrame(int fd, int ns) {
    unsigned char buf[255];
    int res;
    int i = 0;

    State state = START;

    while (state != STOP) {       /* loop for input */
        res = read(fd, buf, 1);   /* returns after 1 chars have been input */

        if(res == 0) continue;
        if(res < 0) return -1;

        printf("%c\n", buf[0]);

        changeStateInfo(&state, ns, buf[0], fd); 
    }
    
    return 0;    
}

unsigned char* makeControlPacket(unsigned char control, int fileSize, char* fileName) {
    unsigned char* packet;
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

unsigned char* makeDataPacket(char *info, int N){
    unsigned char* packet;
    int index = 0;

    packet[index++] = DATA_BYTE;
    packet[index++] = N;

    int length = sizeof(info)/sizeof(char *);

    packet[index++] = length/256;
    packet[index++] = length%256;

    for(int i = 0; i < length; i++){
        packet[index++] = info[i];
    }

    return 0;
}
