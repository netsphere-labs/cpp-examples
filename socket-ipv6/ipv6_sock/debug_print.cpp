// -*- coding:utf-8-with-signature -*-
// デバッグ用の表示関数を定義する。

#include <stdio.h>
#include <assert.h>
#include "debug_print.h"

const char* addr_family_name(int ai_family)
{
    switch (ai_family)
    {
    // POSIXはAF_UNIXを好む。PF_UNIX, PF_LOCAL, PF_FILE でも同じ。
    case AF_UNIX: return "AF_UNIX";
    case AF_INET: return "AF_INET";
    case AF_INET6: return "AF_INET6";
    default:
        return "unknown";
    }
}
           
void sockaddr_print(const char* mes,
                    const struct sockaddr* ai_addr,
                    socklen_t ai_addrlen)
{
    assert(mes);
    assert(ai_addr);

    char hostbuf[NI_MAXHOST];
    char servbuf[NI_MAXSERV];
    
    getnameinfo(ai_addr, ai_addrlen, hostbuf, sizeof(hostbuf),
                servbuf, sizeof(servbuf), NI_NUMERICHOST | NI_NUMERICSERV);
    printf("%s: %s [%s]:%s\n", mes, addr_family_name(ai_addr->sa_family),
           hostbuf, servbuf);
}

void sock_print(const char* mes, int ai_family, int ai_socktype)
{
    printf("%s: %s ", mes, addr_family_name(ai_family) );

    switch (ai_socktype) {
    case SOCK_DGRAM: printf("SOCK_DGRAM\n"); break;
    case SOCK_STREAM: printf("SOCK_STREAM\n"); break;
    default: printf("%d\n", ai_socktype); break;
    }
}

