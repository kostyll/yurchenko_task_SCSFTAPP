#include <stdio.h>
#include <stdlib.h>
// #include <unistd.h>
// #include <string.h>
// #include <sys/types.h>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <netdb.h> 
#include <windows.h>

int MAX_BUFFER_SIZE = 1024;
int MAX_FILENAME_LENGTH = 256;

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int init_WSA(WSADATA *wsaData) {
    int result;
    result = WSAStartup( MAKEWORD(2, 2), &wsaData);
    if(result != 0)
    {
     std::cout << "Error WSAStartup: " << result << std::endl;
     return false;
    }
    return true;
};

int main(int argc, char *argv[])
{
    // int sockfd, portno, n;
    WSADATA wsaData;
    SOCKET clientSocket = INVALID_SOCKET;
    char *filename;
    FILE * srcfile;
    int filenamelength;
    int filesize;
    // struct sockaddr_in serv_addr;
    // struct hostent *server;

    char buffer[MAX_BUFFER_SIZE];
    if (argc < 4) {
       fprintf(stderr,"usage %s hostname port filename\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    // sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // if (sockfd < 0) 
    //     error("ERROR opening socket");
    // server = gethostbyname(argv[1]);
    // if (server == NULL) {
    //     fprintf(stderr,"ERROR, no such host\n");
    //     exit(0);
    // }

    filename = argv[3];

    // opening the file 
    srcfile = fopen(filename, "rb");
    fseek(srcfile, 0L, SEEK_END);
    filesize = ftell(srcfile);
    fseek(srcfile, 0L, SEEK_SET);
    filenamelength = strlen(filename);

    printf("Sending file %s to host %s at port %d\n", filename, argv[1], portno);

    if (not init_WSA(&wsaData)) error("WSA init error"); 

    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if( clientSocket == INVALID_SOCKET)
    {
     std::cout << "Error socket(): " << WSAGetLastError() << std::endl;
     WSACleanup();
     return 1;
    }

    sockaddr_in clientService;
    clientService.sin_family = AF_INET;
    clientService.sin_addr.s_addr = inet_addr(argv[1]);
    clientService.sin_port = htons(portno);

    int result;
    result = connect(
       clientSocket, 
       reinterpret_cast< SOCKADDR* >(&clientService), 
       sizeof(clientService)
      );
    if(result != 0)
    {
     std::cout << "Error at connect(): " << WSAGetLastError() << std::endl;
     WSACleanup();
     return 1;
    }

    // bzero((char *) &serv_addr, sizeof(serv_addr));
    // serv_addr.sin_family = AF_INET;
    // bcopy((char *)server->h_addr, 
    //      (char *)&serv_addr.sin_addr.s_addr,
    //      server->h_length);
    // serv_addr.sin_port = htons(portno);
    // if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
    //     error("ERROR connecting");

    // sending file info
    // filename length
    // write(sockfd, &filenamelength, sizeof(int));
    send(clientSocket, &filenamelength, static_cast< int >(sizeof(int)), 0);
    // filename
    // write(sockfd, filename, filenamelength);
    send(clientSocket, filename, static_cast< int >(filenamelength), 0);
    // filesize
    // write(sockfd, &filesize, sizeof(int));
    send(clientSocket, &filesize, static_cast< int >(sizeof(int)), 0);

    while (1) {
        n=fread(buffer, 1, MAX_BUFFER_SIZE, srcfile);
        if (n <= 0) break;        
        // write(sockfd, buffer, n);
        int result;
        // char data[] = "Test";
        // result = send(clientSocket, data, static_cast< int >(strlen(data)), 0);
        // if( result < 0 )
        // {
        //  std::cout << "Ошибка в send(): " << WSAGetLastError() << std::endl;
        //  return 1;
        // }
        result = send(clientSocket, buffer, static_cast< int >(n), 0);
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
    // close(sockfd);
    printf("Done\n");
    return 0;
}