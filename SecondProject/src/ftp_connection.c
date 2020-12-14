#include "ftp_connection.h"

int ftp_connect(int port, char *ip_address) { //after successful connection, server sends a line of welcome text with 220 as code to indicate the ready state 

    int socket_fd = socket_establish_connection(port, ip_address);
    if(socket_fd < 0) return -1;

    /* Receives the presentation message from the server */
    char response[MAX_LINE_SIZE];

    int status_code = read_response(socket_fd, response);

    if(status_code < 0) return -1; 

    if(status_code != CMD_READY_STATE) {
        fprintf(stderr, "Server is not ready!\n");
        return -1;
    }

    memset(response, 0, MAX_LINE_SIZE);
    return socket_fd;    
}

int ftp_login(int socket_fd, char *username, char *password) { 

    /*
    > user anonymous
    < 331 Password required for euproprio.
    > pass qualquer-password
    < 230 User anonymouslogged in. 
    */
    char user_command[MAX_LINE_SIZE];
    sprintf(user_command, "USER %s%s", username, CMD_TERMINATOR);

    char user_response[MAX_LINE_SIZE];
    if (send_command_receive_response(socket_fd, user_command, CMD_USERNAME_CORRECT, user_response) < 0) return -1;
    
    char pass_command[MAX_LINE_SIZE];
    sprintf(pass_command, "PASS %s%s", password, CMD_TERMINATOR);

    char pass_response[MAX_LINE_SIZE];
    if (send_command_receive_response(socket_fd, pass_command, CMD_LOGIN_CORRECT, pass_response) < 0) return -1;

    
    memset(user_command, 0, MAX_LINE_SIZE);
    memset(user_response, 0, MAX_LINE_SIZE);
    memset(pass_command, 0, MAX_LINE_SIZE);
    memset(pass_response, 0, MAX_LINE_SIZE);

    return 0;
}

int ftp_passive_mode(int socket_fd) {

    /*> pasv
    < 227 Entering Passive Mode (193,136,28,12,19,91)
    */

    char pasv_command[MAX_LINE_SIZE];
    sprintf(pasv_command, "PASV%s", CMD_TERMINATOR);

    char response[MAX_LINE_SIZE];
    if (send_command_receive_response(socket_fd, pasv_command, CMD_PASSIVE_MODE, response) < 0) return -1;

    memset(pasv_command, 0, MAX_LINE_SIZE);
    
    char * response_values = strrchr(response, '(');

    int ip1, ip2, ip3, ip4, port1, port2;
    if (sscanf(response_values, "(%d,%d,%d,%d,%d,%d)", &ip1, &ip2, &ip3, &ip4, &port1, &port2) != 6){
        fprintf(stderr, "Error parsing ip address and port number\n");
        return -1;
    }

    //gets the port number
    int data_port = port1 * 256 + port2;

    //gets ip address
    char ip_address[MAX_STRING_SIZE];
    sprintf(ip_address, "%d.%d.%d.%d", ip1, ip2, ip3, ip4);

    //connects to the data port
    int data_fd = socket_establish_connection(data_port, ip_address);
    
    if(data_fd < 0) return -1;

    memset(ip_address, 0, MAX_STRING_SIZE);
    memset(response, 0, MAX_LINE_SIZE);

    return data_fd;
}

int ftp_request_file(int socket_fd, const char*path) {

    /*
        > retr path
        < ... sending file
    */

    char retr_command[MAX_LINE_SIZE];
    sprintf(retr_command, "RETR %s%s", path, CMD_TERMINATOR);
    if(send_command(socket_fd, retr_command) < 0) return -1;

    memset(retr_command, 0, MAX_LINE_SIZE);
    return 0;
}

int ftp_download_file(int data_fd, const char *path, const char *filename) { 

    int fd = open(filename, O_WRONLY | O_CREAT, 0666);
    if(fd < 0) {
        fprintf(stderr, "Error opening file!\n");
        return -1;
    }

    char buf[MAX_LINE_SIZE];
    int bytes;

    while((bytes = read(data_fd, buf, MAX_LINE_SIZE)) > 0){
        if(write(fd, buf, bytes) < bytes){                 //write the information that server is sending to the created file
            fprintf(stderr, "Error writing to file!\n");
            close(fd);
            return -1;
        }
    }


    memset(buf, 0, MAX_LINE_SIZE);
    
    if(close(fd) < 0){
        fprintf(stderr, "Error closing file!\n");
        return -1;
    }

    return 0;
}