
#ifndef _WIN32
  typedef int SOCKET;
  #define INVALID_SOCKET -1
#else
  #define STRICT 1
  #define WIN32_LEAN_AND_MEAN
  #include <winsock2.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * サーバに接続する。Block する.
 * @return エラーのとき INVALID_SOCKET
 */
SOCKET connect_to_server(
    const char* hostname,  // IPv4 or IPv6ホスト名
    int port );   // ポート番号

#ifdef __cplusplus
}
#endif
