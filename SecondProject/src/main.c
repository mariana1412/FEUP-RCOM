#include <stdio.h>
#include "url.h"

void printURL(url_struct *url ){
    printf("\nPRINTING URL INFO: \n");
    printf("user = %s\n", url->user);
    printf("password = %s\n", url->password);
    printf("host = %s\n", url->host);
    printf("url_path = %s\n", url->url_path);
    printf("filename = %s\n", url->filename);
    printf("ip address = %s\n\n", url->ip_address);
}

//https://programmer.help/blogs/a-simple-ftp-client-implemented-in-c-language.html?fbclid=IwAR1Gvq05Fn62IegJO2PB68MaShMR8Vu4tW_FGYc2J_6DPRQ_XBRNsfLWAkc
int main(int argc, char *argv[]) {

    if (argc != 2) {  
        fprintf(stderr, "usage: ./download ftp://[<user>:<password>@]<host>/<url-path>\n");
        exit(1);
    }
    
    printf("Parsing URL... \n");

    url_struct *url = createUrlStruct();

    if(parse_file_url(argv[1], url) < 0){
        fprintf(stderr, "usage: ./download ftp://[<user>:<password>@]<host>/<url-path>\n");
        exit(1);
    }

    printf("URL is parsed!\n");

    printURL(url);

    printf("GETTING IP ADDRESS... \n");
    if(getIpAddress(url) < 0) exit(1);
    printf("Ip address is ok!\n");

    printf("Establishing a connection... \n");
    printf("Logging in...\n");
    printf("Passive mode... \n");
    printf("Downloading file...\n"); //request and download file
    printf("Disconecting...\n");

    return 0;
}
