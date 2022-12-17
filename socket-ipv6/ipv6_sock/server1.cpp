// -*- coding:utf-8-with-signature -*-
// サーバをIPv4/IPv6両対応にするための、最小のコードを探る。

#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#ifndef _WIN32
  #include <unistd.h>
  #define closesocket close
  typedef int SOCKET;    // Windows: UINT_PTR 型
  #define WINAPI
  #define INVALID_SOCKET -1
  typedef uint32_t DWORD;
#else
  #define STRICT 1
  #define WIN32_LEAN_AND_MEAN
  #include <winsock2.h>
#endif
#include "debug_print.h"

constexpr int BACKLOG = 5;
constexpr int PORT = 12345;

/**
 * @param node bind() するホスト名. NULL の場合, INADDR_ANY or IN6ADDR_ANY_INIT.
 * @param port ポート番号. 0 の場合、空き番号を利用.
 * @return INVALID_SOCKET エラー
 *         >=0  listenしているソケット
 */
SOCKET tcp_listen(const char* node, int port)
{
    if ( port < 0 )
        return INVALID_SOCKET;

    int err;
    struct addrinfo hints;
    struct addrinfo* res = NULL;
    struct addrinfo* ai;
    SOCKET sockfd;

    memset(&hints, 0, sizeof(hints));
    if (!node)
        hints.ai_family = AF_INET6;    // AF_INET6は、IPv4/IPv6両方を受け付ける。
    hints.ai_socktype = SOCK_STREAM;
    // AI_PASSIVE をセットせずに, node = NULL の場合は, loopbackアドレス
    // (INADDR_LOOPBACK or IN6ADDR_LOOPBACK_INIT).
    // AI_PASSIVE をセットして, node = NULLのときは, INADDR_ANY, IN6ADDR_ANY_INIT.
#ifdef _WIN32
    hints.ai_flags = AI_PASSIVE;
#else
    hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV;
#endif

    // (node, service) の両方を nullptr にすると getaddrinfo() が失敗するが,
    // このようにすれば, ephemeral port から取れる.
    char service[11];
    sprintf(service, "%d", port);
    err = getaddrinfo(node, service, &hints, &res);
    if (err != 0) {
        fprintf(stderr, "getaddrinfo() failed: %s\n", gai_strerror(err));
        return INVALID_SOCKET;
    }

    ai = res;
    sock_print("create socket", ai->ai_family, ai->ai_socktype);
    sockfd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
    if ( sockfd == INVALID_SOCKET ) {
        freeaddrinfo(res);
        return INVALID_SOCKET;
    }

    int on = 1;
    if ( setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char*) &on,
                    sizeof(on)) < 0 ) {
        closesocket(sockfd);
        freeaddrinfo(res);
        return INVALID_SOCKET;
    }
    printf("set SO_REUSEADDR\n");

    if (bind(sockfd, ai->ai_addr, ai->ai_addrlen) < 0) {
        closesocket(sockfd);
        freeaddrinfo(res);
        return INVALID_SOCKET;
    }

    // もういらん.
    freeaddrinfo(res);
    res = NULL;

    // 表示する. port = 0 の場合は [::]:0 になってしまう。取得しなおす
    sockaddr_storage addr;   // sockaddr_in は IPv4 only.
    socklen_t socklen = sizeof(addr);
    if ( getsockname(sockfd, (sockaddr*) &addr, &socklen) == -1 ) {
        perror("get port failed");
        closesocket(sockfd);
        return INVALID_SOCKET;
    }
    sockaddr_print("listen socket", (sockaddr*) &addr, socklen);

    if (listen(sockfd, BACKLOG) < 0) {
        closesocket(sockfd);
        return INVALID_SOCKET;
    }
    printf("Listen succeeded\n");

    return sockfd;
}


struct MyArgData {
    SOCKET sockfd;
    int hoge;
};

// Windows: スレッド, UNIX: 子プロセス
DWORD WINAPI handler(void* arg)
{
    MyArgData* myArg = (MyArgData*) arg;

    // 1バイトだけやり取りする
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
    SOCKET listen_sockfd;

    listen_sockfd = tcp_listen(NULL, PORT);  // [::]:12345
    if (listen_sockfd == INVALID_SOCKET) {
        perror("server");
        exit(1);
    }

    printf("wait...\n");

    while (1) {
        SOCKET cs;
        // クライアントの情報を得る場合
        struct sockaddr_storage client_sa;  // sockaddr_in 型ではない。
        socklen_t len = sizeof(client_sa);
        cs = accept(listen_sockfd, (struct sockaddr*) &client_sa, &len);
        if ( cs == INVALID_SOCKET ) {
            if (errno == EINTR)
                continue;
            perror("accept");
            exit(1);
        }

        // accept した相手先を表示.
        printf("accepted.\n");
        sockaddr_print("peer", (struct sockaddr*) &client_sa, len);

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
        // UNIX: 簡単に fork する
        if (fork() == 0) {
            // 子プロセス
            closesocket(listen_sockfd);

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
