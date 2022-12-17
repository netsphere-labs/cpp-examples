
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
#endif
#include <string.h>
#include <errno.h>
#include "debug_print.h"
#include "connect_to_server.h"

/**
 * サーバに接続する。Block する.
 * @return エラーのとき INVALID_SOCKET
 */
SOCKET connect_to_server(
    const char* hostname,  // IPv4 or IPv6ホスト名
    int port )   // ポート番号
{
    assert( hostname );
    if ( port <= 0 || port > 65535 )
        return INVALID_SOCKET;

    SOCKET sockfd = INVALID_SOCKET;
    int err;
    struct addrinfo hints;
    struct addrinfo* res = NULL;
    struct addrinfo* ai;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;     // IPv4/IPv6両対応
    hints.ai_socktype = SOCK_STREAM;
    // AI_NUMERICSERV を指定しなければ、service は 'pop'などでもよい。
    hints.ai_flags = AI_NUMERICSERV;

    char service[11];
    sprintf(service, "%d", port);
    err = getaddrinfo(hostname, service, &hints, &res);
    if (err != 0) {
        fprintf(stderr, "getaddrinfo() failed: %s\n", gai_strerror(err));
        return INVALID_SOCKET;
    }

    // どれか一つが成功するまで試みる.
    for (ai = res; ai; ai = ai->ai_next) {
        //sockaddr_print("connect...", ai->ai_addr, ai->ai_addrlen);
    RETRY:
        sockfd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
        if ( sockfd == INVALID_SOCKET ) {
            fprintf(stderr, "socket() failed: %s\n", strerror(errno));
            freeaddrinfo(res);
            return INVALID_SOCKET;
        }
        // 実際に connect しないと正解か分からない.
        if ( connect(sockfd, ai->ai_addr, ai->ai_addrlen) < 0 ) {
            if (errno == EINTR) { // シグナル割り込み
                // FreeBSD では, open からやり直さないと EALREADY エラー.
                closesocket(sockfd);
                goto RETRY;
            }
            closesocket(sockfd);
            sockfd = INVALID_SOCKET;
            continue;
        }
        // OK. Blocking 版は一つの成功でOK.
        //sockaddr_print("connected", ai->ai_addr, ai->ai_addrlen);
        break;
    }
    freeaddrinfo(res);
    return sockfd;
}
