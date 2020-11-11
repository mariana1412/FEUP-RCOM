#ifndef STATEMACHINES_H
#define STATEMACHINES_H

#include <stdio.h>
#include <string.h>
#include "macros.h"
#include "dataStructures.h"

/**
 * @brief Processes a byte of a SET/DISC/UA frame and updates the state
 * 
 * @param state the state of the packet
 * @param byte the byte to process
 * @param command the command of the packet
 * @param address the address of the packet
 * 
 */
void changeStateS(State *state, unsigned char byte, ControlCommand command, unsigned char address);

/**
 * @brief Processes a byte of an info frame and updates the state
 * 
 * @param state the state of the packet
 * @param byte the byte to process
 * 
 * @return the number of the expected message; -1 otherwise
 */
int changeStateInfo(State *state, unsigned char byte);

/**
 * @brief Processes a byte of an ACK frame and updates the state
 * 
 * @param state the state of the packet
 * @param byte the byte to process
 * 
 * @return the number of the expected message; -1 otherwise
 */
int changeStateAck(AckState *state, unsigned char byte);


#endif /*STATEMACHINES_H*/
