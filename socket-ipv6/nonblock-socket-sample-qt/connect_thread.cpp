
#include "connect_thread.h"
#include "../ipv6_sock/connect_to_server.h"
#ifndef _WIN32
  #include <sys/socket.h>
  #include <unistd.h>
  #define closesocket close
#endif
using namespace std;

ConnectThread::ConnectThread(QObject* parent):
  QThread(parent)
{
}

ConnectThread::~ConnectThread()
{
}

// このメソッドはメインスレッドで呼び出される.
void ConnectThread::requestNewFortune(const string& hostName, int port)
{
    assert(port > 0);

    // パラメータはメンバ変数にて渡す
    this->hostName = hostName;
    this->port = port;
    // スレッドを起動.
    start();
}

// Starting point.
void ConnectThread::run()
{
    SOCKET sockfd = connect_to_server(hostName.c_str(), port);
    if ( sockfd == INVALID_SOCKET ) {
        emit error(errno, tr("client error") );
        return;
    }

    char fortune_buf[1000];
    char* p = fortune_buf;
    while (true) {
        int r = recv(sockfd, p,
                     sizeof(fortune_buf) - (p - fortune_buf) - 1, 0);
        if (r < 0) {
            if (errno == EINTR)
                continue;
            emit error(errno, tr("read() error") );
            closesocket(sockfd);
            return;
        }
        p[r] = '\0';
        if ( !r )
            break;
        p += r;
    }

    closesocket(sockfd);

    // シグナルを発生
    emit fortuneGot(fortune_buf);
}
