// -*- coding:utf-8-with-signature -*-

// http://www.ipv6style.jp/jp/apps/20030829/    => リンク切れ.
// を参考にした。

#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <algorithm>
#include <stdint.h>
#ifndef _WIN32
  #include <unistd.h>
  #define closesocket close
  typedef int SOCKET;    // Windows: UINT_PTR 型
  #define INVALID_SOCKET -1
  #define WINAPI
  typedef uint32_t DWORD;
#endif
#include "debug_print.h"
using namespace std;

constexpr int BACKLOG = 5;
constexpr int PORT = 12345;

/**
 * IPv4専用とIPv6専用の2つのソケットで待ち受ける. listen まで行う.
 * @param node  bind() するホスト名. NULL の場合, INADDR_ANY or IN6ADDR_ANY_INIT.
 * @param port ポート番号. 0 の場合, 空き番号を利用.
 * @return -1  エラー発生
 *         0   listenできたソケットがない
 *         >=1 listenできたソケットの数
 */
int tcp_listen_v6separate(
    const char* node,
    int port,
    SOCKET* sockfd,          // (out) ソケットfdの配列
    int fd_size)
{
    assert(sockfd);

    if (port < 0)
        return -1;
    if (fd_size <= 0)
        return -1;

    int err;
    struct addrinfo hints;
    struct addrinfo* res = NULL;
    struct addrinfo* ai;
    int socksize = 0;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;   // IPv4/IPv6専用ソケットを作る。
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV;   // acceptするためにbind()する場合は指定する。

    char service[11];
    sprintf(service, "%d", port);
    // node = NULLのとき、INADDR_ANY, IN6ADDR_ANY_INIT に相当。
    err = getaddrinfo(node, service, &hints, &res);
    if (err != 0) {
        fprintf(stderr, "getaddrinfo() failed: %s\n", gai_strerror(err));
        return -1;
    }

    // 順に試す.
    for (ai = res; ai; ai = ai->ai_next) {
        sock_print("create socket", ai->ai_family, ai->ai_socktype);

        *sockfd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
        if ( *sockfd == INVALID_SOCKET )
            goto err;

        socksize++;

        // IPv6ソケットはIPv6からの接続だけ受け付ける。
        if (ai->ai_family == AF_INET6) {
            int on = 1;
            if ( setsockopt(*sockfd, IPPROTO_IPV6, IPV6_V6ONLY, (char*) &on,
                            sizeof(on)) < 0 ) {
                goto err;
            }
            printf("set IPV6_V6ONLY\n");
        }

        if (ai->ai_family == AF_INET || ai->ai_family == AF_INET6) {
            int on = 1;
            if (setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, (char*) &on,
                           sizeof(on)) < 0) {
                goto err;
            }
            printf("set SO_REUSEADDR\n");
        }

        if (bind(*sockfd, ai->ai_addr, ai->ai_addrlen) < 0)
            goto err;
        if (listen(*sockfd, BACKLOG) < 0)
            goto err;

        sockaddr_print("listen succeeded", ai->ai_addr, ai->ai_addrlen);
        sockfd++;
        if (socksize >= fd_size)
            break;
    }
    freeaddrinfo(res);
    return socksize;

 err:
    for (int i = 0; i < socksize; i++)
        closesocket(sockfd[i]);
    freeaddrinfo(res);
    return -1;
}


struct MyArgData {
    SOCKET sockfd;
    int hoge;
};

DWORD WINAPI handler(void* arg)
{
    MyArgData* myArg = (MyArgData*) arg;

    // 1バイトだけやり取りする.
    unsigned char ch;
    recv( myArg->sockfd, (char*) &ch, 1, 0);
    ch++;
    send( myArg->sockfd, (char*) &ch, 1, 0);

    closesocket(myArg->sockfd);
#ifdef _WIN32
    HeapFree(GetProcessHeap(), 0, arg);
#else
    free(arg);
#endif
    return 0;
}


void test_server()
{
    SOCKET sockfds[FD_SETSIZE];
    int socknum;
    SOCKET smax;
    fd_set rfd, rfd_init;
    int i;

    socknum = tcp_listen_v6separate(NULL, PORT, sockfds,
                                    sizeof(sockfds) / sizeof(SOCKET) );
    if (socknum < 0) {
        perror("server");
        exit(1);
    }
    else if (socknum == 0) {
        printf("can't listen socket.\n");
        exit(1);
    }

    FD_ZERO(&rfd_init);
    smax = -1;
    for (i = 0; i < socknum; i++) {
        FD_SET(sockfds[i], &rfd_init);
        smax = max(smax, sockfds[i]);
    }

    printf("wait...\n");

    while (1) {
        rfd = rfd_init;
        // 第一引数は個数ではなく (値の最大値 + 1)
        int m = select(smax + 1, &rfd, NULL, NULL, NULL);
        if (m < 0) {
            if (errno == EINTR)
                continue;
            perror("select");
            exit(1);
        }

        // それぞれのソケットを調べる
        for (i = 0; i < socknum; i++) {
            if (!FD_ISSET(sockfds[i], &rfd))
                continue;

            struct sockaddr_storage sa;  // sockaddr_in 型ではない。
            socklen_t len = sizeof(sa);  // クライアントの情報を得る場合
            SOCKET cs = accept(sockfds[i], (struct sockaddr*) &sa, &len);
            if ( cs == INVALID_SOCKET ) {
                perror("accept");
                exit(1);
            }
            printf("accepted.\n");
            sockaddr_print("peer", (struct sockaddr*) &sa, len);

#ifdef _WIN32
            // Windows: マルチスレッド
            MyArgData* arg = (MyArgData*) HeapAlloc(GetProcessHeap(),
                                                HEAP_ZERO_MEMORY,
                                                sizeof(MyArgData) );
            arg->sockfd = cs;
            HANDLE h = CreateThread(
                        NULL,       // lpThreadAttributes
                        0,          // dwStackSize. 0 is default size.
                        handler,    // lpStartAddress
                        arg,        // lpParameter
                        0,          // dwCreationFlags
                        NULL);      // lpThreadId
            if ( h == NULL ) {
                fprintf(stderr, "CreateThread failed: %lu\n", GetLastError() );
                exit(1);
            }
#else
            // UNIX: マルチプロセス
            if (fork() == 0) {
                // 子プロセス
                closesocket(sockfds[i]);

                MyArgData* arg = (MyArgData*) malloc(sizeof(MyArgData));
                arg->sockfd = cs;
                handler(arg);
                exit(0);
            }
            // 親プロセス
            closesocket(cs);
#endif
        }
    }
}


int main()
{
#ifdef _WIN32
    // Initialize Winsock2
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        fprintf(stderr, "WSAStartup failed: %d\n", iResult);
        return 1;
    }
#endif

    test_server();
    return 0;
}
