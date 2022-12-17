
// tcp-server.cpp

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <uv.h>
#include "server-common.h"

/**
 * Callback. stream->alloc_cb(stream, 64 * 1024, &buf) の形で呼び出される.
 * @param handle  stream
 * @param suggested_size  64 * 1024 固定.
 * @param [out] buf  バッファを格納. .base == NULL || .len == 0 で戻すと,
 *                   read_cb() に UV_ENOBUFS エラーを通知.
 */
void alloc_buffer( uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
{
  printf("alloc_buffer() called: .\n");
  
  buf->base = (char*) malloc(suggested_size);  // ●●これは重い! 自分で管理が必要.
  buf->len = suggested_size;
}


// クライアント TCPハンドル. ●●複数クライアントとの通信では, 配列にする.
// uv_tcp_t < uv_stream_t < uv_handle_t
uv_tcp_t* g_client = NULL;

void after_close(uv_handle_t* handle)
{
  free(g_client); g_client = NULL;
}

void client_shutdown()
{
  uv_close((uv_handle_t*) g_client, after_close);
  //free(g_client); g_client = NULL; // ここで free() してはいけない!
}


// Callback.
void after_write(uv_write_t* req, int status)
{
  if (status) {
    fprintf(stderr, "Write error: %s\n", uv_strerror(status));
    client_shutdown();
  }
  free(req);
}


void after_server_close(uv_handle_t* handle)
{
  free(g_server); g_server =NULL;
}

void server_shutdown()
{
  uv_close((uv_handle_t*) g_server, after_server_close);
  //free(g_server); g_server =NULL;
}


/**
 * Callback.
 * EOF の場合, uv__stream_eof() から呼び出される. 第2引数が UV_EOF.
 *             この場合も, 領域確保済み.
 * それ以外の場合, uv__read() から呼び出される.
 * @param nread < 0 の場合, EOF or エラー. > 0 の場合, 読み出せる.
 *              == 0 の場合, ソケットが non-blocking で読み出せるデータなし.
 */
void echo_read( uv_stream_t* client, ssize_t nread, uv_buf_t* buf )
{
  assert(buf);
  
  if (nread < 0) {
    fprintf(stderr, "recv() EOF or error: %s\n", uv_err_name(nread) );

    // If error, *user* should call uv_close().
    client_shutdown();
  }
  else if (nread > 0) {
    if (buf->base[0] == 'X') {
      printf("Get client shutdown message.\n");
      client_shutdown();
    }
    else if (buf->base[0] == 'Z') {
      printf("Get server shutdown message.\n");
      server_shutdown();
    }
    else {
      // 書き込みもややこしい.
      uv_write_t* req = (uv_write_t*) malloc(sizeof(uv_write_t));
      uv_buf_t wbuf = uv_buf_init(buf->base, nread); // ローカル変数でよい.
      uv_write(req, client, &wbuf, 1, after_write);
    }
  }

  // エラーの場合も, 領域を解放しなければならない. ●●重い処理.
  if (buf->base) {
    free(buf->base); buf->base = NULL;
  }
}


void on_connection( uv_stream_t* server, int status )
{
  if ( status != 0 ) {
    // エラーの場合, エラー名を得る uv_err_name() or エラーメッセージを得る
    // uv_strerror() を使う.
    fprintf(stderr, "New connection error: %s\n", uv_strerror(status) );
    return;
  }

  g_client = (uv_tcp_t*) malloc(sizeof(uv_tcp_t));
  uv_tcp_init( g_loop, g_client );

  int r = uv_accept(server, (uv_stream_t*) g_client);
  if (r) {
    fprintf(stderr, "uv_accept() failed: %s\n", uv_strerror(r) );
    free(g_client); g_client = NULL;
    return;
  }

  // Read data from an incoming stream. alloc_buffer() で領域を確保する.
  r = uv_read_start( (uv_stream_t*) g_client, alloc_buffer,
                     (uv_read_cb) echo_read );
  assert( r == 0 );
}

