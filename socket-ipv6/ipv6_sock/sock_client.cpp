// -*- coding:utf-8-with-signature -*-

// 次を参考にした。ただし、もとサンプルはsocket()の書く場所がおかしい。
// http://www.coins.tsukuba.ac.jp/~yas/coins/syspro-2004/2004-04-26/

#include <sys/types.h>
#include <stdio.h>
#include <assert.h>
#ifndef _WIN32
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <unistd.h>
  #include <sys/socket.h>
  #define closesocket close
  typedef int SOCKET;
  #define INVALID_SOCKET -1
#else
  #define STRICT 1
  #define WIN32_LEAN_AND_MEAN
  #include <winsock2.h>
#endif
#include <string.h>
#include "debug_print.h"
#include "connect_to_server.h"

constexpr int PORT = 12345;

int main(int argc, char* argv[])
{
    SOCKET sockfd;
    char ch;

#ifdef _WIN32
    // Initialize Winsock
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }
#endif

    sockfd = connect_to_server(argc >= 2 ? argv[1] : "localhost", PORT);
    if ( sockfd == INVALID_SOCKET ) {
        perror("client");
        return 1;
    }

    // 送受信
    ch = 'A';
    send(sockfd, &ch, 1, 0);
    recv(sockfd, &ch, 1, 0);
    printf("char from server = '%c'\n", ch);

    closesocket(sockfd);
    return 0;
}
