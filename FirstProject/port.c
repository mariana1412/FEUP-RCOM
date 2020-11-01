#include "port.h"

static struct termios oldtioReceiver, oldtioSender;

int initPort(int portInt, int vtime, int vmin, int status){
    
    char* port = (char *)malloc(11);
    if (port == NULL) {
        printf("Could not allocate memory for port!\n");
        return -1;
    }
    struct termios newtio;

    sprintf(port, "/dev/ttyS%d", portInt);
    
    int fd = open(port, O_RDWR | O_NOCTTY);
    if (fd < 0) {
        perror(port); 
        return -1; 
    }

    /*save current port settings*/
    if((status == SENDER && (tcgetattr(fd, &oldtioSender) == -1)) || (status == RECEIVER && (tcgetattr(fd, &oldtioReceiver) == -1))) {
        perror("tcgetattr");
        return -1;
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = vtime;
    newtio.c_cc[VMIN]     = vmin;

    /* 
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a 
    leitura do(s) pr�ximo(s) caracter(es)
    */

    tcflush(fd, TCIOFLUSH);

    if (tcsetattr(fd, TCSANOW, &newtio) == -1) {
        perror("tcsetattr");
        return -1;
    }

    printf("New termios structure set\n");

    return fd;
}

int closePort(int fd, int status){
    sleep(1);

    if((status == SENDER && (tcsetattr(fd, TCSANOW, &oldtioSender) == -1)) || (status == RECEIVER && (tcsetattr(fd, TCSANOW, &oldtioReceiver) == -1))) {
        perror("tcsetattr");
        return -1;
    }

    close(fd);
    printf("Closed with success!\n");
    return 0;
}