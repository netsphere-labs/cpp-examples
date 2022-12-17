
TARGETS = 1-tcp-server 1-client multi-tcp-server multi-client

.o:
	$(CC)  -o $@ $^ $(LDFLAGS)

all: $(TARGETS)

CXXFLAGS = -g -Wall -Wextra -Wno-unused-parameter -I../socket-ipv6/ipv6_sock
LDFLAGS = -lstdc++ 
#-lws2_32

1-tcp-server: 1-tcp-server.o server-common.o
	gcc -o $@ $^ $(LDFLAGS) -luv

1-client: 1-client.o ../socket-ipv6/ipv6_sock/connect_to_server.o ../socket-ipv6/ipv6_sock/debug_print.o

multi-tcp-server: multi-tcp-server.o server-common.o
	gcc -o $@ $^ $(LDFLAGS) -luv

multi-client: multi-client.o ../socket-ipv6/ipv6_sock/connect_to_server.o ../socket-ipv6/ipv6_sock/debug_print.o

clean:
	rm -f $(TARGETS) *.o
