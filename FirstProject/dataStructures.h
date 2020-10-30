#ifndef DATASTRUCTURES_H
#define DATASTRUCTURES_H

#pragma once

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

typedef enum State{START, FLAG_RCV, A_RCV, C_RCV, BCC_OK, DATA, C2_RCV, BCC2_OK, STOP} State;
typedef enum ControlCommand{SET, DISC, UA, RR, REJ} ControlCommand;

#endif /*DATASTRUCTURES_H*/