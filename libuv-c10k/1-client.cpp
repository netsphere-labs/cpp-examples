
#include <stdio.h>
#include <assert.h>
#include <string.h>
#ifndef _WIN32
  #include <netdb.h>
  #include <unistd.h>
  #define closesocket close
#else
#endif
#include "connect_to_server.h"

int main(int argc, char* argv[])
{
  SOCKET sockfd;

#ifdef _WIN32 
  // Initialize Winsock 
  WSADATA wsaData; 
  int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData); 
  if (iResult != 0) { 
    printf("WSAStartup failed: %d\n", iResult); 
    return 1; 
  }     
#endif
    
  sockfd = connect_to_server(argc >= 2 ? argv[1] : "localhost", 7000);
  if (sockfd == INVALID_SOCKET) {
    fprintf(stderr, "connect_to_server() failed.\n");
    return 1;
  }

  // 送受信
  char ch = 'A';
  send(sockfd, &ch, 1, 0);
  ch = '\0';
  recv(sockfd, &ch, 1, 0);
  printf("char from server = %c\n", ch);

#if 0
  // 明示的に shutdown する
  ch = 'X';
  send(sockfd, &ch, 1, 0);
#endif  
  // 明示的に server を shutdown する
  ch = 'Z';
  send(sockfd, &ch, 1, 0);
  
  closesocket(sockfd);
  return 0;
}


