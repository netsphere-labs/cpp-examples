
#include <config.h>

#include <assert.h>
#include <string.h>
#include <errno.h>
#include <string>
#include <vector>
#include <netdb.h>
#include <fcntl.h>
#include <gtk/gtk.h>
#include "client.h"
#include "callbacks.h"
#ifdef USE_THREAD
  #include "../../ipv6_sock/connect_to_server.h"  // blocking 版
#endif
#ifndef _WIN32
  #include <sys/socket.h>
  #include <unistd.h>
  #define closesocket close
  typedef int SOCKET;
  #define INVALID_SOCKET -1
#endif
#include "../../ipv6_sock/debug_print.h"  // sockaddr_print()
#include <map>
using namespace std;

// デバッグ関数
void print_condition(const char* mes, int condition) {
    string e;
    if (condition & G_IO_IN) e += " IN";
    if (condition & G_IO_OUT) e += " OUT";
    if (condition & G_IO_PRI) e += " PRI";
    if (condition & G_IO_ERR) e += " ERR";
    if (condition & G_IO_HUP) e += " HUP";
    if (condition & G_IO_NVAL) e += " NVAL";
    printf("%s: condition = %s\n", mes, e.c_str());
}


void error_dialog(const char* msg)
{
    assert(msg);

    GtkWidget* dialog = gtk_message_dialog_new(
                                GTK_WINDOW(top_window->self),
                                GTK_DIALOG_DESTROY_WITH_PARENT,
                                GTK_MESSAGE_ERROR,
                                GTK_BUTTONS_OK,
                                msg);
    g_signal_connect(dialog, "response",
                     G_CALLBACK(gtk_window_destroy), NULL);

    // GTK4: gtk_dialog_run() は廃止.
    gtk_widget_show(dialog);
}


#ifndef USE_THREAD

// GTK では, GIOChannel がソケットFD を wrap する.
// イベントループにイベント発生でコールバックさせるよう登録すると, event source
// id が発行される.
// map が必要
map<GIOChannel*, guint> wait_connect_events;

// 接続が確立されたもの.
pair<GIOChannel*, guint> g_connection;


void close_and_notify(bool succeeded)
{
    SOCKET fd = g_io_channel_unix_get_fd(g_connection.first);
    closesocket(fd);
    g_idle_add(succeeded ? on_thread_finished : on_connect_error,
               (void*) -1);  // emit.
}

// Callback. GIOChannel のほうが引数になる. -> 直接 g_source_remove() できない.
gboolean on_ready_to_read(GIOChannel* source, GIOCondition condition,
                          gpointer data)
{
    print_condition(__func__, condition); // DEBUG

    if ( condition & (G_IO_ERR | G_IO_HUP) ) {
        error_dialog("読み込み中にエラー発生");
        close_and_notify(false);
        return FALSE; // FALSE = The event source to be removed.
    }

    if (condition & G_IO_IN) {
        SOCKET fd = g_io_channel_unix_get_fd(g_connection.first);
        char buf[1000];
        int bytes_read = recv(fd, buf, sizeof(buf) - 1, 0);
        if (bytes_read < 0) {
            perror("recv() failed");
            error_dialog("recv() でエラー発生");
            close_and_notify(false);
            return FALSE; // FALSE = The event source to be removed.
        }
        else if ( bytes_read == 0 ) {
            // EOF
            close_and_notify(true);
            return FALSE;
        }

        assert( bytes_read > 0 );
        buf[bytes_read] = '\0';
        GtkTextBuffer* buffer = gtk_text_view_get_buffer(top_window->textView);
        assert(buffer);
        gtk_text_buffer_insert_at_cursor(buffer, buf, bytes_read);
    }

    return TRUE; // 再利用する.
}


// Callback
void on_io_destroy(void* data)
{
    // どの iochannel か不明 -> ここでソケットを close できない.
}


/**
 * gdk_input_add() が deprecated (非推奨) なので、似たようなメソッドを作る。
 * @param condition 次の bitor
 *              G_IO_IN  POLLIN と同じ. 読み込み可能. あるいは EOF
 *              G_IN_OUT POLLOUT と同じ. Blockされずに書き込み可能.
 *              G_IO_PRI POLLPRI と同じ. TCPソケットに out-of-band data がある.
 *                       See tcp(7)
 * Callback には, 次も設定される.
 *   G_IO_ERR, G_IO_HUP, G_IO_NVAL. それぞれ POLLERR, POLLHUP, POLLNVAL と同じ.
 *
 * Note.
`POLLRDHUP` は Linux 拡張. 相手方が close したか, 書き込み側を shutdown した.
    -> EOF を read しなくても検出できるようにする.
FreeBSD でも実装を議論 (FreeBSD 13 には入っていない).
    See https://reviews.freebsd.org/D29757
Solaris 11 には入っていない.
 */
pair<GIOChannel*, guint>
start_watch(SOCKET sockfd, int condition, GIOFunc proc, gpointer user_data)
{
    assert(sockfd != INVALID_SOCKET);

    // GIOChannel で wrap する
    GIOChannel* channel = g_io_channel_unix_new(sockfd);
    guint io_watch = g_io_add_watch_full(channel, G_PRIORITY_DEFAULT,
                                         (GIOCondition) condition,
                                         proc,
                                         user_data,
                                         on_io_destroy);
    assert(io_watch);
    // g_io_add_watch() 内でもrefされるので、ここでは解放する
    g_io_channel_unref(channel);

    return make_pair(channel, io_watch);
}


