# -*- coding:utf-8 -*-

# Fortune server
# 102 自然は衰える。しかしラチナムの輝きは永遠なり。

require 'socket'

# open(host = nil, service)
# 例  'localhost', 0 => [::1]:42543
#     0              => 0.0.0.0:37751  1引数は service の指定. IPv4専用
#                       ※これを使ってはならない。IPv6 で接続できない。ヒドい.
#     '::', 0        => [::]:34489     IN6ADDR_ANY_INIT
#                       こちらを使え. IPv4 でも接続できる。
gs = TCPServer.open('::', 0)
# ホスト名を指定した場合は、名前を引いて, IPアドレスを返す. 
#=> ["AF_INET6", 33617, "fe80::3b9a:be08:a27b:a4aa%ens33", "fe80::3b9a:be08:a27b:a4aa%ens33"]
printf "Server is on %s\n", gs.addr.inspect

while true
  # accept() は新しい TCPSocket インスタンスを返す
  Thread.start(gs.accept) do |s|
    #=> ["AF_INET6", 43915, "::ffff:127.0.0.1", "::ffff:127.0.0.1"]
    print s.addr, " is accepted\n"
    begin
      # %x{} も同じ. 標準出力 = 戻り値, 終了ステイタス = $? (Process::Status型)
      fortune = `fortune`
    rescue Errno::ENOENT
      fortune = "THE SERVER HAD A BAD DAY!\n"
    end
    s.write fortune
    s.close
  end
end

