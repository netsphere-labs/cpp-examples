
# 現代のソケット

## 1. ipv6_sock/

解説 <a href="https://www.nslabs.jp/socket.rhtml">IPv6ソケットプログラミング [C++]</a>


## 2. nonblock-gtk/

GTK4 クライアントサンプル. non-blocking モードによる非同期ソケットサンプル.

getaddrinfo() がブロックしてしまうので、正しくない.
記事内で解説。

●● TODO: <s>.ui ファイルもインストールに加えないといけないのでは?</s> 済み


## 3. nonblock-socket-sample-qt/

QT5 / QT6 両対応.
スレッドを作って blocking モードでやる。ソケットの non-blocking モードだけが非同期 I/O ではない.