// Callback. 接続した or エラー. 成功の場合, g_connection を設定する.
// @param source NULL の場合がある.
gboolean on_connect(GIOChannel* source, GIOCondition condition, gpointer data)
{
    print_condition(__func__, condition); // DEBUG

    if ( condition & (G_IO_ERR | G_IO_HUP) ) {
        // 自分だけ削除する.
        SOCKET fd = g_io_channel_unix_get_fd(source);
        closesocket(fd);

        wait_connect_events.erase(source);
        if (wait_connect_events.size() == 0) {
            error_dialog("接続に失敗しました。");
            g_idle_add(on_connect_error, (void*) -1);  // emit.
        }
        return FALSE; // FALSE = The event source to be removed.
    }

    assert( wait_connect_events.size() > 0 );

    // ほかのを全部 close する
    for (auto i = wait_connect_events.begin();
                                        i != wait_connect_events.end(); i++) {
        if (i->first == source)
            continue;
        SOCKET fd = g_io_channel_unix_get_fd(i->first);
        closesocket(fd);
        g_source_remove(i->second);  // 内部で g_io_channel_unref() される.
    }
    wait_connect_events.clear();

    // Callback を改めて登録する.
    SOCKET sockfd = g_io_channel_unix_get_fd(source);
    g_connection = start_watch(sockfd, G_IO_IN, on_ready_to_read, NULL);

    return FALSE; // FALSE = The event source to be removed.
}


/**
 * @param mode  非0 = into non-blocking mode.
 * @return If failed, -1.
 */
int non_blocking( SOCKET sock, int mode )
{
    assert( sock != INVALID_SOCKET );
#ifndef _WIN32
    // UNIX
    int oldflags = fcntl(sock, F_GETFL, 0 /* dummy */);
    return fcntl(sock, F_SETFL,
                 mode ? (oldflags | O_NONBLOCK) : (oldflags & ~O_NONBLOCK) );
#else
    // Windows では「現在のモード」を得る方法はない
    u_long iMode = mode ? 1 : 0;
    int iResult = ioctlsocket( sock, FIONBIO, &iMode);
    if ( iResult != NO_ERROR )
        return -1;
    return 0;
#endif
}


// <var>hostname</var> からIPアドレスを引き, 全部に接続要求する. Non-blocking 版.
// 実際に接続したときに on_connect() が呼び出されるようにする.
// @return 少なくとも一つが接続要求が成功 = true
bool start_connect_to_server_nonblock(const char* hostname, int port)
{
    assert( hostname );
    if (port <= 0 || port > 65535 )
        return false;

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;  // IPv4/IPv6 両対応
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_NUMERICSERV;

    char service[11];
    sprintf(service, "%d", port);

    struct addrinfo* res = NULL;
    // ●ここで待ちが入る。アカン!!
    int err = getaddrinfo(hostname, service, &hints, &res);
    if (err != 0) {
        fprintf(stderr, "getaddrinfo() failed: %s\n", gai_strerror(err));
        error_dialog("ホストまたはポートが不正です。");
        return false;
    }

    SOCKET sockfd = INVALID_SOCKET;
    bool succeeded = false;
    for (addrinfo* ai = res; ai; ai = ai->ai_next) {
        sockaddr_print("connect...", ai->ai_addr, ai->ai_addrlen);

        sockfd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
        if ( sockfd == INVALID_SOCKET ) {
            freeaddrinfo(res);
            return false;
        }

        non_blocking(sockfd, 1);  // 非ブロックモードに切り替え.
        int r = ::connect(sockfd, ai->ai_addr, ai->ai_addrlen);
        if (r < 0) {
            if (errno == EINPROGRESS || errno == EINTR ) { // 即時確立しなかった
                // Non-blocking モードのときは, シグナル割り込みは EINPROGRESS
                // と同じでよいみたい.
                // 成功. 書き込み可能になるまで待つ.
                pair<GIOChannel*, guint> ev =
                    start_watch(sockfd, G_IO_OUT, on_connect, NULL);
                wait_connect_events.insert(ev);
                // Non-blocking 版では, 全部を試す.
                succeeded = true;
                continue;
            }
            closesocket(sockfd);
            sockfd = INVALID_SOCKET;
            continue;
        }
        else {
            // 即時成功の場合, 他のを close して, g_connection を設定.
            on_connect(NULL, G_IO_OUT, NULL);
            return true;
        }
    }
    freeaddrinfo(res);  // もういらん.

    return succeeded;
}
#endif // !USE_THREAD


#ifdef USE_THREAD
// Callback. 別スレッド: 開始ボタンのクリック
gpointer start_get_fortune(gpointer data)
{
    assert(data);
    Param* param = (Param*) data;
    assert(param->port > 0);

    // Block する.
    SOCKET sockfd = connect_to_server(param->hostname.c_str(), param->port);
    if ( sockfd == INVALID_SOCKET ) {
        g_idle_add(on_connect_error, (void*) errno);  // これでメインスレッドへの emit.
        return nullptr;
    }

    char fortune_buf[1000];
    char* p = fortune_buf;
    while (true) {
        int r = recv(sockfd, p,
                     sizeof(fortune_buf) - (p - fortune_buf) - 1, 0);
        if (r < 0) {
            if (errno == EINTR)
                continue;
            g_idle_add(on_connect_error, (void*) errno);  // emit
            closesocket(sockfd);
            return nullptr;
        }
        p[r] = '\0';
        if ( !r )
            break;
        p += r;
    }

    closesocket(sockfd);

    // シグナルを発生
    g_idle_add(on_thread_finished, strdup(fortune_buf));
    return nullptr;
}
#endif // USE_THREAD
