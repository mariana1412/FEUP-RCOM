#ifndef DATASTRUCTURES_H
#define DATASTRUCTURES_H

#pragma once

typedef struct File
{
    long int size;
    unsigned char *name;
    unsigned char *controlPacket; //sem start/end
    int lastIndex;
    unsigned char *data;
} File;

typedef enum State
{
    START,
    FLAG_RCV,
    A_RCV,
    C_RCV,
    BCC_OK,
    DATA,
    C2_RCV,
    BCC2_OK,
    STOP,
    IGNORE,
    REJECTED
} State;

typedef enum AckState
{
    START_ACK,
    FLAG_ACK,
    A_ACK,
    ACK_RCV,
    BCC_ACK,
    STOP_ACK
} AckState;

typedef enum ControlCommand
{
    SET,
    DISC,
    UA,
    RR,
    REJ
} ControlCommand;

#endif /*DATASTRUCTURES_H*/