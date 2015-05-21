#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

int MAX_BUFFER_SIZE = 1024;
int MAX_FILENAME_LENGTH = 256;

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    char *filename;
    FILE * srcfile;
    int filenamelength;
    int filesize;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[MAX_BUFFER_SIZE];
    if (argc < 4) {
       fprintf(stderr,"usage %s hostname port filename\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    filename = argv[3];

    // opening the file 
    srcfile = fopen(filename, "rb");
    fseek(srcfile, 0L, SEEK_END);
    filesize = ftell(srcfile);
    fseek(srcfile, 0L, SEEK_SET);
    filenamelength = strlen(filename);

    printf("Sending file %s to host %s at port %d\n", filename, argv[1], portno);

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");

    // sending file info
    // filename length
    write(sockfd, &filenamelength, sizeof(int));
    // filename
    write(sockfd, filename, filenamelength);
    // filesize
    write(sockfd, &filesize, sizeof(int));

    while (1) {
        n=fread(buffer, 1, MAX_BUFFER_SIZE, srcfile);
        if (n <= 0) break;        
        write(sockfd, buffer, n);
    };
    fclose(srcfile);

    // printf("Please enter the message: ");
    // bzero(buffer,256);
    // fgets(buffer,255,stdin);
    // n = write(sockfd,buffer,strlen(buffer));
    // if (n < 0) 
    //      error("ERROR writing to socket");
    // bzero(buffer,256);
    // n = read(sockfd,buffer,255);
    // if (n < 0) 
    //      error("ERROR reading from socket");
    // printf("%s\n",buffer);
    close(sockfd);
    printf("Done\n");
    return 0;
}