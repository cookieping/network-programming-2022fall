#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <errno.h>
#define MAX 80
#define PORT 11111
#define SA struct sockaddr
using namespace std;

int main(int argc, char* argv[])
{
	int sockfd, connfd;
    pid_t childpid;
    socklen_t clilen;
	struct sockaddr_in servaddr, cliaddr;
    // void sig_child(int);

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		printf("Socket creation failed...\n");
		exit(0);
	} else {
		printf("Socket successfully created..\n");
    }
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	// Bind
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
		printf("Socket bind failed...\n");
		exit(0);
	} else {
		printf("Socket successfully binded..\n");
    }

	// Listen
	if ((listen(sockfd, 5)) != 0) {
		printf("Listen failed...\n");
		exit(0);
	} else {
		printf("Server listening..\n");
    }

    // Signal
    // signal(SIGCHLD, sig_child);
    for(; ;) {
        clilen = sizeof(cliaddr);
        if ( (connfd = accept(sockfd, (SA*) &cliaddr, &clilen)) < 0) {
            if(errno == EINTR) continue;
            else {
                printf("Server accept failed...\n");
		        exit(0);
            }
        }

        // success
        if((childpid = fork()) == 0) {
            close(sockfd);

            // process request
            const char* pathToProgram = argv[2];
            char *envp[]={0,NULL};
            
            // dup2(1, STDOUT_FILENO);
            char testBuf[] = "write to client!!!!\n";
            write(connfd, testBuf, sizeof(testBuf));
            execvp(pathToProgram, argv + 2);

            exit(0);
        }
        close(connfd);
    }
    return 0;
}
