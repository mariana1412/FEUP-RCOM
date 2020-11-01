#include "stateMachines.h"

unsigned char bcc2Check = 0x00;
unsigned char answer;
static int escaped = FALSE;
static int s = 0;
static int rej = FALSE;
static int dataIndex = 0;


int changeStateS(State *state, unsigned char byte, ControlCommand command, unsigned char address){
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
                if(byte == UA_ANSWER) isCorrect = TRUE;
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
        if(byte == (address ^ command)){
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

int changeStateInfo(State *state, unsigned char byte, int fd) {
    switch (*state) {
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
        if(byte == NS(s) || (byte == NS(++s))){
            printf("state = a_rcv -> c_rcv!!! s = %d, ns = %d\n", s, NS(s));
            *state = C_RCV;
            return s;
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
        if(byte == SEND_REC ^ NS(s)){
            printf("state = c_rcv -> bcc_ok !!! s = %d, ns = %d\n", s, NS(s));
            dataIndex = 0;
            *state = DATA;
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
    case DATA:
        if (dataIndex < DATA_MAX_SIZE) {
            dataIndex++;
            if (escaped) {
                bcc2Check = bcc2Check ^ byte;
                escaped = FALSE;
            } else if (byte == FLAG) {
                printf("state = data -> flag!!! \n");
                *state = FLAG;
            } else if (byte == ESCAPE) {
                escaped = TRUE;
            } else {
                bcc2Check = bcc2Check ^ byte;
            }
        }
        break;
    case C2_RCV:
        if (byte == bcc2Check) {
            printf("state = c2_rcv -> bcc2_ok!!! \n");
            rej == FALSE;
            *state = BCC2_OK;
        } else if (byte == FLAG) {
            printf("state = c2_rcv -> flag!!! \n");
            *state = FLAG;
        }
        else {
            printf("state = c2_rcv -> flag!!! \n");
            rej = TRUE;
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

int changeStateAck(AckState *state, unsigned char byte) {
    int isCorrect;
    int nr = -1;

    switch (*state) {
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

            switch (byte) {
                case RR_ANSWER(0):
                    nr = 0;
                    isCorrect = TRUE;
                    break;
                case RR_ANSWER(1):
                    nr = 1;
                    isCorrect = TRUE;
                    break;
                case REJ_ANSWER(0):
                    nr = 2;
                    isCorrect = TRUE;
                    break;
                case REJ_ANSWER(1):
                    nr = 3;
                    isCorrect = TRUE;
                    break;
            }

            if(isCorrect){
                answer = byte;
                printf("state = a_rcv -> rr or rej!!! \n");
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
            if(byte == (SEND_REC ^ answer)){
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

int getREJ(){
    return rej;
}