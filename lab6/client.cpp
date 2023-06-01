#include <sys/select.h>
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/signal.h>
#include <sys/types.h>
#include<sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <errno.h>
#include <iostream>
#include <map>
#include <queue>
#include <time.h>
#include <cstdlib>
#include <stdarg.h>
#include "function.h"
#define MAX 10000
#define SA struct sockaddr
using namespace std;

bool keepSendingData = true;
void func(int sockfd)
{
	char buff[MAX];
	int n;

	// 2. create connections to the server data sink channel
	int sinkNum = 15;
	vector<int> sinkfd;
	for(int i = 0; i < sinkNum; i++) {  // create connections
		int fd;
		struct sockaddr_in servaddr;

		fd = Socket(AF_INET, SOCK_STREAM, 0);
		bzero(&servaddr, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
		servaddr.sin_port = htons(9999);  // the data sink port

		Connect(fd, (SA*)&servaddr, sizeof(servaddr));
		sinkfd.push_back(fd);
	}
	
	// 3. send the reset command
    string msg = "/reset\n";
	write(sockfd, msg.c_str(), msg.length());
	memset(buff, 0, sizeof(buff));
	n = read(sockfd, buff, sizeof(buff));

	// 4. keep sending data to the server with the data sink connections until the client is terminated
	// initialize the writing buffer
    string buffWrite = "";
    for(int i = 0; i < 100000; i++) buffWrite = buffWrite + "a";
	
	while(keepSendingData) {
		for(int i = 0; i < sinkNum; i++) {
			write(sinkfd[i], buffWrite.c_str(), buffWrite.length());
		}
	}

	// 5. close all the data sink connections
	for(int i = 0; i < sinkNum; i++) {
		close(sinkfd[0]);
		sinkfd.erase(sinkfd.begin());
	}

	// 6. send the report command and show the result
	msg = "/report\n";
	write(sockfd, msg.c_str(), msg.length());
	memset(buff, 0, sizeof(buff));
	n = read(sockfd, buff, sizeof(buff));
	cout << buff << "\n";
}

void my_handle(int s) {
	keepSendingData = false;
}

int main(int argc, char **argv)
{
	signal(SIGTERM, my_handle);

	int sockfd;
	struct sockaddr_in servaddr;

	// 1. connect to the server's command channel
	sockfd = Socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);
	servaddr.sin_port = htons(atoi(argv[2]));  // the command port

	// connect the client socket to server socket
	Connect(sockfd, (SA*)&servaddr, sizeof(servaddr));

	func(sockfd);

	close(sockfd);
}
