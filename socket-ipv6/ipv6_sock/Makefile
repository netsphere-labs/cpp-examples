
TARGETS = sock_client server1 server2

CC = gcc
CXXFLAGS = -g -Wall -Wextra
LDFLAGS = -lstdc++
#-lws2_32

#.cpp.o:
#	$(CC) $(CFLAGS) -c $< -o $@
.o:
	$(CC)  -o $@ $^ $(LDFLAGS)

all: $(TARGETS)

sock_client: sock_client.o debug_print.o connect_to_server.o
server1: server1.o debug_print.o
server2: server2.o debug_print.o

connect_to_server.o: connect_to_server.cpp connect_to_server.h
sock_client.o: sock_client.cpp debug_print.h
server1.o: server1.cpp debug_print.h
server2.o: server2.cpp debug_print.h
debug_print.o: debug_print.cpp debug_print.h

clean:
	rm -f $(TARGETS) a.out *~ *.o

