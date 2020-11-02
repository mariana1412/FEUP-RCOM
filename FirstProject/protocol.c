#include "protocol.h"

int alarmSender = 1;

int overallIndex = 0;

void alarmHandler()
{
    alarmSender = 0;
    return;
}

int SandWOpenClose(int fd, ControlCommand send, ControlCommand receive)
{
    int rec;

    for (int i = 0; i < 4; i++)
    {
        if (sendOpenCloseFrame(fd, send, SEND_REC) == -1)
        {
            printf("Could not send Frame! Attempt number %d\n", i + 1);
            return -1;
        }
        else
        {
            printf("Sent Frame with success! Attempt number %d\n", i + 1);
        }

        alarmSender = 1;
        alarm(3);

        while (alarmSender)
        {
            rec = receiveOpenCloseFrame(fd, receive, SEND_REC);
            if (rec == 0)
            {
                alarm(0);
                break;
            }
            else if (rec < 0)
            {
                printf("Could not read from port! Code: %d\n", rec);
                return -1;
            }
        }

        if (alarmSender)
        {
            return 0;
        }
        else if (i < 3)
        {
            printf("Timeout number %d, trying again...\n", i + 1);
        }
    }
    return -1;
}

int sendOpenCloseFrame(int fd, ControlCommand command, int address)
{
    int res;
    unsigned char frame[S_FRAME_SIZE];

    frame[0] = FLAG;
    frame[1] = address;
    frame[4] = FLAG;

    switch (command)
    {
    case SET:
        frame[2] = SET_COMMAND;
        frame[3] = address ^ SET_COMMAND;
        break;
    case DISC:
        frame[2] = DISC_COMMAND;
        frame[3] = address ^ DISC_COMMAND;
        break;
    case UA:
        frame[2] = UA_ANSWER;
        frame[3] = address ^ UA_ANSWER;
        break;

    default:
        break;
    }

    res = write(fd, frame, S_FRAME_SIZE);
    if (res == -1)
        return -1;
    return 0;
}

int sendAckFrame(int fd, ControlCommand command, int r)
{
    int res;
    unsigned char frame[S_FRAME_SIZE];

    frame[0] = FLAG;
    frame[1] = SEND_REC;
    frame[4] = FLAG;

    switch (command)
    {
    case RR:
        frame[2] = RR_ANSWER(r);
        frame[3] = RR_ANSWER(r) ^ SEND_REC;
        break;
    case REJ:
        frame[2] = REJ_ANSWER(r);
        frame[3] = REJ_ANSWER(r) ^ SEND_REC;
        break;
    default:
        break;
    }

    res = write(fd, frame, S_FRAME_SIZE);
    if (res == -1)
        return -1;
    return 0;
}

int receiveOpenCloseFrame(int fd, ControlCommand command, int address)
{
    unsigned char buf[255];
    int res;

    State state = START;

    while (state != STOP && alarmSender == 1)
    {
        res = read(fd, buf, 1);
        if (res == 0)
            continue;
        if (res < 0)
        {
            perror("-----------> ");
            return -1;
        }

        printf("%d\n", buf[0]);

        changeStateS(&state, buf[0], command, address);
    }

    return 0;
}

int receiveAckFrame(int fd, int ns)
{
    unsigned char buf[255];
    int res;
    int nr;
    int acknowledged = -1;

    AckState state = START_ACK;

    while (state != STOP_ACK && alarmSender == 1)
    {
        res = read(fd, buf, 1);
        if (res == 0)
            continue;
        if (res < 0)
            return -1;

        printf("%d\n", buf[0]);

        nr = changeStateAck(&state, buf[0]);

        if (state == ACK_RCV)
        {
            if (nr == ns)
            {
                acknowledged = 0;
            }
            else if (nr - 2 == ns)
            {
                acknowledged = 1;
            }
        }
    }

    return acknowledged;
}

int sendInfoFrame(int fd, int ns, unsigned char *info, int length)
{
    int res, index = 0;
    unsigned char bcc2 = 0x00;

    unsigned char *infoFrame = (unsigned char *)malloc(IFRAME_SIZE);
    if (infoFrame == NULL)
    {
        printf("Could not allocate memory for infoFrame!\n");
        if (llclose(fd, SENDER) < 0)
        {
            printf("llclose failed\n");
            exit(1);
        }
        return 0;
    }

    infoFrame[index++] = FLAG;
    infoFrame[index++] = SEND_REC;
    infoFrame[index++] = NS(ns);

    infoFrame[index++] = SEND_REC ^ NS(ns);

    for (int i = 0; i < length; i++)
    {
        bcc2 = bcc2 ^ info[i];
        if (info[i] == FLAG || info[i] == ESCAPE)
        {
            infoFrame[index++] = ESCAPE;
            infoFrame[index++] = info[i] ^ STUFF_BYTE;
        }
        else
        {
            infoFrame[index++] = info[i];
        }
    }

    while (index < (IFRAME_SIZE - 2))
    {
        infoFrame[index++] = 0x00;
    }

    infoFrame[index++] = bcc2;
    infoFrame[index++] = FLAG;

    res = write(fd, infoFrame, index);

    if (res < 1)
        return -1;

    return 0;
}

int receiveInfoFrame(int fd, unsigned char *info)
{
    unsigned char buf[255];
    int res;
    int i = 0;
    int ns = -1;
    int firstTime = TRUE;
    int escaped = FALSE;

    State state = START;

    while (state != STOP)
    {
        res = read(fd, buf, 1);

        if (res == 0)
            continue;
        if (res < 0)
            return -1;

        printf("%d\n", buf[0]);

        int aux = changeStateInfo(&state, buf[0], fd);
        if (aux != -1)
            ns = aux;

        if (state == DATA)
        {
            if (firstTime)
                firstTime = FALSE;
            else
            {
                if (escaped)
                {
                    if (buf[0] == (FLAG ^ STUFF_BYTE))
                    {
                        info[i++] = FLAG;
                    }
                    else if (buf[0] == (ESCAPE ^ STUFF_BYTE))
                    {
                        info[i++] = ESCAPE;
                    }
                    escaped = FALSE;
                }
                else if (buf[0] == ESCAPE)
                {
                    escaped = TRUE;
                }
                else if (i < 509)
                {
                    printf("put in info: %d\n", buf[0]);
                    printf("i: %d\n", i);
                    info[i++] = buf[0];
                }
            }
        }
    }

    return ns;
}

int makeControlPacket(unsigned char control, int fileSize, unsigned char *fileName, unsigned char *packet)
{
    int index = 0;

    packet[index++] = control;

    packet[index++] = FILESIZE;
    packet[index++] = 0x08;

    unsigned char *n = (unsigned char *)malloc(8);
    sprintf(n, "%d", fileSize);

    for (int i = 0; i < sizeof(n); i++)
    {
        packet[index++] = n[i];
    }

    packet[index++] = FILENAME;
    packet[index++] = strlen(fileName);

    for (int j = 0; j < strlen(fileName); j++)
    {
        packet[index++] = fileName[j];
    }

    return index;
}

int makeDataPacket(unsigned char *info, int N, unsigned char *packet, int length)
{
    int index = 0;

    packet[index++] = DATA_BYTE;
    packet[index++] = N;

    packet[index++] = length / 256;
    packet[index++] = length % 256;

    for (int i = 0; i < length; i++)
    {
        printf("byte: %d, index: %d\n", info[i], overallIndex++);
        packet[index++] = info[i];
    }

    return index;
}
