/*Non-Canonical Input Processing*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>
#include <string.h>
#include "protocol.h"

int main(int argc, char** argv){

  int fd, c;
  struct termios oldtio, newtio;

  if ( (argc < 2) || ((strcmp("/dev/ttyS10", argv[1])!=0) && (strcmp("/dev/ttyS11", argv[1])!=0)) || ((strcmp("/dev/ttyS0", argv[1])!=0) && (strcmp("/dev/ttyS1", argv[1])!=0))) {
    printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
    exit(1);
  }


  /*
  Open serial port device for reading and writing and not as controlling tty
  because we don't want to get killed if linenoise sends CTRL-C.
  */


  fd = open(argv[1], O_RDWR | O_NOCTTY );
  if (fd < 0) {perror(argv[1]); exit(-1); }

  if (tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
    perror("tcgetattr");
    exit(-1);
  }

  bzero(&newtio, sizeof(newtio));
  newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
  newtio.c_iflag = IGNPAR;
  newtio.c_oflag = 0;

  /* set input mode (non-canonical, no echo,...) */
  newtio.c_lflag = 0;

  newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
  newtio.c_cc[VMIN]     = 1;   /* blocking read until 5 chars received */



  /* 
  VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a 
  leitura do(s) pr�ximo(s) caracter(es)
  */


  tcflush(fd, TCIOFLUSH);

  if ( tcsetattr(fd, TCSANOW, &newtio) == -1) {
    perror("tcsetattr");
    exit(-1);
  }

  printf("New termios structure set\n");

  if (receiveSetFrame(fd) == -1) {
    printf("The SET Frame received is wrong!\n");
    exit(-1);
  }

  if(sendUAFrame(fd) == -1){
    printf("Could not send UA FRAME!\n");
    exit(-1);
  }

  sleep(1);
  tcsetattr(fd, TCSANOW, &oldtio);
  close(fd);
  return 0;
}
