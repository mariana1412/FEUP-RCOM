#ifndef MACROS_H
#define MACROS_H

#define BAUDRATE        B38400
#define MODEMDEVICE     "/dev/ttyS1"
#define _POSIX_SOURCE   1 /* POSIX compliant source */
#define FALSE           0
#define TRUE            1

#define SENDER          0
#define RECEIVER        1

#define FLAG            0x7E
#define SEND_REC        0X03 //commands from sender, answers from receiver
#define REC_SEND        0X01 //commands from receiver, answers from sender
#define SET_COMMAND     0x03
#define UA_ANSWER       0x07
#define S_FRAME_SIZE    5

#define RR(R)           R << 7 + 0x05  
#define REJ(R)          R << 7 + 0x01  

#define FILESIZE        0
#define FILENAME        1

struct AppLayer {
    int fd;
    int status;
} AppLayer;

struct Frame_S {
    char flag;
    char address;
    char command;
    char bcc;
} Frame_S;

extern int alarmSender;
extern int alarmReceiver;

#endif /*MACROS_H*/