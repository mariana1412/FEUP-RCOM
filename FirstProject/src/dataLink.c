#include "dataLink.h"

static int senderNS = 0;
static int receiverNS = 0;

int llopen(int port, int status)
{
    struct sigaction newAction, oldAction;

    newAction.sa_handler = alarmSenderHandler;
    sigemptyset(&newAction.sa_mask);
    newAction.sa_flags = 0;

    sigaction(SIGALRM, &newAction, &oldAction);

    int fd = initPort(port, 0, 0, status);

    int res;

    if (status == SENDER)
    {
        res = SandWOpenClose(fd, SET, SEND_REC, UA, SEND_REC);
        if (res == 0)
            return fd;
        else if (res == -1)
        {
            printf("Could not receive UA Frame!\n");
            return -1;
        }
    }
    else if (status == RECEIVER)
    {
        int recSet = receiveOpenCloseFrame(fd, SET, SEND_REC);

        if (recSet == 0)
        {
            printf("Received SET Frame with success!\n");
            res = sendOpenCloseFrame(fd, UA, SEND_REC);
            if (res == 0)
            {
                printf("Sent UA Frame!\n");
                return fd;
            }
            else
            {
                printf("Could not send UA Frame!\n");
            }
        }
        else if (recSet == -1)
        {
            printf("Could not read from port!\n");
        }
    }
    return -1;
}

int llwrite(int fd, char *buffer, int length)
{
    struct sigaction newAction, oldAction;

    newAction.sa_handler = alarmSenderHandler;
    sigemptyset(&newAction.sa_mask);
    newAction.sa_flags = 0;

    sigaction(SIGALRM, &newAction, &oldAction);

    int bytesSent;
    int response;

    for (int i = 0; i < ATTEMPTS; i++)
    {
        bytesSent = sendInfoFrame(fd, senderNS, buffer, length);
        if (bytesSent == -1)
        {
            printf("Could not send I Frame! Attempt number %d\n", i + 1);
            return -1;
        }
        else
        {
            printf("Sent I Frame! Attempt number %d\n", i + 1);
        }

        alarmSender = 1;
        alarm(3);

        while (alarmSender)
        {
            response = receiveAckFrame(fd, 1 - senderNS);
            if (response < 0)
            {
                printf("Could not read ACK Frame!\n");
            }
            else
            {
                alarm(0);
                break;
            }
        }

        if (alarmSender)
        {
            if (response == 0)
            {
                printf("ACK Frame received with success!\n");
                senderNS = 1 - senderNS;
                return bytesSent;
            }
            else if (i < 3)
            {
                printf("Rejected, trying again...\n");
            }
        }
        else if (i < 3)
        {
            printf("Timeout number %d, trying again...\n", i + 1);
        }
    }

    return -1;
}

int llread(int fd, char *buffer)
{
    int receive = receiveInfoFrame(fd, buffer, receiverNS);

    if (receive == 1) {
        printf("Sent REJ message!\n");
        sendAckFrame(fd, REJ, receiverNS);
        return -1;
    }
    else if (receive == 0)
    {
        printf("Sent RR message!\n");
        receiverNS = 1 - receiverNS;
        sendAckFrame(fd, RR, receiverNS);
        return IFRAME_SIZE;
    } else if (receive == -2) {
        return 0;
    } else return -1;

}

int llclose(int fd, int status)
{
    struct sigaction newAction, oldAction;

    newAction.sa_handler = alarmSenderHandler;
    sigemptyset(&newAction.sa_mask);
    newAction.sa_flags = 0;

    sigaction(SIGALRM, &newAction, &oldAction);

    int res;

    if (status == SENDER)
    {
        res = SandWOpenClose(fd, DISC, SEND_REC, DISC, REC_SEND);
        if (res == 0)
        {
            printf("Received DISC Frame with success!\n");
            res = sendOpenCloseFrame(fd, UA, REC_SEND);
            if (res == 0)
            {
                printf("Sent UA Frame!\n");
            }
            else
            {
                printf("Could not send UA Frame!\n");
                return -1;
            }
        }
        else if (res == -1)
        {
            printf("Could not receive DISC Frame!\n");
            return -1;
        }
    }
    else if (status == RECEIVER)
    {
        int recDISC = receiveOpenCloseFrame(fd, DISC, SEND_REC);

        if (recDISC == 0)
        {
            printf("Received DISC Frame with success!\n");
            res = SandWOpenClose(fd, DISC, REC_SEND, UA, REC_SEND);
            if (res == 0)
            {
                printf("Received UA Frame with success!\n");
            }
            else if (res == -1)
            {
                printf("Could not receive UA Frame!\n");
                return -1;
            }
        }
        else if (recDISC == -1)
        {
            printf("Could not read from port!\n");
            return -1;
        }
    }

    return closePort(fd, status);
}

