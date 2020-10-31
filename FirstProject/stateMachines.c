#include "stateMachines.h"

unsigned char bcc2Check = 0x0;
static int started = FALSE;
static int finished = FALSE;

static File *file;

int changeStateS(State *state, unsigned char byte, ControlCommand command, unsigned char address){
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
        else if(byte == address){
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
            case DISC:
                if (byte == DISC_COMMAND) isCorrect = TRUE;
                break;
            case UA:
                if(byte == UA_COMMAND) isCorrect = TRUE;
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
            printf("state = a_rcv -> set, disc or ua!!! \n");
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
                if (byte == (address ^ SET_COMMAND)) isCorrect = TRUE;
                break;
            case DISC:
                if (byte == (address ^ DISC_COMMAND)) isCorrect = TRUE;
                break;
            case UA:
                if(byte == (address ^ UA_COMMAND)) isCorrect = TRUE;
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

int changeStateInfo(State *state, unsigned char byte, int fd) {
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
        int s = 0;
        if(byte == NS(s) || (byte == NS(++s))){
            printf("state = c_rcv -> bcc_ok !!! ns = %d\n", s);
            *state = BCC_OK;
            return s;
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
        if (byte == FLAG) {
            printf("state = data -> flag!!! \n");
            *state = FLAG;
        } else { 
            printf("received data byte!!! \t");
            bcc2Check = bcc2Check ^ byte;

            switch (byte){

            case (START_BYTE):
                if (started == FALSE && (finished == FALSE)){
                    printf("START -> Called function to read control packet!!! \n");
                    if(readControlPacket(fd) == -1){
                        *state = START;
                        printf("something went wrong -> readControlPacket\n");
                    }
                    else started = TRUE;
                }
                else {
                    printf("Something is wrong: start/end byte!!! \n");
                    started = TRUE;
                    *state = START;
                }

            break;

            case (DATA_BYTE):
                if(started == FALSE){
                    printf("Data -> Haven't received start byte yet!\n");
                    *state = START;
                }
                else { //TO DO: verificar se o N está a 0 ou não
                    printf("Called function to read data packet!!! \n");
                    started = TRUE;
                    
                }

            break;

            case (END_BYTE):
                if(started == FALSE){
                    printf("End -> Haven't received start byte yet!\n");
                    *state = START;
                }
                else if(checkControlPacket(fd) == 0){
                    printf("state = data -> c2_rcv!!! \n");
                    finished = TRUE;
                    *state = C2_RCV;
                }
                else {
                    printf("Something is wrong: end byte!!! \n");
                    finished = TRUE;
                    *state = START;
                }
            break;
            }
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
    
    return -1;
}

int readControlPacket(int fd){
    unsigned char buf[255];
    int i = 0;
    int res;
    int l;

    //reads file size
    res = read(fd, buf, 1);
    if(res != 1) return -1;

    if(buf[0] != 0) return 1;
    file->controlPacket[i++] = buf[0];
    
    res = read(fd, buf, 1);
    if(res != 1) return -1;

    l = buf[0];
    file->controlPacket[i++] = buf[0];
    
    int j = 0;
    
    while (j < l) {
        res = read(fd, buf, 1);
        if(res != 1) return -1;
        file->size = file->size*256 + atoi(buf[0]);
        file->controlPacket[i++] = buf[0];
    }
    
    //reads file name
    res = read(fd, buf, 1);
    if(res != 1) return -1;

    if(buf[0] != 1) return 1;
    file->controlPacket[i++] = buf[0];
    
    res = read(fd, buf, 1);
    if(res != 1) return -1;

    l = buf[0];
    file->controlPacket[i++] = buf[0];
    
    j = 0;
    while (j < l) {
        res = read(fd, buf, 1);
        if(res != 1) return -1;

        file->name[j++] = buf[0];
        file->controlPacket[i++] = buf[0];
    }

    file->lastIndex = 0;
    printf("start -> control packet is correct!\n");
    return 0;
}

int checkControlPacket(int fd){

    int length = sizeof(file->controlPacket)/sizeof(char*); //verificar i'm not sure
    int res;
    unsigned char buf[255];

    for(int i = 0; i < length; i++){
        res = read(fd, buf, 1);
        if(res != 1) return -1;
        if(buf[0] != file->controlPacket[i]) return -1;
    }

    printf("end -> control packet is correct!\n");
    return 0;
}

int readDataPacket(int fd){
    int res;
    unsigned char buf[255];

    res = read(fd, buf, 1);
    if(res != 1) return -1;
    int N = buf[0];

    res = read(fd, buf, 1);
    if(res != 1) return -1;
    int L2 = buf[0];

    res = read(fd, buf, 1);
    if(res != 1) return -1;
    int L1 = buf[0];

    int K = 256*L2 + L1;

    for(int i = 0; i < K; i++){
        if(file->lastIndex == file->size) return -1;
        res = read(fd, buf, 1);
        if(res != 1) return -1;
        file->data[file->lastIndex++] = buf[0]; 
    }

    return 0;
}

File* getFile(){
    return file;
}