#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include "macros.h"
#include "dataStructures.h"
#include "stateMachines.h"

extern int alarmSender;
extern int alarmReceiver;

/**
 * @brief Catches a specific signal and the value of alarmSender becomes 0 
*/
void alarmSenderHandler();


/**
 * @brief Catches a specific signal and the value of alarmReceiver becomes 0 
*/
void alarmReceiverHandler();

/**
 * @brief Sends a message and waits for response, using a Stop and Wait mechanism
 * 
 * @param fd the file descriptor of port
 * @param send the command to be sent
 * @param sendAddress the address to be sent
 * @param receive the command to be received
 * @param recAddress the address to be received
 * 
 * @return 0 on success; -1 on error
 */
int SandWOpenClose(int fd, ControlCommand send, char sendAddress, ControlCommand receive, char recAddress);

/**
 * @brief Creates and sends a frame (SET, DISC or UA)
 * 
 * @param fd the file descriptor of port
 * @param command the command to be sent
 * @param address the address to be sent
 * 
 * @return 0 on success; -1 on error
 */
int sendOpenCloseFrame(int fd, ControlCommand command, int address);

/**
 * @brief Creates and sends a frame ACK
 * 
 * @param fd the file descriptor of port
 * @param send the command to be sent
 * @param r the required message (0/1)
 * 
 * @return 0 on success; -1 on error
 */
int sendAckFrame(int fd, ControlCommand command, int r);

/**
 * @brief Receives a SET/DISC/UA frame and calls the function with the state machine
 * 
 * @param fd the file descriptor of port
 * @param command the command to be received
 * @param address the address to be received
 * 
 * @return 0 on success; -1 on error
 */
int receiveOpenCloseFrame(int fd, ControlCommand command, int address);

/**
 * @brief Receives an ACK frame and calls the function with the state machine
 * 
 * @param fd the file descriptor of port
 * @param ns the expected message (0/1)
 * 
 * @return 1 if message was rejected; 0 on success; -1 on error
 */
int receiveAckFrame(int fd, int ns);

/**
 * @brief Creates and sends a info frame
 * 
 * @param fd the file descriptor of port
 * @param info the info to be sent
 * @param length the size of the info to be sent
 * 
 * @return 0 on success; -1 on error
 */
int sendInfoFrame(int fd, int ns, unsigned char *info, int length);

/**
 * @brief Receives an info frame and calls the function with the state machine
 * 
 * @param fd the file descriptor of port
 * @param info the info received
 * @param expectedNS the expected sequence number 
 * 
 * @return 1 to send a REJ message; 0 to send a RR message; -1 on error; -2 on timeout; -3 if it's a duplicate
 */
int receiveInfoFrame(int fd, unsigned char *info, int expectedNS);

/**
 *@brief generates random errors at a certain rate
 * 
 * 
 *@return 1 if error, 0 otherwise
 */
int getRandomError(int percentage);

#endif /*PROTOCOL_H*/
