
# クライアント側 - 非同期ソケットのサンプル C++, Qt5

「非同期I/O asynchronous I/O」とひと言でいっても、やり方はいくつもある. が, まともに動く方法は少ない.

クライアント側だと, <code>connect()</code> がブロックするので, non-blocking モードにしたうえで呼出し, その後 <code>select()</code> で待ち合わせするのが, まず思いつく.

getaddrinfo() が block してしまうので、これはダメ.
   -> スレッドを作って, blocking モードで通信するしかない.




## Qt6 でビルド失敗 -> workaround あり.

2022.11:
  Fedora 37, Qt Creator 8.0, Qt 6.3 の組み合わせで, ビルドに失敗.

mainwindow.ui ファイルで <code>onHelpAbout()</code> 等々のスロットが <code>QMainWindow</code> クラスに見つからないエラーになる.

同じ症状の質問あり. https://stackoverflow.com/questions/67643618/qt6-migration-uic-generated-code-fails-to-compile-connect-slots-updated
  -> .ui で connect するのは諦めて, コードビハインドのほうに書け. ヒドい

Qt5 なら .ui に書いても見つけてくれる. ソースコードで connect すれば, 両方でビルド可能.

