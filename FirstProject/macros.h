#ifndef MACROS_H
#define MACROS_H

#define BAUDRATE        B38400
#define MODEMDEVICE     "/dev/ttyS1"
#define _POSIX_SOURCE   1 /* POSIX compliant source */
#define FALSE           0
#define TRUE            1

//PORTS
#define COM0            0    
#define COM1            1
#define COM10           10
#define COM11           11

//STATUS
#define SENDER          0
#define RECEIVER        1

//S or U Frames
#define S_FRAME_SIZE    5
#define FLAG            0x7E

#define SEND_REC        0X03 //commands from sender, answers from receiver
#define REC_SEND        0X01 //commands from receiver, answers from sender

#define SET_COMMAND     0x03
#define DISC_COMMAND    0x0B
#define UA_COMMAND      0x07
#define RR(R)           R << 7 + 0x05  
#define REJ(R)          R << 7 + 0x01  

//Control Packet
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

#endif /*MACROS_H*/