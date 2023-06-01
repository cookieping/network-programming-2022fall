#include <iostream>
#include <time.h>
#include <sys/time.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <strings.h>

using namespace std;
static struct timeval _t0;  // include: tv_sec & tv_usec
static unsigned long long bytesent = 0;

double tv2s(struct timeval *ptv) {
	return 1.0*(ptv->tv_sec) + 0.000001*(ptv->tv_usec);
}

void handler(int s) {
	struct timeval _t1;
	double t0, t1;
	gettimeofday(&_t1, NULL);
	t0 = tv2s(&_t0);
	t1 = tv2s(&_t1);
	fprintf(stderr, "\n%lu.%06lu %llu bytes sent in %.6fs (%.6f Mbps; %.6f MBps)\n",
		_t1.tv_sec, _t1.tv_usec, bytesent, t1-t0, 8.0*(bytesent/1000000.0)/(t1-t0), (bytesent/1000000.0)/(t1-t0));
	exit(0);
}

int main(int argc, char* argv[]) {
    int sockfd;
    struct sockaddr_in servaddr;

    // socket
    if((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) <= 0) {
        cout << "Fail to create socket.\n";
        exit(0);
    }
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(10003);

    // connect
    if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
        cout << "Fail to connect.\n";
        exit(0);
    }

    // read
    char bufRead[1000];
    memset(bufRead, 0, sizeof(bufRead));
    read(sockfd, bufRead, sizeof(bufRead));

    // write
    signal(SIGINT,  handler);
    signal(SIGTERM, handler);

    string dataRateStr = argv[1];
    int dataRate = stod(dataRateStr) * 1000000;  // mega-byte = 1000,000 bytes
    
    // initialize the writing buffer
    string bufWriteStr = "";
    string tmp = "";  // 1000 bytes
    for(int i = 0; i < 1000; i++) tmp = tmp + "a";
    for(int i = 0; i < dataRate; i+=1000) bufWriteStr = bufWriteStr + tmp;
    const char* bufWrite = bufWriteStr.c_str();
    
    static struct timeval _t_start;
    static struct timeval _t_end;
    gettimeofday(&_t0, NULL);
    while(1) {
        // sending data
        gettimeofday(&_t_start, NULL);
        int w = write(sockfd, bufWrite, bufWriteStr.length());
        if(w == -1) cout << "Fail to write data...\n";
	bytesent += bufWriteStr.length();
        gettimeofday(&_t_end, NULL);

        // calculate left time
        double spentTime = tv2s(&_t_end) - tv2s(&_t_start);
        double leftTime = 1 - spentTime;
        long t_nano = leftTime * 1000000000;
        struct timespec t = {0, t_nano};
        cout << "Data sending rate: " << dataRateStr << "MBps\n";
	nanosleep(&t, NULL);
    }

    close(sockfd);
    return 0;
}
