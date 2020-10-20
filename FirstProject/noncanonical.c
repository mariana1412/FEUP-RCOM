/*Non-Canonical Input Processing*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>
#include <string.h>
#include "protocol.h"

void alarmHandler(){
  alarmReceiver = 0;   
}

int main(int argc, char** argv){
  int fd;
  struct termios oldtio, newtio;

  if ((argc < 2) || ((strcmp("/dev/ttyS10", argv[1])!=0) && (strcmp("/dev/ttyS11", argv[1])!=0) && (strcmp("/dev/ttyS0", argv[1])!=0) && (strcmp("/dev/ttyS1", argv[1])!=0))) {
    printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
    exit(1);
  }


  /*
  Open serial port device for reading and writing and not as controlling tty
  because we don't want to get killed if linenoise sends CTRL-C.
  */


  fd = open(argv[1], O_RDWR | O_NOCTTY);
  if (fd < 0) {
    perror(argv[1]);
    exit(-1); 
  }

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

  newtio.c_cc[VTIME]    = 1;   /* inter-character timer unused */
  newtio.c_cc[VMIN]     = 0;   /* blocking read until 5 chars received */

  tcflush(fd, TCIOFLUSH);

  if (tcsetattr(fd, TCSANOW, &newtio) == -1) {
    perror("tcsetattr");
    exit(-1);
  }

  printf("New termios structure set\n");

  (void) signal(SIGALRM, alarmHandler);
  int alarmStop = TRUE;
  int recSet;

  alarm(13);

  while(alarmReceiver) {
    recSet = receiveSetFrame(fd);
    if (recSet == 0) {
      printf("Received Set Frame with success!\n");
      alarm(0);
      alarmStop = FALSE;
      break;
    } else if (recSet == -1) {
      printf("Could not read from port!\n");
      close(fd);
      exit(-1);
    }
  }

  if (alarmStop == TRUE) {
    printf("Could not receive Set Frame!\n");
  } else {
    sendUAFrame(fd);
  }
  
  sleep(1);
  if (tcsetattr(fd, TCSANOW, &oldtio) == -1) {
    perror("tcsetattr");
    exit(-1);
  }

  close(fd);
  return 0;
}