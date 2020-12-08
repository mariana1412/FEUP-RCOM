#include "url.h"

url_struct * createUrlStruct() {

    url_struct *url = malloc (sizeof (url_struct));
    url->user = malloc(MAX_STRING_SIZE);
    url->password = malloc(MAX_STRING_SIZE);
    url->host = malloc(MAX_STRING_SIZE);
    url->url_path = malloc(MAX_STRING_SIZE);
    url->filename = malloc(MAX_STRING_SIZE);
    url->ip_address = malloc(MAX_STRING_SIZE);
    url->port = 21; //this protocol uses port 21 by default

    return url;
}

int parse_file_url(char * url, url_struct *urlInfo) { //format ftp://[<user>:<password>@]<host>/<url-path>
    
    char *ftp = malloc(MAX_STRING_SIZE);
    memcpy(ftp, url, 6);

    if(strcmp(ftp, "ftp://") != 0) return -1;

    strtok(url, "/"); 
    char* rest_args = strtok(NULL, "") + 1; //[<user>:<password>@]<host>/<url-path>

    char *user = malloc(MAX_STRING_SIZE);
    char *password = malloc(MAX_STRING_SIZE);
    char *host = malloc(MAX_STRING_SIZE);
    char *url_path = malloc(MAX_STRING_SIZE);

    if(strchr(rest_args, '@') == NULL){ //anonymous
        if(sscanf(rest_args, "%[^/]/%s", host, url_path) == 2){
            user = "anonymous";
            password = "password";
        }
        else return -1;
    }
    else if (sscanf(rest_args, "%[^:]:%[^@]@%[^/]/%s", user, password, host, url_path) != 4) return -1;

    strcpy(urlInfo->user, user);
    strcpy(urlInfo->password, password);
    strcpy(urlInfo->host, host);
    strcpy(urlInfo->url_path, url_path);    

    char *filename = strrchr(urlInfo->url_path, '/');
    if(filename == NULL) urlInfo->filename = urlInfo->url_path;
    else urlInfo->filename = ++filename;
        
    return 0;
}

int getIpAddress(url_struct *url){ //getip.c moodle

    struct hostent *h;
    
    if ((h = gethostbyname(url->host)) == NULL) {  
        herror("gethostbyname");
        return -1;
    }

    url->ip_address = inet_ntoa(*((struct in_addr *)h->h_addr));

    return 0;
}