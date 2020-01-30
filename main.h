#ifndef MAIN_H
#define MAIN_H

#include "awl.h"
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h> 
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <netinet/in.h>
 #include <netinet/ip.h> 
 #include <time.h>
 #include <poll.h>
#include <asm/byteorder.h> 
#include <pthread.h>
#include <netdb.h>
#include <signal.h>
#include  <sys/ioctl.h>
#include <netinet/tcp.h> 
#include <sqlite3.h>

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"


#define EPOLL_TIMEOUT 3600 // seconds
#define EPOLL_EVENTS 20
#define EPOLL_MIN_CHECK 100 // milliseconds

#define SESSION_LIMIT 40 // percent
#define SESSION_MAX (1024 * SESSION_LIMIT / 100) // number
#define SOCKS5_NONE 1
#define SOCKS5_HELLO 2
#define SOCKS5_AUTH 3
#define SOCKS5_CONNECT 4
#define SOCKS5_CONNECTED 5

#define TCP_CLOSE 2
#define TCP_CONNECTED 1

#define NONE 0
#define ALIVE 1
#define CLOSED 2

#define TERMINATE 3
#define BLOCKED 4

#define CONNECT_COMMAND 1
#define BIND_COMMAND 2
#define UDP_COMMAND 3

#define SEND_BUF_DEFAULT 163840 // bytes
#define MAX_BUF 65535  // bytes

#define TCP_INIT_TIMEOUT 20 // seconds ~net.inet.tcp.keepinit
#define TCP_IDLE_TIMEOUT 3600 // seconds ~net.inet.tcp.keepidle
#define TCP_CLOSE_TIMEOUT 20 // seconds
#define TCP_KEEP_TIMEOUT 300 // seconds

int server,version;

int epollfd;
short myport;

struct toks{
	char **tokens;
	int count;
};
struct blocked_ip{
	char domain[1024];
	char ip[1024];
	struct blocked_ip *next;
};
struct tcp_session {
    int uid;
    time_t time;
    int version;
  

 
    long long last_keep_alive;

    uint64_t sent;
    uint64_t received;

    union {
		__be32 ip4; // network notation
        struct in6_addr ip6;
    } saddr;
    __be16 source; // network notation

    union {
        __be32 ip4; // network notation
        struct in6_addr ip6;
		 } daddr;
		 
    __be16 dest; // network notation
	//char destination[256];
    uint8_t client_state;
	uint8_t remote_state;
    uint8_t socks5;
	//uint8_t data[65535];
	
   struct segment *forward;
};
struct session {
    uint8_t protocol;
	int id;
    int client;
	int remote;
	union{
		struct tcp_session tcp;
		
	};
    struct epoll_event ev_c;
	struct epoll_event ev_r;
    struct session *next;
	
};
struct sock_hello{
	uint8_t v;
	uint8_t m;
	uint8_t a;
}sh;


struct context {
    pthread_mutex_t lock;
    int pipefds[2];
    int stopping;
	struct blocked_ip *blocked;
	struct Node *root;
	
    struct session *session;
};
struct arguments {
    int server;
	short port;
	int scount;
	long send;
	long rcvd;
    struct context *ctx;
};
struct segment {
    uint16_t len;
    uint16_t sent;
   
    uint8_t *data;
    struct segment *next;
};
long get_ms();
void startserver(struct arguments *args);
int check_tcp_session(const struct arguments *args, struct session *s,
                      int sessions, int maxsessions,int epollfd);
void handle_Client(struct arguments* args,struct sockaddr_in *clientaddr,int cs,const int epollfd);
void handle_sock_init(struct session *c);
void handle_sock_command(struct arguments *args,  struct session* c,int epollfd);
int connectToServer(struct session *c);
int sendToremote(struct session *c);
int sentToClient(struct session *c);
 int SetSocketBlockingEnabled(int fd, int blocking);
 int monitor_tcp_session(const struct arguments *args, struct session *s, int epoll_fd);
 int addblockedip(const struct arguments *args,const char *ip);
 void queue_client_data(const struct arguments *args,struct tcp_session *cur,
               const uint8_t *data, uint16_t datalen);
			


#endif //MAIN_H