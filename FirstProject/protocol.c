#include "protocol.h"

volatile int STOP=FALSE;

int sendSetFrame(int fd){
    int res;
    char setFrame[S_FRAME_SIZE];

    setFrame[0] = FLAG;
    setFrame[1] = SEND_REC;
    setFrame[2] = SET_COMMAND;
    setFrame[3] = SEND_REC ^ SET_COMMAND;
    setFrame[4] = FLAG;

    res = write(fd, setFrame, S_FRAME_SIZE);

    if(res == -1) return -1;
    printf("Set message sent! %d bytes written\n", res);

    return 0;
}

int receiveSetFrame(int fd){
    char setFrame[S_FRAME_SIZE];
    char buf[255];
    int i = 0;
    int flag = 0;
    int res;

    while (STOP == FALSE) {       // loop for input 
        res = read(fd, buf, 1);   // returns after 1 chars have been input 
        buf[res] = 0;               // so we can printf... 

        printf("%d\n", buf[0]);

        setFrame[i] = buf[0];
        if (setFrame[i] == FLAG) flag++;
        i++;

        if (flag > 1) STOP = TRUE;
    }

    /* if (setFrame[2] != SET_COMMAND) return 1; perguntar ao stor sobre verificação */

    if (setFrame[3] != (setFrame[1] ^ setFrame[2])) return -1;

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

    if(res == -1) return -1;
    printf("UA message sent! %d bytes written\n", res);

    return 0;
}

int receiveUAFrame(int fd){
    char uaFrame[S_FRAME_SIZE];
    char buf[255];
    int flag = 0;
    int i = 0;
    int res;

    while (STOP == FALSE) {       /* loop for input */
        res = read(fd, buf, 1);   /* returns after 1 chars have been input */
        buf[res] = 0;               /* so we can printf... */

        printf("%d\n", buf[0]);

        uaFrame[i] = buf[0];
        if (uaFrame[i] == FLAG) flag++;
        i++;

        if (flag > 1) STOP=TRUE;
    }

    /* if (setFrame[2] != SET_COMMAND) return 1; perguntar ao stor sobre verificação */

    if (uaFrame[3] != (uaFrame[1] ^ uaFrame[2])) return -1;

    return 0;    
}