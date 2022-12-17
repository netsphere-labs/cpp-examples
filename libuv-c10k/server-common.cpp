
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <uv.h>
#include <sys/resource.h> // getrlimit()
#include <stdlib.h>

/**
 * TCP で bind() までを行う. IPv6対応.
 * uv_stream_t 型を基底クラスとして, uv_tcp_t, uv_pipe_t および uv_tty_t が派生
 * クラス.
 * @param node bind() するホスト名. NULL の場合, INADDR_ANY, IN6ADDR_ANY_INIT.
 * @param port ポート番号. 0 の場合, 空いているポートを割り当てる.
 *
 * @return 成功 = 0, エラーの場合 = -1
 */
int tcp_setup_server(uv_tcp_t* server, const char* node, int port)
{
  assert(server);
  if (port < 0)
    return -1;

  // uv_ip4_addr() を使うサンプルは古い!!
  struct addrinfo hints;
  struct addrinfo* res = NULL;

  memset(&hints, 0, sizeof hints);
  if (!node)
    hints.ai_family = AF_INET6; // IPv4/IPv6両対応
  hints.ai_socktype = SOCK_STREAM;
  // AI_PASSIVE をセットして node = NULLのときは, INADDR_ANY, IN6ADDR_ANY_INIT.
  hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV;

  // (node, service) の両方を NULL にしてはいけない.
  char service[11];
  sprintf(service, "%d", port);
  int err = getaddrinfo(node, service, &hints, &res);
  if (err != 0) {
    fprintf( stderr, "getaddrinfo() failed: %s\n", gai_strerror(err) );
    return -1;
  }

  // socket(), bind() を一体にしたもの.
  int r = uv_tcp_bind(server, res->ai_addr, 0);
  freeaddrinfo(res);
  if (r) {
    fprintf(stderr, "uv_tcp_bind() failed: %s\n", uv_strerror(r) );
    return -1;
  }

  return 0;
}


constexpr int PORT = 7000;
constexpr int BACKLOG = 100;

// イベントループ. グローバル.
uv_loop_t* g_loop = NULL;

// サーバ. グローバル.
uv_tcp_t* g_server = NULL;

// これをサンプルで切り替える.
extern void on_connection( uv_stream_t* server, int status );

int main()
{
  struct rlimit rlim;
  getrlimit(RLIMIT_NOFILE, &rlim);
  printf("soft = %ld, hard = %ld, FD_SETSIZE = %d\n",
         rlim.rlim_cur, rlim.rlim_max, FD_SETSIZE);

  // イベントループ
  g_loop = uv_default_loop();
  
  g_server = (uv_tcp_t*) malloc(sizeof(uv_tcp_t));
  uv_tcp_init( g_loop, g_server );  // まだソケットは作られない.

  printf("Waiting port = %d\n", PORT);
  int r = tcp_setup_server(g_server, NULL, PORT);
  if (r) {
    fprintf(stderr, "tcp_setup_server() failed.\n");
    return 1;
  }
  
  r = uv_listen( (uv_stream_t*) g_server, BACKLOG, on_connection );
  if (r) {
    // Error
    fprintf( stderr, "uv_listen() failed: %s\n", uv_strerror(r) );
    return 1;
  }

  // ループ開始
  r = uv_run(g_loop, UV_RUN_DEFAULT);
  printf("Exit uv_run()\n");  // uv_close(server) でループを抜ける.

  return r;
}
