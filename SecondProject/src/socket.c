#include "socket.h"

int socket_establish_connection(int port, char *ip_address) { //clientTCP.c moodle

    int socketfd;
    struct sockaddr_in server_addr;

    /*server address handling*/
    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip_address); 
    server_addr.sin_port = htons(port);                  

    /*open an TCP socket*/
    if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Error on socket creation:");
        return -1;
    }

    /*connect to the server*/
    if (connect(socketfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Can't connect to server");
        return -1;
    }

    return socketfd;
}

int isDigit(char c){
    return (c >= '0' && c <= '9');
}

int read_response(int socket_fd, char* response) {
    FILE *socket_file = fdopen(socket_fd, "r"); //Open socket for reading

    if(socket_file == NULL) {
        perror("fdopen() failed:");
        return -1;
    }

    size_t bytes;
    char *buf = malloc(MAX_LINE_SIZE);
    
    while(getline(&buf, &bytes, socket_file) > 0){
        printf("%s", buf);
        if((buf[3] == ' ') && isDigit(buf[0]) && isDigit(buf[1]) && isDigit(buf[2])) break;
    }

    strcpy(response, buf);

    int response_code = atoi(buf);
    printf("code = -%d-\n", response_code);

    free(buf);
    return response_code;
}

int send_command(int socket_fd, char * command){

    if(write(socket_fd, command, strlen(command)) <= 0){
        fprintf(stderr, "Error writitng to socket!\n");
        return -1;
    }
    
    return 0;
}

int send_command_receive_response(int socket_fd, char* command, int response_code, char *response) {

    if(send_command(socket_fd, command) < 0) return -1;

    int code = read_response(socket_fd, response);
    
    if(code < 0) return -1;

    if(code != response_code) {
        fprintf(stderr, "Failed: Server sent a code that indicates error!\n");
        return -1;
    }

    return 0;
}