#ifndef _WIN32
  #include <netdb.h>
#else
  //#include <ws2def.h>
  #define STRICT 1
  #define WIN32_LEAN_AND_MEAN
  #include <ws2tcpip.h>  // socklen_t
#endif

#ifdef __cplusplus
extern "C" {
#endif
  
extern void sockaddr_print(const char* mes, const struct sockaddr* ai_addr,
                           socklen_t ai_addrlen);
extern void sock_print(const char* mes, int ai_family, int ai_socktype);

#ifdef __cplusplus
}
#endif
