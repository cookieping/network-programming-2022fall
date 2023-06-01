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
#define MAXLINE 10000
#define SA struct sockaddr
#define LISTENQ 1024
using namespace std;

string getCommand(char* buf) {
    string ans = "";
    for(int i = 0; i < sizeof(buf); i++) {
        if(buf[i] == ' ' || buf[i] == '\n') break;
        ans += buf[i];
    }
    return ans;
}
bool checkCommand(string buf, string cmd) {
    if(buf.length() != cmd.length()) return false;
    for(int i = 0; i <= buf.length(); i++) {
        if(buf[i] != cmd[i]) return false;
    }
    return true;
}

int main(int argc, char **argv) {
    int                 i1, i2, maxi1, maxi2, maxfd1, maxfd2, listenfd1, listenfd2, sockfd, connfd1, connfd2;
    int                 nready1, nready2, client1[FD_SETSIZE], client2[FD_SETSIZE];
    ssize_t             n;
    fd_set              rset1, allset1, rset2, allset2;
    char                buf1[MAXLINE], buf2[MAXLINE];
    socklen_t           clilen1, clilen2;
    struct sockaddr_in  cliaddr1, cliaddr2, servaddr1, servaddr2;
    uint16_t            SERV_PORT1 = atoi(argv[1]);
    uint16_t            SERV_PORT2 = SERV_PORT1 + 1;

    // port 1 for command channel used to handle client commands
    listenfd1 = Socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr1, sizeof(servaddr1));
    servaddr1.sin_family      = AF_INET;
    servaddr1.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr1.sin_port        = htons(SERV_PORT1);
    Bind(listenfd1, (SA*)&servaddr1, sizeof(servaddr1));
    cout << "Bind success. Port = " << SERV_PORT1 << endl;
    Listen(listenfd1, LISTENQ);

    maxfd1 = listenfd1;
    maxi1 = -1;
    for (i1 = 0; i1 < FD_SETSIZE; i1++) client1[i1] = -1;
    FD_ZERO(&allset1);
    FD_SET(listenfd1, &allset1);

    // port 2 data sink used for receiving data from connected sockets without replies
    listenfd2 = socket(AF_INET, SOCK_STREAM, 0);
    // int status = fcntl(listenfd2, F_SETFL, fcntl(listenfd2, F_GETFL, 0) | O_NONBLOCK); // set non-blocking
    // if(status < 0) cout << "fcntl error\n";

    bzero(&servaddr2, sizeof(servaddr2));
    servaddr2.sin_family      = AF_INET;
    servaddr2.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr2.sin_port        = htons(SERV_PORT2);
    Bind(listenfd2, (SA*)&servaddr2, sizeof(servaddr2));
    cout << "Bind success. Port = " << SERV_PORT2 << endl;
    Listen(listenfd2, LISTENQ);

    maxfd2 = listenfd2;
    maxi2 = -1;
    for (i2 = 0; i2 < FD_SETSIZE; i2++) client2[i2] = -1;
    FD_ZERO(&allset2);
    FD_SET(listenfd2, &allset2);
    long long receiveByte = 0;  // record how many byte has been received

    timeval noWait = {0, 0};  // so that select won't keep waiting
    struct timeval tv_last;
    struct timezone tz_last;
    gettimeofday(&tv_last, &tz_last);

    for ( ; ; ) {
        // 2. sink channel (port 2)
        rset2 = allset2;
        nready2 = select(maxfd2 + 1, &rset2, NULL, NULL, &noWait);
        
        if (FD_ISSET(listenfd2, &rset2)) {  // new client connection
            clilen2 = sizeof(cliaddr2);
            connfd2 = Accept(listenfd2, (SA*) &cliaddr2, &clilen2);
#ifdef  NOTDEF
            printf("new client: %s, port %d\n",
            Inet_ntop(AF_INET, &cliaddr.sin_addr, 4, NULL),
            ntohs(cliaddr.sin_port));
#endif
            for (i2 = 0; i2 < FD_SETSIZE; i2++) {
                if (client2[i2] < 0) {
                    client2[i2] = connfd2;
                    break;
                }
            }
            if (i2 == FD_SETSIZE) {
                cout << "too many clients\n";
                exit(0);
            }
            FD_SET(connfd2, &allset2);
            if (connfd2 > maxfd2) maxfd2 = connfd2;
            if (i2 > maxi2) maxi2 = i2;
            if (--nready2 <= 0) continue;
        }
        for (i2 = 0; i2 <= maxi2; i2++) {
            if ( (sockfd = client2[i2]) < 0) continue;
            if (FD_ISSET(sockfd, &rset2)) {
                memset(buf2, 0, sizeof(buf2));
                n = read(sockfd, buf2, MAXLINE);
                // cout << "* buffer = " << buf2;

                if (n == 0) {
                    close(sockfd);
                    FD_CLR(sockfd, &allset2);
                    client2[i2] = -1;

                } else if (n == -1) { 
                    // cout << "Read fail\n";

                } else { 
                    string bufRead = buf2;
                    receiveByte += bufRead.length();

                }
                if (--nready2 <= 0) break;
            }
        }

         // 1. read the command from the command channel (port 1)
        rset1 = allset1;
        nready1 = select(maxfd1 + 1, &rset1, NULL, NULL, &noWait);
        if (FD_ISSET(listenfd1, &rset1)) {  // new client connection
            clilen1 = sizeof(cliaddr1);
            connfd1 = Accept(listenfd1, (SA*) &cliaddr1, &clilen1);
#ifdef  NOTDEF
                        printf("new client: %s, port %d\n",
                                        Inet_ntop(AF_INET, &cliaddr.sin_addr, 4, NULL),
                                        ntohs(cliaddr.sin_port));
#endif
            for (i1 = 0; i1 < FD_SETSIZE; i1++) {
                if (client1[i1] < 0) {
                    client1[i1] = connfd1;
                    break;
                }
            }
            if (i1 == FD_SETSIZE) {
                cout << "too many clients\n";
                exit(0);
            }
            FD_SET(connfd1, &allset1);
            if (connfd1 > maxfd1) maxfd1 = connfd1;
            if (i1 > maxi1) maxi1 = i1;
            if (--nready1 <= 0) continue;
        }

        for (i1 = 0; i1 <= maxi1; i1++) {
            if ( (sockfd = client1[i1]) < 0) continue;
            if (FD_ISSET(sockfd, &rset1)) {
                memset(buf1, 0, sizeof(buf1));
                n = read(sockfd, buf1, MAXLINE);
                cout << "* buffer = " << buf1;

                if (n == 0) {
                    close(sockfd);
                    FD_CLR(sockfd, &allset1);
                    client1[i1] = -1;

                } else if (n == -1) { 
                    // cout << "Read fail\n";

                } else { 
                    // get currant time
                    struct  timeval tv;
                    struct  timezone tz;
                    gettimeofday(&tv, &tz);
                    string currentTime = to_string(tv.tv_sec) + "." + to_string(tv.tv_usec);
                    string msg;

                    string cmd = getCommand(buf1);
                    if(checkCommand(cmd, "/ping")) {
                        msg = currentTime + " PONG\n";
                        write(sockfd, msg.c_str(), msg.length());
                    }
                    if(checkCommand(cmd, "/clients")) {
                        int clientNum = 0;
                        for (int j = 0; j <= maxi2; j++) {
                            if(client2[j] != -1) clientNum++;
                        }
                        msg = currentTime + " CLIENTS " + to_string(clientNum) + "\n";
                        write(sockfd, msg.c_str(), msg.length());
                    }
                    if(checkCommand(cmd, "/reset")) {
                        msg = currentTime + " RESET " + to_string(receiveByte) + "\n";
                        // cout << msg << endl;
                        write(sockfd, msg.c_str(), msg.length());
                        receiveByte = 0;
                        tv_last = tv;
                        tz_last = tz;
                    }
                    if(checkCommand(cmd, "/report")) {
                        // calculate time pass
                        int currTime = tv.tv_sec * 1000000 + tv.tv_usec;
                        int lastTime = tv_last.tv_sec * 1000000 + tv.tv_usec;
                        float elaspeTime = (currTime - lastTime) / 1000000.0;

                        float measure = (8.0 * receiveByte) / 1000000.0 / elaspeTime;
                        msg = currentTime + " REPORT " + to_string(receiveByte) + " " + to_string(elaspeTime) + "s " + to_string(measure) + "Mbps\n";
                        // cout << msg << endl;
                        write(sockfd, msg.c_str(), msg.length());
                    }
                }
                if (--nready1 <= 0) break;
            }
        }
    }
}