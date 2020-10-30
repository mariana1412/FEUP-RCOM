#include "stateMachines.h"

char bcc2Check = 0x0;

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