/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

int MAX_BUFFER_SIZE = 1024;
int MAX_FILENAME_LENGTH = 256;

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int connection_handler(int sockfd) {
    FILE *destfile;
    int n;
    int filesize, filenamelength;
    char filename[256];
    char buffer[1024];
    bzero(buffer,256);


    // reading filename size and filename
    n = read(sockfd,buffer,sizeof(int));
    filenamelength = *(int*)buffer;

    n = read(sockfd,buffer,filenamelength);
    memcpy(filename, buffer, filenamelength);
    for (char *c = buffer; c< buffer+filenamelength; c++) {
        if (*c == '\0') {
            *c = '0';
        };
    };
    filename[filenamelength] = '\0';

    // correcting filename
    char * slash_pointer;
    while ((slash_pointer=index((char *)filename, '/')) != NULL) *slash_pointer = '_';

    // reading filesize
    n = read(sockfd,buffer,sizeof(int));
    filesize = *(int*)buffer;
    printf("Starting to recive file %s with size %d\n", filename, filesize);

    // opening file 
    destfile = fopen(filename, "wb");

    // reading filedata
    int bytesdone = 0;
    while (bytesdone < filesize) {
        n = read(sockfd, buffer, MAX_BUFFER_SIZE);
        if (n < 0) {
            error("ERROR reading from socket");
            break;
        };
        bytesdone += n;
        // printf("Recidev %d bytes\n", n);
        fwrite(buffer, n, 1, destfile);
    };

    fclose(destfile);

    printf ("File %s was writen in size of %d\n", filename, bytesdone);
    // if (n < 0) error("ERROR reading from socket");
    // printf("Here is the message: %s\n",buffer);
    // n = write(newsockfd,"I got your message",18);
    // if (n < 0) error("ERROR writing to socket");
    close(sockfd); 
};

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno;
     socklen_t clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     while (1) {
         newsockfd = accept(sockfd, 
                     (struct sockaddr *) &cli_addr, 
                     &clilen);
         if (newsockfd < 0) 
              printf("ERROR on accept");
         connection_handler(newsockfd);
     };
     close(sockfd);
     return 0; 
}