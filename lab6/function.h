#include <sys/socket.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h> 
#define SA struct sockaddr

int Socket(int family, int type, int protocol) {
    int listenfd = socket(family, type, protocol);
    if(listenfd == -1) {
        printf("Socket creation failed\n");
        exit(0);
    } else {
        // printf("Socket successfully created\n");
    }
    return listenfd;
}

void Bind(int sockfd, const SA * myaddr, socklen_t addrlen) {
    int res = bind(sockfd, myaddr, addrlen);
    if(res != 0) {
        printf("Socket bind failed\n");
		exit(0);
    } else {
        // printf("Socket successfully binded\n");
    }
}

void Connect(int sockfd, const SA * servaddr, socklen_t addrlen) {
    int res = connect(sockfd, servaddr, addrlen);
    if(res != 0) {
        printf("Socket connect failed\n");
        exit(0);
    } else {
        // printf("Socket successfully connected\n");
    }
}

void Listen(int sockfd, int backlog) {
    int res = listen(sockfd, backlog);
    if(res != 0) {
        printf("Listen failed\n");
		exit(0);
    } else {
        // printf("Server listening\n");
    }
}

int Accept(int sockfd, SA * clientaddr, socklen_t * addrlen) {
    int connfd = accept(sockfd, clientaddr, addrlen);
    if(connfd < 0) {
        if(errno == EINTR) return -1;
        else {
            printf("Server accept failed\n");
            // exit(0);
        }
    }
    return connfd;
}

void Close(int fd) {
    close(fd);
}