#include <stdio.h>
#include "url.h"
#include "ftp_connection.h"

void printURL(url_struct *url ){
    printf("\nPRINTING URL INFO: \n");
    printf("user = %s\n", url->user);
    printf("password = %s\n", url->password);
    printf("host = %s\n", url->host);
    printf("url_path = %s\n", url->url_path);
    printf("filename = %s\n", url->filename);
    printf("ip address = %s\n\n", url->ip_address);
}

//wget ftp://rcom:rcom@netlab1.fe.up.pt/files/pic1.jpg -> faz download do ficheiro para confirmar que o programa fez download corretamente
//https://programmer.help/blogs/a-simple-ftp-client-implemented-in-c-language.html?fbclid=IwAR1Gvq05Fn62IegJO2PB68MaShMR8Vu4tW_FGYc2J_6DPRQ_XBRNsfLWAkc
//https://blog.pantuza.com/artigos/programando-um-protocolo-utilizando-sockets

int main(int argc, char *argv[]) {

    if (argc != 2) {  
        fprintf(stderr, "usage: ./download ftp://[<user>:<password>@]<host>/<url-path>\n");
        exit(1);
    }
    
    printf("Parsing URL...\n");

    url_struct *url = createUrlStruct();

    if(parse_file_url(argv[1], url) < 0){
        fprintf(stderr, "Url parser failed!\n");
        exit(1);
    }
    printf("URL is parsed!\n\n");

    printf("Getting IP address... \n");

    if(getIpAddress(url) < 0) exit(1);

    printf("Ip address is ok!\n\n");


    printURL(url);


    printf("Establishing a TCP connection... \n"); 

    int socket_fd = ftp_connect(url->port, url->ip_address); 
    if(socket_fd < 0) exit(1);

    printf("Established TCP connection!\n\n"); 


    printf("Logging in...\n");

    if(ftp_login(socket_fd, url->user, url->password) < 0) exit(1);

    printf("Logged in!\n\n");


    printf("Entering Passive Mode... \n");

    int data_fd = ftp_passive_mode(socket_fd);
    if(data_fd < 0) exit(1);

    printf("Entered Passive Mode\n\n");


    printf("Downloading file...\n"); 

    if(ftp_request_file(socket_fd, url->url_path) < 0) exit(1);
    if(ftp_download_file(data_fd, url->url_path, url->filename) < 0) exit(1);

    printf("Downloaded file\n\n");


    printf("Disconecting...\n");

    if(close(socket_fd) < 0) {
        perror("Error closing socket file descriptor!\n");
        exit(1);
    }

    if(close(data_fd) < 0) {
        perror("Error closing data socket file descriptor!\n");
        exit(1);
    }

    printf("Disconected with success!\n");
    return 0;
}
