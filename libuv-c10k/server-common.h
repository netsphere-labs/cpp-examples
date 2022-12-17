

extern int tcp_setup_server(uv_tcp_t* server, const char* node, int port);

extern uv_tcp_t* g_server;

// イベントループ.
extern uv_loop_t* g_loop;

