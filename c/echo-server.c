#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <signal.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <strings.h> 



#define READ_BUFFER 50000

int main(int argc, char *argv[]){ 

    int sockfd, newsockfd, portno, clilen, childpid;
    char buffer[READ_BUFFER];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    signal(SIGCHLD,SIG_IGN);
    if(argc < 2){
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
        perror("ERROR opening socket");
    bzero((char*) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
        perror("ERROR on binding");
        exit(2);
    }
    listen(sockfd, 5);
    while(1){
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if(newsockfd < 0)
            perror("ERROR on accept");
        if((childpid = fork()) < 0){
            perror("server:fork error");
            exit(3);
        }
        else if(childpid == 0){
            close(sockfd);
            while(1){

                bzero(buffer, READ_BUFFER);
                n = read(newsockfd, buffer, READ_BUFFER-1);
                if(n < 0)
                    perror("ERROR reading from socket");
                n = write(newsockfd, buffer, READ_BUFFER-1);
                if(n < 0)
                    perror("ERROR writing to socket");
            }
            exit(0);
        }
        close(newsockfd);
    }
}
