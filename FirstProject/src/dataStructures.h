#ifndef DATASTRUCTURES_H
#define DATASTRUCTURES_H

#pragma once

/**
 * @brief Struct to save file's information
*/
typedef struct File
{
    long int size; 
    unsigned char *name;
    unsigned char *controlPacket; 
    int lastIndex;
    unsigned char *data;
} File;

/**
 * @brief State of the state machine
*/
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

/**
 * @brief state of the ack state machine
*/
typedef enum AckState
{
    START_ACK,
    FLAG_ACK,
    A_ACK,
    ACK_RCV,
    BCC_ACK,
    STOP_ACK
} AckState;

/**
 * Type of control command
*/
typedef enum ControlCommand
{
    SET,
    DISC,
    UA,
    RR,
    REJ
} ControlCommand;

#endif /*DATASTRUCTURES_H*/