
// tcp-server.cpp
// 1000 クライアントを超える同時接続したいときは, ulimit -S -n 10000 しろ.

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <uv.h>
#include <map>
#include "server-common.h"
#include "noncopyable.h"
using namespace std;


void after_server_close(uv_handle_t* handle)
{
  free(g_server); g_server =NULL;
}

void server_shutdown()
{
  uv_close((uv_handle_t*) g_server, after_server_close);
  //free(g_server); g_server =NULL;
}


class ClientSession: private noncopyable
{
  // uv_tcp_t < uv_stream_t < uv_handle_t
  uv_tcp_t* m_handle;  // 所有する

  static map<uv_tcp_t*, class ClientSession*> _session_list;

protected:
  ClientSession(uv_tcp_t* h): m_handle(h) {
    assert( h != NULL );
  }

public:
  static ClientSession* new_instance(uv_tcp_t* h) {
    assert(h);
    ClientSession* self = new ClientSession(h);
    _session_list.insert(make_pair(h, self));
    return self;
  }
  
  ~ClientSession() {
    assert(m_handle);
    _session_list.erase(m_handle);
    free(m_handle); m_handle = NULL;
  }

  // @return If error, non-zero
  int read_start() {
    assert( m_handle != NULL );
    // Read data from an incoming stream. alloc_buffer() で領域を確保する.
    return uv_read_start( (uv_stream_t*) m_handle, _alloc_buffer,
                          (uv_read_cb) _on_read );
  }
  
  void on_receive(ssize_t nread, const uv_buf_t* buf) {
    assert(buf);
  
    if (nread < 0) {
      if (nread != UV_EOF) {
        fprintf(stderr, "recv() EOF or error: %s\n", uv_err_name(nread) );
      }
      // If error, *user* should call uv_close().
      shutdown();
    }
    else if (nread > 0) {
      if (buf->base[0] == 'X') {
        printf("Get client shutdown message.\n");
        shutdown();
      }
      else if (buf->base[0] == 'Z') {
        printf("Get server shutdown message.\n");
        server_shutdown();
      }
      else {
        // 書き込みもややこしい.
        uv_write_t* req = (uv_write_t*) malloc(sizeof(uv_write_t));
        uv_buf_t wbuf = uv_buf_init(buf->base, nread); // ローカル変数でよい.
        uv_write(req, (uv_stream_t*) m_handle, &wbuf, 1, _after_write);
      }
    }
  }
  
  void shutdown() {
    assert( m_handle != NULL );
    uv_close((uv_handle_t*) m_handle, _after_close);
  }

private:
  // Callback.
  // uv_write_t < uv_req_t
  static void _after_write(uv_write_t* req, int status)
  {
    assert(req);
    
    if (status) {
      fprintf(stderr, "Write error: %s\n", uv_strerror(status));
      // See src/unix/stream.c
      ClientSession* v = _session_list.at((uv_tcp_t*) req->handle);
      v->shutdown();
    }
    free(req);
  }
  
  // Callback
  static void _after_close(uv_handle_t* handle) {
    ClientSession* v = _session_list.at((uv_tcp_t*) handle);
    delete v;
  }
  
  /**
   * Callback. stream->alloc_cb(stream, 64 * 1024, &buf) の形で呼び出される.
   * @param handle  stream
   * @param suggested_size  64 * 1024 固定.
   * @param [out] buf  バッファを格納. .base == NULL || .len == 0 で戻すと,
   *                   read_cb() に UV_ENOBUFS エラーを通知.
   */
  static void _alloc_buffer( uv_handle_t* handle, size_t suggested_size,
                             uv_buf_t* buf)
  {
    //printf("alloc_buffer() called: .\n");
    buf->base = (char*) malloc(suggested_size); // ●●重い! 自分で管理が必要.
    buf->len = suggested_size;
  }

  /**
   * Callback.
   * EOF の場合, uv__stream_eof() から呼び出される. 第2引数が UV_EOF.
   *             この場合も, 領域確保済み.
   * それ以外, uv__read() から呼び出される.
   * @param nread < 0 の場合, EOF or エラー. > 0 の場合, 読み出せる.
   *              == 0 の場合, ソケットが non-blocking で読み出せるデータなし.
   */
  static void _on_read( uv_stream_t* client, ssize_t nread, uv_buf_t* buf ) {
    ClientSession* v = _session_list.at((uv_tcp_t*) client);
    v->on_receive(nread, buf);

    // エラーの場合も, 領域を解放しなければならない. ●●重い処理.
    if (buf->base) {
      free(buf->base); buf->base = NULL;
    }
  }
};

map<uv_tcp_t*, class ClientSession*> ClientSession::_session_list;

void on_connection( uv_stream_t* server, int status )
{
  if ( status != 0 ) {
    // エラーの場合, エラー名を得る uv_err_name() or エラーメッセージを得る
    // uv_strerror() を使う.
    fprintf(stderr, "New connection error: %s\n", uv_strerror(status) );
    return;
  }

  uv_tcp_t* client = (uv_tcp_t*) malloc(sizeof(uv_tcp_t));
  uv_tcp_init( g_loop, client );

  int r = uv_accept(server, (uv_stream_t*) client);
  if (r) {
    fprintf(stderr, "uv_accept() failed: %s\n", uv_strerror(r) );
    free(client);
    return;
  }

  ClientSession* session = ClientSession::new_instance(client); // client所有渡す
  r = session->read_start();
  assert( r == 0 );
}

