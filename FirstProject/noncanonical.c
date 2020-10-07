/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>

#include "macros.h"

volatile int STOP=FALSE;

int main(int argc, char** argv){

  int fd,c, res;
  struct termios oldtio,newtio;
  char buf[255];

  // if ( (argc < 2) || 
  //      ((strcmp("/dev/ttyS10", argv[1])!=0) &&
  //       (strcmp("/dev/ttyS11", argv[1])!=0) )) {
  //   printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS10\n");
  //   exit(1);
  // }


  /*
  Open serial port device for reading and writing and not as controlling tty
  because we don't want to get killed if linenoise sends CTRL-C.
  */


  fd = open(argv[1], O_RDWR | O_NOCTTY );
  if (fd <0) {perror(argv[1]); exit(-1); }

  if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
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

  if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
    perror("tcsetattr");
    exit(-1);
  }

  printf("New termios structure set\n");

  char set_frame[SET_FRAME_SIZE];
  int i = 0;
  int flag = 0;

  while (STOP==FALSE) {       /* loop for input */
    res = read(fd,buf,1);   /* returns after 1 chars have been input */
    buf[res]=0;               /* so we can printf... */

    printf("%d\n", buf[0]);

    set_frame[i] = buf[0];
    if (set_frame[i] == FLAG) flag++;
    i++;

    if (flag >1) STOP=TRUE;
  }



  /* 
  O ciclo WHILE deve ser alterado de modo a respeitar o indicado no gui�o 
  */

  char ua_frame[SET_FRAME_SIZE];
  ua_frame[0] = set_frame[0];
  ua_frame[1] = set_frame[1];
  ua_frame[2] = UA_ANSWER;
  ua_frame[3] = ua_frame[1] ^ ua_frame[2];
  ua_frame[4] = set_frame[4];

  res = write(fd, ua_frame, SET_FRAME_SIZE);
  printf("%d bytes written\n", res);

  sleep(1);
  tcsetattr(fd,TCSANOW,&oldtio);
  close(fd);
  return 0;
}
