/*Non-Canonical Input Processing*/


#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "protocol.h"


void alarmHandler(){
  alarmSender = 0;   
}


int main(int argc, char** argv) {
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

  fd = open(argv[1], O_RDWR | O_NOCTTY );
  if (fd < 0) {
    perror(argv[1]); 
    exit(-1); 
  }

  if (tcgetattr(fd, &oldtio) == -1) { /* save current port settings */
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

  /* 
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a 
    leitura do(s) pr�ximo(s) caracter(es)
  */

  tcflush(fd, TCIOFLUSH);

  if (tcsetattr(fd, TCSANOW, &newtio) == -1) {
    perror("tcsetattr");
    exit(-1);
  }

  printf("New termios structure set\n");

  (void) signal(SIGALRM, alarmHandler); 
  int alarmStop = TRUE;
  int recUA;

  for(int i = 0; i < 4; i++){

    if(sendSetFrame(fd) == -1){
      close(fd);
      exit(-1);
    }

    alarmSender = 1;
    alarm(3);

    while(alarmSender){  
      recUA = receiveUAFrame(fd);
      if(recUA == 0){ 
        alarm(0);
        alarmStop = FALSE;
        break;
      } else if (recUA == -1) {
        printf("Could not read from port!\n");
        close(fd);
        exit(-1);
      }
    }

    if(alarmStop == FALSE){ 
      printf("Received UA Frame with success!\n");
      break;
    } else if (i < 3) {
      printf("Timeout number %d, trying again...\n", i+1);
    }

  }

  if(alarmStop == TRUE){
    printf("Could not receive UA Frame!\n");
  } else {
    char* data = "my name is";
    char* control = makeControlPacket(2, 354, "chef");
    int ns = 0;
    int nr;
    int recRR;

    for(int j = 0; j < 4; j++) {
      
      if(sendInfoFrame(fd, ns, control) == -1){
        close(fd);
        exit(-1);
      }

      alarmSender = 1;
      alarm(3);

      while(alarmSender){
        recRR = receiveRRFrame(fd, &nr);
        if(recRR == 0){ 
          alarm(0);
          alarmStop = FALSE;
          break;
        } else if (recRR == -1) {
          printf("Could not read from port!\n");
          close(fd);
          exit(-1);
        }
      }

      if(alarmStop == FALSE){ 
        printf("Received UA Frame with success!\n");
        break;
      } else if (j < 3) {
        printf("Timeout number %d, trying again...\n", j+1);
      }
    }

    if (alarmStop == TRUE) {
      printf("Could not receive UA Frame!\n");
    } else {
      sendInfoFrame(fd, 0, data);

      
    }
  }

  closePort()
  return 0;
}

int closePort(int fd, struct termios oldtio){
    sleep(1);

    if (tcsetattr(fd, TCSANOW, &oldtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    close(fd);
}