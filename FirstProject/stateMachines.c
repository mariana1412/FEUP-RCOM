#include "stateMachines.h"

unsigned char bcc2Check = 0x00;
unsigned char answer;
static int escaped = FALSE;
static int s = 0;
static int rej = FALSE;
static int dataIndex = 0;

int changeStateS(State *state, unsigned char byte, ControlCommand command, unsigned char address)
{
    int isCorrect;

    switch (*state)
    {
    case START:
        if (byte == FLAG)
        {
            *state = FLAG_RCV;
        }
        break;

    case FLAG_RCV:
        if (byte == address)
        {
            *state = A_RCV;
        }
        else if(byte != FLAG)
        {
            *state = START;
        }

        break;

    case A_RCV:
        isCorrect = FALSE;

        switch (command)
        {
        case SET:
            if (byte == SET_COMMAND)
                isCorrect = TRUE;
            break;
        case DISC:
            if (byte == DISC_COMMAND)
                isCorrect = TRUE;
            break;
        case UA:
            if (byte == UA_ANSWER)
                isCorrect = TRUE;
            break;
        }

        if (isCorrect)
        {
            *state = C_RCV;
        }
        else if (byte == FLAG)
        {
            *state = FLAG_RCV;
        }
        else
        {
            *state = START;
        }

        break;

    case C_RCV:
        isCorrect = FALSE;

        switch (command)
        {
        case SET:
            if (byte == (address ^ SET_COMMAND))
                isCorrect = TRUE;
            break;
        case DISC:
            if (byte == (address ^ DISC_COMMAND))
                isCorrect = TRUE;
            break;
        case UA:
            if (byte == (address ^ UA_ANSWER))
                isCorrect = TRUE;
            break;
        }

        if (isCorrect)
        {
            *state = BCC_OK;
        }
        else if (byte == FLAG)
        {
            *state = FLAG_RCV;
        }
        else
        {
            *state = START;
        }
        break;

    case BCC_OK:
        if (byte == FLAG)
        {
            *state = STOP;
        }
        else
        {
            *state = START;
        }
        break;
    }
    return 0;
}

int changeStateInfo(State *state, unsigned char byte, int fd)
{
    switch (*state)
    {
    case START:
        if (byte == FLAG)
        {
            *state = FLAG_RCV;
        }
        break;

    case FLAG_RCV:
        if (byte == SEND_REC)
        {
            *state = A_RCV;
        }
        else if(byte != FLAG)
        {
            *state = START;
        }

        break;

    case A_RCV:
        if (byte == NS(s))
        {
            *state = C_RCV;
            return s;
        }
        else if (byte == NS(1 - s))
        {
            *state = C_RCV;
            s = 1 - s;
            return s;
        }
        else if (byte == FLAG)
        {
            *state = FLAG_RCV;
        }
        else
        {
            *state = START;
        }

        break;

    case C_RCV:
        if (byte == SEND_REC ^ NS(s))
        {
            dataIndex = 0;
            bcc2Check = 0x00;
            *state = DATA;
        }
        else if (byte == FLAG)
        {
            *state = FLAG_RCV;
        }
        else
        {
            *state = START;
        }
        break;
    case DATA:
        dataIndex++;
        if (dataIndex < DATA_MAX_SIZE)
        {
            if (escaped)
            {
                if (byte == (FLAG ^ STUFF_BYTE))
                {
                    bcc2Check = bcc2Check ^ FLAG;
                }
                else if (byte == (ESCAPE ^ STUFF_BYTE))
                {
                    bcc2Check = bcc2Check ^ ESCAPE;
                }
                escaped = FALSE;
            }
            else if (byte == FLAG)
            {
                *state = FLAG;
            }
            else if (byte == ESCAPE)
            {
                escaped = TRUE;
            }
            else
            {
                bcc2Check = bcc2Check ^ byte;
            }
        }
        else
        {
            *state = C2_RCV;
        }
        break;
    case C2_RCV:

        if (byte == bcc2Check)
        {
            rej == FALSE;
            *state = BCC2_OK;
        }
        else if (byte == FLAG)
        {
            *state = FLAG;
        }
        else
        {
            rej = TRUE;
            *state = START;
        }
        break;
    case BCC2_OK:
        if (byte == FLAG)
        {
            *state = STOP;
        }
        else
        {
            *state = START;
        }
        break;
    }

    return -1;
}

int changeStateAck(AckState *state, unsigned char byte)
{
    int isCorrect;
    int nr = -1;

    switch (*state)
    {
    case START_ACK:
        if (byte == FLAG)
        {
            *state = FLAG_ACK;
        }
        break;

    case FLAG_ACK:
        if (byte == SEND_REC)
        {
            *state = A_ACK;
        }
        else if(byte != FLAG)
        {
            *state = START_ACK;
        }

        break;

    case A_ACK:
        isCorrect = FALSE;

        switch (byte)
        {
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

        if (isCorrect)
        {
            answer = byte;
            *state = ACK_RCV;
        }
        else if (byte == FLAG)
        {
            *state = FLAG_ACK;
        }
        else
        {
            *state = START_ACK;
        }

        break;

    case ACK_RCV:
        if (byte == (SEND_REC ^ answer))
        {
            *state = BCC_ACK;
        }
        else if (byte == FLAG)
        {
            *state = FLAG_ACK;
        }
        else
        {
            *state = START_ACK;
        }
        break;

    case BCC_ACK:
        if (byte == FLAG)
        {
            *state = STOP_ACK;
        }
        else
        {
            *state = START_ACK;
        }
        break;
    }
    return nr;
}

int getREJ()
{
    return rej;
}