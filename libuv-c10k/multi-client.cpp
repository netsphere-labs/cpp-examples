
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#ifndef _WIN32
  #include <netdb.h>
  #include <sys/resource.h>
  #include <unistd.h>
  #define closesocket close
#else
  #include <process.h>
  #include <synchapi.h>
#endif
#include "connect_to_server.h"

/*
select() で確認できるソケットの数は, OS ごとに異なる.
file descriptor の "値" が FD_SETSIZE まで.
    -> 1,000 を超えるときは select() を使うな.

z/OS FD_SETSIZE で変更。デフォルトでは255.

Linux ドキュメントでは, FD_SETSIZE マクロを変更できることになっているが,
       __FD_SETSIZE = 1024 決め打ち. gcc -Dオプションに与えても変更できない.
         -> Red Hatのドキュメントで, 決め打ちで変更できない解説あり.
   /usr/include/bits/posix1_lim.h ファイルで,
      _POSIX_OPEN_MAX = 16 or 20
      _POSIX_FD_SETSIZE = _POSIX_OPEN_MAX ... マジか -> これは使われていない.
  ○ file descriptor の上限
   ulimit -n コマンド (bash の組込みコマンド) で, ログイン端末ごとのFD上限 = 1024
   ハードリミットを上限として, 変更可能.
   ulimit -Hn ハードリミット表示 -> 524288
   $ ulimit -S -n 10000 で変更する. -S 付けないとハードリミットも引き下がる!
   システム全体の制約
       $ cat /proc/sys/fs/file-max 
       9223372036854775807
   プログラムから変更する ulimit(3) は廃止予定. POSIX でも廃止予定. 

FreeBSD コンパイル時に FD_SETSIZE マクロを変更できる. デフォルト 1024
*/

/*
Windows には fork() はないし、似た関数もない。
   _spawn*() -> 別プロセスを生成. 内部で CreateProcess() を呼び出す.

UNIX & Windows 共通:
   exec*()   -> POSIX, Windows 共通なのは次の6ヶ
      -> The exec family *replace* the current process image. 制御が帰ってこない. 用途違い.
      execl()    execv()
      execle()   execve()
      execlp()   execvp()

   popen() 単に新しいプロセスを起動して, パイプ接続. 引数が空白を含むとダメっぽい
   system() 単に新しいプロセスを起動する. 引数が空白を含むとダメっぽい

UNIX only:
   vfork()  POSIX.1-2001 で廃止予定, -2008 で廃止 (削除).
       -> CLONE_VM | CLONE_VFORK | SIGCHLD フラグを指定して clone(2) を呼び
          出すのと等価.
   clone(2)  POSIX にない. Linux 特有。システムコール.
   posix_spawn(3)  -> 別プロセスを生成. fork(2)+exec(3) と等価な機能
*/

constexpr int CLIENT_SIZE = 1000;
// C100K  => 50 は OK. 100 にすると失敗発生しだす.
//     connect() failed: Cannot assign requested address
//     Addr is...: AF_INET6 [::1]:7000
constexpr int CHILD_NUMBER = 100; 
  
// @return 0 が成功.
int make_children(const char* argv0, const char* hostname)
{
  assert(hostname);
  
#ifndef _WIN32
  struct rlimit rlim;
  getrlimit(RLIMIT_NOFILE, &rlim);
  printf("soft = %ld, hard = %ld, FD_SETSIZE = %d\n",
         rlim.rlim_cur, rlim.rlim_max, FD_SETSIZE);
#endif

  for (int k = 0; k < CHILD_NUMBER; k++) {
#ifndef _WIN32
    // UNIX: fork() + exec*() が伝統的な方法. posix_spawn(3) でもよい.
    pid_t pid = fork();
    if (pid == -1) {
      fprintf(stderr, "fork() failed.\n");
      return 1;
    }
    else if (pid == 0) {
      // child
      execl(argv0, argv0, "-c", hostname, NULL); // restart
      abort(); // ここには来ない
    }
    // 親プロセス
#else
    // Windows: _spawn* 系かCreateProcess() を直接使う.
    _spawnl(_P_NOWAIT, argv0, argv0, "-c", hostname, NULL);
#endif
  }
  return 0;
}


SOCKET sockfd[CLIENT_SIZE];

// @return 0 が成功.
int test_connect_server(const char* hostname)
{
  assert(hostname);
  
#ifdef _WIN32 
  // Initialize Winsock 
  WSADATA wsaData; 
  int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData); 
  if (iResult != 0) { 
    printf("WSAStartup failed: %d\n", iResult); 
    return 1; 
  }     
#endif

  for (int i = 0; i < CLIENT_SIZE; i++) {
    sockfd[i] = connect_to_server(hostname, 7000);
    if (sockfd[i] == INVALID_SOCKET) {
      fprintf(stderr, "%d: connect_to_server() failed.\n", i); //=> 1020 でコケる
      return 1;
    }
  }

  const char ch = 'A';
  for (int i = 0; i < CLIENT_SIZE; i++) {
    // 送受信
    send(sockfd[i], &ch, 1, 0);
  }
  printf("send finished.\n");
#ifndef _WIN32
  sleep(5);
#else
  Sleep(5 * 1000);
#endif
  for (int i = 0; i < CLIENT_SIZE; i++) {
    char rch = '\0';
    recv(sockfd[i], &rch, 1, 0);
    if (ch != rch) {
      fprintf(stderr, "%d: char from server = %c\n", i, rch);
      exit(1);
    }
  }
  printf("receive finished.\n");
#ifndef _WIN32
  sleep(5);
#else
  Sleep(5 * 1000);
#endif
/*
#if 0
  // 明示的に shutdown する
  ch = 'X';
  send(sockfd, &ch, 1, 0);
#endif  
  // 明示的に server を shutdown する
  ch = 'Z';
  send(sockfd, &ch, 1, 0);
*/  
  for (int i = 0; i < CLIENT_SIZE; i++) {
    closesocket(sockfd[i]);
  }
  
  return 0;
}


// a.out hostname  -- 親. hostname を省略したときは localhost
// 子:
//   a.out -c hostname  hostname 必須
int main(int argc, char* argv[])
{
  if (argc >= 2 && !strcmp(argv[1], "-c") ) {
    if (!argv[2]) {
      fprintf(stderr, "Hostname missing\n");
      exit(1);
    }
    return test_connect_server(argv[2]);
  }
  
  return make_children(argv[0], argc >= 2 ? argv[1] : "localhost");
}

