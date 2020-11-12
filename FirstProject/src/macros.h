#ifndef MACROS_H
#define MACROS_H

#pragma once

#define BAUDRATE        B38400
#define MODEMDEVICE     "/dev/ttyS1"
#define _POSIX_SOURCE   1 /* POSIX compliant source */
#define _XOPEN_SOURCE   700
#define FALSE           0
#define TRUE            1
#define ATTEMPTS        4

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
#define UA_ANSWER       0x07
#define RR_ANSWER(R)    ((R == 0) ?  0x05 : 0x85)
#define REJ_ANSWER(R)   ((R == 0) ?  0x01 : 0x81)

//I Frames
#define IFRAME_SIZE     512
#define DATA_MAX_SIZE   (IFRAME_SIZE - 6)
#define NS(S)           ((S == 0) ?  0x00 : 0x40)
#define ESCAPE          0x7D
#define STUFF_BYTE      0x20

//Packet
#define MAX_PACKET_SIZE (DATA_MAX_SIZE / 2)
#define MAX_K           (MAX_PACKET_SIZE - 4)
#define MAX_VALUE_SIZE  255

#define FILESIZE        0x00
#define FILENAME        0x01

#define DATA_BYTE       0x01
#define START_BYTE      0x02
#define END_BYTE        0x03


#endif /*MACROS_H*/