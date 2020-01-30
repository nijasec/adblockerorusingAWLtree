#include "main.h"

void ipv6_to_str_unexpanded(char *str, const struct in6_addr *addr)
{
	sprintf(str, "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
		(int) addr->s6_addr[0], (int) addr->s6_addr[1],
		(int) addr->s6_addr[2], (int) addr->s6_addr[3],
		(int) addr->s6_addr[4], (int) addr->s6_addr[5],
		(int) addr->s6_addr[6], (int) addr->s6_addr[7],
		(int) addr->s6_addr[8], (int) addr->s6_addr[9],
		(int) addr->s6_addr[10], (int) addr->s6_addr[11],
		(int) addr->s6_addr[12], (int) addr->s6_addr[13],
		(int) addr->s6_addr[14], (int) addr->s6_addr[15]);
}
struct toks* strtotok(char *in_str,const char delim,int len)
{
//	printf("len");
	struct toks *t;
	char *temp;
	int i,j,k=0,count=0;
	if(len==0){
		//printf("len 0");
		return NULL;
	}
		//printf("\nnot len 0");
		//return NULL;
	temp=(char *)malloc(len);
	t=(struct toks *)malloc(sizeof(struct toks));
	for(i=0;i<len;i++)
		if(delim == in_str[i])count++;
	
	
	//return NULL;
	t->count=(count==0)?0:count+1;
	//printf("count:%d",count);
	if(t->count!=0)
	{
	t->tokens=(char **)malloc(sizeof(char *)*(t->count));
	for(i=0;i<len;i++)
	{
		memset(temp,0,len);
		 j=0;
        while(in_str[i]!=delim && i<len){            temp[j++]=in_str[i++];        }
		//temp[j]=0;//null at end
		t->tokens[k++]=strdup(temp);
	}
	
	}
	return t;
	
}
int  domaintoip(char *domain,char *ip)
{
	struct addrinfo *result,hints;
	int rv;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = PF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
 if ( (rv = getaddrinfo( domain , "http" , &hints ,&result)) != 0) 
	{
		//fprintf(stderr, &quot;getaddrinfo: %s\n&quot;, gai_strerror(rv));
		return -1;
	}
//	h = (struct sockaddr_in *) servinfo->ai_addr;

if(result == NULL)
{
	printf("host NOT found");
					
					freeaddrinfo(result);
					return;
	
}
else
{
	if (result->ai_family == AF_INET)
					{
						inet_ntop(AF_INET,&((struct sockaddr_in *)result->ai_addr)->sin_addr, ip, 255);
						
						// printf("IP:%s\n",ip);
					}
					else
					{
						//inet_ntop(AF_INET6, (struct sockaddr_in6 *)servinfo->ai_addr, address, sizeof(address));
						//c->tcp.daddr.ip6 = ((struct sockaddr_in6*) servinfo->ai_addr)->sin6_addr;//*(struct in6_addr *) servinfo->ai_addr;
						//c->tcp.version = 6;
					}
}
	
	
	freeaddrinfo(result); // all done with this structure
  
}
void trim(char *src)
{
    int i, len;
    len = strlen(src);

    for(i = 0; i < len; i++) {
        if(src[i] == ' ') {
            continue;
        }
        if(src[i] == '\n') {
			src[i]=0;
            return;
			
        }
        //printf("%c", src[i]); // prints: host=1.2.3.4
    }
}


void printlog(struct session *c)
{
	char log[1000];
						char source[256];
						char dest[256];
						inet_ntop(AF_INET, &c->tcp.saddr.ip4, source, sizeof(source));
						inet_ntop(AF_INET, &c->tcp.daddr.ip4, dest, sizeof(dest));
						time_t t;
						time(&t);
						
						sprintf(log,"%s Protocol: %d IPVersion:%d Source Address:%s:%d --> Destination Address:%s:%d Send:%ldKb Recieved:%ldKb",ctime(&t),c->protocol,c->tcp.version,source,ntohs(c->tcp.source),dest,ntohs(c->tcp.dest),c->tcp.sent/1024,c->tcp.received/1024);
						//printf(GRN "\r%s",log);
						
						//printf("%s",log);
}
void clear_tcp_data(struct tcp_session *cur)
{
	struct segment *s = cur->forward;
	while (s != NULL)
	{
		struct segment *p = s;
		s = s->next;
		free(p->data);
		free(p);
	}
}

int monitor_tcp_session(const struct arguments *args, struct session *s, int epoll_fd) {
 unsigned int events = EPOLLERR;

   
   if(s->tcp.remote_state==ALIVE)
   {
	   events = events | EPOLLIN;
   }
   else
	   return 0;
        // Check for outgoing data
        if (s->tcp.forward != NULL) {
           
                events = events | EPOLLOUT;
           
        }
    

    if (events != s->ev_r.events) {
        s->ev_r.events = events;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, s->remote, &s->ev_r)) {
            s->tcp.remote_state = CLOSED;
        //   ALOG("epoll mod tcp error %d: %s", errno, strerror(errno));
        }
		//else
		//printf("mode changed");
    }
	return 1;
}
void clean_all(struct arguments *args)
{
struct session *sl = NULL;
struct session	*s = args->ctx->session;
			while (s != NULL)
			{
				int del = 0;

					if (sl == NULL)
						args->ctx->session = s->next;
					else
						sl->next = s->next;

					struct session *c = s;
					s = s->next;
					if (c->protocol == IPPROTO_TCP)
					{
						if(c->client>0)
						close(c->client);
						if(c->remote>0)
						close(c->remote);
						args->send+=c->tcp.sent;
						args->rcvd+=c->tcp.received;
						printlog(c);
						clear_tcp_data(&c->tcp);
						//close(args->server);
					printf("\rterminated session id:%d ",c->id);
				//	printf(log);
					free(c);
					}
					
				
				
			}
			//delete BST
			deleteTree(args->ctx->root);
			
}


void *threadfunc(void *param)
{
	struct arguments *args = (struct arguments *) param;
	printf("i am in thread");
	startserver(args);
	return NULL;
}

void sig_handler(int signum)
{
	printf("error=%d", signum);
}
char getByte(int socket)
{
	char buf[1], n;
	if (socket > 0)
	{
		n = read(socket, buf, 1);
		if (n > 0)
			return buf[0];
		else
			return 0x00;
	}
}

int main(int argc, char **argv)
{
	short server_port = 1081;
	pthread_t mythread;
	printf("This is a sock5 server using epoll on port:%d", server_port);
	version = 4;
	signal(SIGPIPE, sig_handler);
	// Get TCP socket
	if ((server = socket(version == 4 ? PF_INET : PF_INET6, SOCK_STREAM, 0)) < 0)
	{
		printf("socket error %d: %s", errno, strerror(errno));
		return -1;
	}
	int flags = fcntl(server, F_GETFL, 0);
	if (flags < 0 || fcntl(server, F_SETFL, flags &~O_NONBLOCK) < 0)
		printf("fcntl server ~O_NONBLOCK error %d: %s",
			errno, strerror(errno));

	struct arguments *args = (struct arguments *) malloc(sizeof(struct arguments));
	struct context *ctx = (struct context *) malloc(sizeof(struct context));
	memset(ctx, 0, sizeof(struct context));
	args->ctx = ctx;
	args->server = server;
	args->port = server_port;
	myport = server_port;
	if (pipe(args->ctx->pipefds) < 0)
	{
		int err = errno;

		printf("pipe failed");
	}
	int a=0;
	char line[1000];
	char ipline[256];
	char *token;
	char *rs;
	
	
	//tryin sqlite
	sqlite3 *db;
	int rc;
   rc = sqlite3_open("data.db", &db);
   if( rc ){
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      exit(0);
   }else{
      fprintf(stderr, "Opened database successfully\n");
   }
   char *err_msg=0;
   const char *q_table="CREATE TABLE IF NOT EXISTS blockedip(id INTEGER PRIMARY KEY,domain TEXT NOT NULL,ip TEXT NOT NULL,status INTEGER DEFAULT 1);";
    rc = sqlite3_exec(db, q_table, 0, 0, &err_msg);
    
    if (rc != SQLITE_OK ) {
        
        printf("SQL error: %s\n", err_msg);
        
        sqlite3_free(err_msg);        
        sqlite3_close(db);
        
        return 1;
    } 
	printf("table created");
	const char *sql ="CREATE UNIQUE INDEX idx_blockedip_ipdomain ON blockedip (domain,ip);";
	sql="SELECT * from blockedip;";
	//rc=
	/*rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
	 if (rc != SQLITE_OK ) {
        
        printf( "Failed to indexing data\n");
       printf("SQL error: %s\n", err_msg);

        sqlite3_free(err_msg);
        sqlite3_close(db);
        
        return 1;
    } */
	/* const char *sql = "INSERT INTO blockedip(domain,ip) VALUES('dummy','dummyip');";
         printf("inserrint"); 
    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    printf("inserrint");
    if (rc != SQLITE_OK ) {
        
        printf( "Failed to insert data\n");
       printf("SQL error: %s\n", err_msg);

        sqlite3_free(err_msg);
        sqlite3_close(db);
        
        return 1;
    } */
  
  
 //  sqlite3_exec(&db,q_table,NULL,NULL,NULL);
   sqlite3_close(db);
	
		int r,number=0,i,slen=0;
	FILE *fp2=fopen("blockedlist.txt","r");
//	struct node *root=NULL;
struct Node *root=NULL;
	time_t ti=time(NULL);
	printf("\ntime:%ld",ti);
	printf("\ntime:%ld",ti);
	printf("\ntime:%ld",ti);
	if(fp2!=NULL)
	{
		char b[1000];
		printf("getms:%u",get_ms());
		do{
			number++;
				
			rs=fgets(b,sizeof(b),fp2);
			if(line[0]=='#')
				continue;
				//trim(b);
				slen=strlen(b);
				//for(int i=0;i<slen;i++);
			//	line[]
			if(b[slen-1]=='\n')//cut new line
				b[slen-1]='\0';//null
				
		root=insert(root,b,slen);
	
			args->ctx->root=root;
				
		}while(rs!=NULL);
		//exit(0);
	}
//printf("insertin fininsdw\n");

	fclose(fp2);
	printf("getms:%u",get_ms());
	ti=time(NULL);
	printf("time:%ld",ti);
	printf("\nlist count:%d\n",number);
	//inorder(root);
	//preOrder(root);
	pthread_create(&mythread, NULL, threadfunc, args);
	
	char command[100];
	char option=0;
	
	while (option != '4')
	{
		//memset(command,0,sizeof(command));
		printf("\n 0 for options:");
		
		scanf("%c", &option);
		switch(option)
		{
			case '0':
			printf("\n 0 for options:");
			printf("\n 1 to print domains list");
			printf("\n 2 Dashboard");
			printf("\n 3 clear screen");
			printf("\n 4 Quit");
			break;
			case '1':
			inorder(args->ctx->root);
			break;
			
			case '2':
			write(args->ctx->pipefds[1], "dashboard", 20);
			break;
			case '3':
			system("clear");
			break;
			case '4':
			
			break;
			
			
		}
		
		
		
		
		
	}
	
	write(args->ctx->pipefds[1], "exit", 20);

	pthread_join(mythread, NULL);

	struct blocked_ip *p=args->ctx->blocked;
	while(p!=NULL)
	{
		struct blocked_ip *bl=p;
		p=p->next;
		free(bl);
		
	}
	free(ctx);
	free(args);

	return 0;
}
void startserver(struct arguments *args)
{
	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(struct sockaddr_in));
	serv_addr.sin_port = htons(args->port);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	int i=1;
	while(i<4){
	if (bind(args->server, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr_in)) < 0)
	{
		
		i++;
		printf("\nfailed to bind retrying...%d wating 8sec",i);
		sleep(8);
		//continue;
	}
	else
		break;
	
	}
	if(i>=4){
		printf("\nBind to port failed");
		return;
	}
		printf("Server running");
	
	if (listen(args->server, 32) < 0)
	{
		printf("listen error");
		return;
	}
	//printf("\nserver is runnu=ing");
	int epoll_fd = epoll_create(1);
	if (epoll_fd < 0)
	{
		printf("epoll create error");
		args->ctx->stopping = 1;
	}
	// Monitor stop events skipping

	// Get max number of sessions
	int maxsessions = SESSION_MAX;
	//monitor tun
	struct epoll_event ev_server, ev_pipe;

	memset(&ev_server, 0, sizeof(struct epoll_event));
	ev_server.events = EPOLLIN | EPOLLERR;
	ev_server.data.fd = args->server;

	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, args->server, &ev_server))
	{
		printf("epoll add server error %d: %s", errno, strerror(errno));

		args->ctx->stopping = 1;
	}
	memset(&ev_pipe, 0, sizeof(struct epoll_event));
ev_pipe.events = EPOLLERR | EPOLLIN;
	ev_pipe.data.fd = args->ctx->pipefds[0];
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, args->ctx->pipefds[0], &ev_pipe))
	{
		printf("epoll add pipe error %d: %s", errno, strerror(errno));

		args->ctx->stopping = 1;
	}
	//printf("\npipe added\n");
	long long last_check = 0;
	//Loop
	while (!args->ctx->stopping)
	{

		int timeout = EPOLL_TIMEOUT;

		// Count sessions
		int isessions = 0;
		
		struct session *s = args->ctx->session;

		while (s != NULL)
		{

			isessions++;
			if (s->protocol == IPPROTO_TCP) {
				if (s->remote >= 0)
                    monitor_tcp_session(args, s, epoll_fd);
			}
			s = s->next;
		}

		int sessions = isessions;
		 long ms = get_ms();
		if (ms - last_check > EPOLL_MIN_CHECK)
		{
			//  if(1){ last_check = ms;
			//last_check = ms;
			time_t now = time(NULL);
			struct session *sl = NULL;
			s = args->ctx->session;
			while (s != NULL)
			{
				int del = 0;

				/*if (s->protocol == IPPROTO_UDP) { 	del = check_udp_session(args, s, sessions, maxsessions);
				    if (s->udp.state == UDP_ACTIVE && !del) { 	int stimeout = s->udp.time +
				                       get_udp_timeout(&s->udp, sessions, maxsessions) - now + 1;
				        if (stimeout > 0 && stimeout < timeout)
				            timeout = stimeout;
				    }
				}else */
				if (s->protocol == IPPROTO_TCP)
					del = check_tcp_session(args, s, sessions, maxsessions, epoll_fd);

				if (del)
				{

					if (sl == NULL)
						args->ctx->session = s->next;
					else
						sl->next = s->next;

					struct session *c = s;
					s = s->next;
					if (c->protocol == IPPROTO_TCP){
						args->send+=c->tcp.sent;
						args->rcvd+=c->tcp.received;
					printlog(c);
					clear_tcp_data(&c->tcp);
					//printf("\nclosed session id:%d ",c->id);
					
					free(c);
					
					}
				}
				else
				{
					sl = s;
					s = s->next;
				}
			}
		}else
		{
		
		}
	//	printf("waiting");

		// Poll
		struct epoll_event ev[EPOLL_EVENTS];
		int ready = epoll_wait(epoll_fd, ev, EPOLL_EVENTS,
			timeout*100);

		if (ready < 0)
		{
			if (errno == EINTR)
			{
				printf("epoll interrupted tun %d", args->server);
				continue;
			}
			else
			{
				printf("epoll tun %d error %d: %s",
					args->server, errno, strerror(errno));
				//report_exit(args, "epoll tun %d error %d: %s",
				//          args->tun, errno, strerror(errno));
				break;
			}
		}

		if (ready == 0)
			printf("epoll timeout");
		else
		{

			if (pthread_mutex_lock(&args->ctx->lock))
				printf("pthread_mutex_lock failed");

			for (int i = 0; i < ready; i++)
			{

				if (ev[i].data.fd == args->ctx->pipefds[0])
				{
				 		// Check pipe
					//printf("pipe event\n");
					char buffer[20];
					if (read(args->ctx->pipefds[0], buffer, 20) < 0)
						printf("Read pipe error %d: %s",
							errno, strerror(errno));
					else
					{
						//printf("%s",buffer);
						if(strcmp(buffer,"dashboard")==0){
							printf("\nTotal Data Recieved : "GRN"%ldKB"RESET" Sent: "BLU"%ldKB",args->rcvd/1024,args->send/1024);
							printf(RED"\n%d domains blocked."RESET,args->blocked);
			
						}
						
						else if(strcmp(buffer,"exit")==0)
						{
						printf("\nRead exit command");

						close(server);
						//args->server = -1;
						
						args->ctx->stopping=1;
						//break;
						}
					}
				}
				else if (ev[i].data.fd == args->server)
				{
				 		// Check upstream
				/*	printf("epoll ready %d/%d in % out %d err %d hup %d",
						i, ready,
						(ev[i].events &EPOLLIN) != 0,
						(ev[i].events &EPOLLOUT) != 0,
						(ev[i].events &EPOLLERR) != 0,
						(ev[i].events &EPOLLHUP) != 0);*/
					struct sockaddr_in clientaddr;
					socklen_t addrlen = sizeof(clientaddr);
				
					int new_socket;
					//do{
					if ((new_socket = accept(args->server, (struct sockaddr *) &clientaddr, &addrlen)) < 0)
					{
						if (errno == EAGAIN || errno == EWOULDBLOCK)
							break;

						//
						else
							perror("accept error");
						//continue;
						exit(EXIT_FAILURE);
					}
					SetSocketBlockingEnabled(new_socket, 1);
					handle_Client(args, &clientaddr, new_socket, epoll_fd);
					//count++;
					//}//while(count<10);
				}
				else
				{
					int n;

					struct session *c = args->ctx->session;

					while (c != NULL && !(c->client == ev[i].data.fd || c->remote == ev[i].data.fd))	//searching for session
						c = c->next;

					if (c != NULL)
					{
					 			//	printf("ev[i]:%d",ev[i].data.fd);

						if (c->client == ev[i].data.fd)
						{
							if (ev[i].events & EPOLLERR)
							{
								c->tcp.time = time(NULL);

								int serr = 0;
								socklen_t optlen = sizeof(int);
								int err = getsockopt(c->client, SOL_SOCKET, SO_ERROR, &serr, &optlen);
								if (err < 0)
									printf("getsockopt error %d: %s",
										errno, strerror(errno));
								else if (serr)
									printf(" SO_ERROR %d: %s",
										serr, strerror(serr));

								c->tcp.client_state = CLOSED;
								c->tcp.remote_state = CLOSED;
								//continue;
							}

							switch (c->tcp.socks5)
							{
								case SOCKS5_NONE:

									handle_sock_init(c);

									break;

								case SOCKS5_HELLO:
									//printf("got client commandq");
									handle_sock_command(args,c, epoll_fd);

									break;
								case 
								SOCKS5_CONNECT:

									break;
								case SOCKS5_CONNECTED:
									

									if(c->tcp.client_state == BLOCKED)
									{
										uint8_t buffer[MAX_BUF];
										n=read(c->client,buffer,sizeof(buffer));
										if(n>0)
										{
											
											const char *blockmsg="HTTP/1.1 200 OK\r\nServer: Proxy\r\nContent-Type: html\r\nConnection: keep-alive\r\n\r\n<html>"
											 "";
											n=write(c->client,blockmsg,strlen(blockmsg));
											c->tcp.client_state=CLOSED;
											continue;
										}
										
									}
									
									
									if (c->tcp.remote_state == ALIVE)
									{
										uint8_t data[MAX_BUF];
										uint8_t *das = data;
										n = read(c->client, data, sizeof(data));

										if (n < 0)
										{
											if (errno == EAGAIN || EINTR)
												continue;
											else
												c->tcp.client_state = CLOSED;
										}
										else if (n == 0)
											c->tcp.client_state = CLOSED;
										else
										{
										 								//que 
											queue_client_data(args, &c->tcp, das, n);

											
										}
									}
									else if (c->tcp.remote_state == CLOSED)
										c->tcp.client_state = CLOSED;
									
									break;
									
							}
						}
						else if (c->remote == ev[i].data.fd)
						{
						 				//printf("its s2:%d",c->s2);
							if (ev[i].events &EPOLLERR)
							{
								c->tcp.time = time(NULL);

								int serr = 0;
								socklen_t optlen = sizeof(int);
								int err = getsockopt(c->remote, SOL_SOCKET, SO_ERROR, &serr, &optlen);
								if (err < 0)
									printf("getsockopt error %d: %s",
										errno, strerror(errno));
								else if (serr)
									printf(" SO_ERROR %d: %s",
										serr, strerror(serr));

								c->tcp.remote_state = CLOSED;
								c->tcp.client_state = CLOSED;
								//continue;
							}
							else
							{
								if (ev[i].events &EPOLLOUT)
								{

									//printf("pollout");
									if(c->tcp.socks5!=SOCKS5_CONNECTED)
										{
									char r[10];
									r[0] = 0x05;
									r[1] = 0x00;
									r[2] = 0x00;
									r[3] = 0x01;
									r[4] = 0x00;
									r[5] = 0x00;
									r[6] = 0x00;
									r[7] = 0x00;
									r[8] = (char)((myport & 0xFF00) >> 8);	// Port High
									r[9] = (char)(myport & 0x00FF);	// Port Low

									n = write(c->client, r, 10);
								//	printf("sent success:%d", n);
									c->tcp.socks5 = SOCKS5_CONNECTED;
									c->tcp.remote_state = ALIVE;
									
									}
									else
									{
										
										// Forward data

											while (c->tcp.forward != NULL)
											{
											 									//sending

												ssize_t sent = write(c->remote, c->tcp.forward->data + c->tcp.forward->sent, c->tcp.forward->len - c->tcp.forward->sent);
												if (sent < 0)
												{
													if (errno == EINTR || errno == EAGAIN)
													{
													 		//printf("retry seniding");									// Retry later
														break;
													}
													else
													{
														c->tcp.remote_state = CLOSED;
														break;
													}
												}
												else
												{
												 										//c->tcp.sent += sent;
													c->tcp.forward->sent += sent;
													c->tcp.sent+=sent;
													if (c->tcp.forward->sent == c->tcp.forward->len)
													{
														struct segment *p = c->tcp.forward;
														c->tcp.forward = c->tcp.forward->next;
														free(p->data);
														free(p);
													}
													else
													{
														//printf("partial sent");
													}
												}
											}
									}
									


								}

								if (ev[i].events &EPOLLIN)
								{
								 						//printf("epollin remote;");
									if (c->tcp.client_state != CLOSED)
									{
										uint8_t data[MAX_BUF];
										n = read(c->remote, data, sizeof(data));

										//	printf("read from remote:%d",n);

										if (n < 0)
										{
											if (errno == EAGAIN || EINTR)
												continue;
											else
												c->tcp.remote_state = CLOSED;
										}
										else if (n == 0)
											c->tcp.remote_state = CLOSED;
										else
										{
											n = write(c->client, data, n);
											
											if (n < 0)
											{
												if (errno == EAGAIN || EINTR)
													continue;
												else
													c->tcp.client_state = CLOSED;
											}
											c->tcp.received+=n;
										}
									}
									else
										c->tcp.remote_state = CLOSED;
								}
							}
						}
					}
				}
			}
		}

		if (pthread_mutex_unlock(&args->ctx->lock))
			printf("pthread_mutex_unlock failed");
	}
	clean_all(args);
}
int addblockedip(const struct arguments *args,const char *ip)
{
	
	struct blocked_ip *p = NULL;
	struct blocked_ip *s = args->ctx->blocked;
	
	while(s!= NULL)
	{
		
		p=s;
		if(strcmp(s->domain,ip)==0){
			printf("already exists!\n");
			return -1;
			
		}
		s=s->next;
		
	}
	struct blocked_ip *blo=(struct blocked_ip*)malloc(sizeof(struct blocked_ip));
	//memset(blo,0,sizeof(struct blocked_ip));
	memcpy(blo->domain,ip,sizeof(blo->domain));
	blo->next=s;
	
	if(p==NULL)
		args->ctx->blocked=blo;
	else
		p->next=blo;
		
		return 0;
	
}
void queue_client_data(const struct arguments *args, struct tcp_session *cur, const uint8_t *data, uint16_t datalen)
{
	struct segment *p = NULL;
	struct segment *s = cur->forward;

	while (s != NULL)
	{

		p = s;
		s = s->next;
	}

	
		struct segment *n = (struct segment *) malloc(sizeof(struct segment));

		n->len = datalen;
		n->sent = 0;
		n->data = (uint8_t*) malloc(datalen);
		memcpy(n->data, data, datalen);
		n->next = s;
		if (p == NULL)
			cur->forward = n;
		else
			p->next = n;
	
}
void handle_Client(struct arguments *args, struct sockaddr_in *clientaddr, int client_socket, const int epollfd)
{

	//if(errno == EAGAIN || errno == EWOULDBLOCK)
	//continue;
	char source[INET6_ADDRSTRLEN + 1];
	memset(&source, 0, sizeof(INET6_ADDRSTRLEN + 1));
	struct sockaddr_in *pV4Addr = clientaddr;
	struct in_addr ipAddr = pV4Addr->sin_addr;
	inet_ntop(pV4Addr->sin_family, &ipAddr, source, sizeof(source));

//	printf("\nsocket num:%d and  Client IP:%s port:%d", client_socket, source, ntohs(pV4Addr->sin_port));
	//clientaddr.addr
	struct session *s = malloc(sizeof(struct session));
	//SetSocketBlockingEnabled(client_socket,1);
	s->client = client_socket;
	s->remote = -1;
	
	s->protocol = IPPROTO_TCP;
	s->tcp.received = 0;
	s->tcp.sent = 0;
	s->tcp.socks5 = SOCKS5_NONE;
	s->tcp.client_state = NONE;
	s->tcp.remote_state = NONE;
	s->tcp.time = time(NULL);
	s->tcp.saddr.ip4=clientaddr->sin_addr.s_addr;
	s->tcp.source=clientaddr->sin_port;
	s->next = NULL;
	s->tcp.forward = NULL;

	// Search session add new connection at end
	struct session *cur = args->ctx->session;
	struct session *p=NULL;
	while (cur != NULL){
		p=cur;
		cur = cur->next;
		
	}
	

	
	//struct session *ns =malloc(sizeof(struct session));
	memset(&s->ev_c, 0, sizeof(struct epoll_event));
	s->ev_c.events = EPOLLERR | EPOLLIN;
	s->ev_c.data.fd = s->client;
	args->scount++;
		s->id=args->scount;
	
	
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, s->client, &s->ev_c))
		printf("epoll add tcp error %d: %s",
			errno, strerror(errno));

	s->next = args->ctx->session;
	args->ctx->session = s;

}
int check_tcp_session(const struct arguments *args, struct session *s,
	int sessions, int maxsessions, int epollfd)
{

	if (s->tcp.client_state == CLOSED)
	{
		if(s->client>0)
		close(s->client);
		s->client = -1;
	}
	if (s->tcp.remote_state == CLOSED)
	{
		if(s->remote>0)
		close(s->remote);
		s->remote = -1;
	}
	if (s->tcp.client_state == CLOSED && s->tcp.remote_state == CLOSED)
		return 1;
	else if (s->tcp.client_state == TERMINATE)
	{
		if(s->client>0)
		close(s->client);
		s->client = -1;
		return 1;
	}

	return 0;

}

void handle_sock_init(struct session *c)
{
	int n;
	uint8_t buf[3];
	n = read(c->client, buf, 3);

	if (n <= 0)
	{
		if (errno == EAGAIN || errno == EINTR)
			return;
		else
			c->tcp.client_state = TERMINATE;
	}
	else
	{
		struct sock_hello *sr = (struct sock_hello *) buf;
		if (sr->v == 0x05)
		{
			if (sr->a == 0x00)
			{

				buf[0] = 0x05;
				buf[1] = 0x00;
				n = write(c->client, buf, 2);
				c->tcp.socks5 = SOCKS5_HELLO;
				//printf("hello sent:%d\n",n);
			}
		}
	}
}
void handle_sock_command(struct arguments *args, struct session *c, const int epollfd)
{
	uint8_t domain[256];
	uint8_t buf[5];
	int n, i, sock;
	uint8_t DST_Addr[256];
	uint8_t DST_Port[2];
	int addr_Len;
	char ATYP;
	char COMMAND;
	n = read(c->client, buf, 4);
	
	if (n < 0)
	{
		if (errno == EAGAIN || errno == EINTR)
			return;
		else
			c->tcp.client_state = TERMINATE;
	}
	else
	{
		//printf("read cpommmec");
		//
		memset(DST_Addr, 0, sizeof(DST_Addr));
		memset(domain, 0, sizeof(domain));

		DST_Addr[0] = getByte(c->client);
		ATYP = buf[3];
		COMMAND=buf[1];
		char address[256];
		memset(address, 0, sizeof(address));
		switch (ATYP)
		{
			case 1:
				addr_Len = 4;
				for (i = 1; i < addr_Len; i++)
				{
					DST_Addr[i] = getByte(c->client);
					domain[i - 1] = DST_Addr[i];
				}
				int ip = *(int*) DST_Addr;
				//printf("ip:%d",ip);

				c->tcp.daddr.ip4 = ip;
				inet_ntop(AF_INET,DST_Addr, address, sizeof(address));
				//inet_ntop(AF_INET, &ip, address, sizeof(address));
			//printf("%s",address);

				c->tcp.version = 4;
				break;
			case 3:
				addr_Len = DST_Addr[0] + 1;
				for (i = 1; i < addr_Len; i++)
				{
					DST_Addr[i] = getByte(c->client);
					domain[i - 1] = DST_Addr[i];
				}

	struct addrinfo hints, *servinfo;
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // use AF_INET6 to force IPv6
	hints.ai_socktype = SOCK_STREAM;

	struct node *root=args->ctx->root;
	time_t t=time(NULL);
	int a=iterativeSearch(root,domain);
	time_t t2=time(NULL);
	//printf("\nt1:%ld-- t2:%ld",t,t2);
	if(a)
	{
	//	printf("domain blocked\n%s",domain);
			printf(RED);
		printf("%s blocked\n",domain);
		
		printf(RESET);
		args->blocked++;
			char r[10];
									r[0] = 0x05;
									r[1] = 0x00;
									r[2] = 0x00;
									r[3] = 0x01;
									r[4] = 0x00;
									r[5] = 0x00;
									r[6] = 0x00;
									r[7] = 0x00;
									r[8] = (char)((myport & 0xFF00) >> 8);	// Port High
									r[9] = (char)(myport & 0x00FF);	// Port Low

									n = write(c->client, r, 10);
			c->tcp.client_state=BLOCKED;
			c->tcp.socks5=SOCKS5_CONNECTED;
			return;
	}
	if ( (rv = getaddrinfo( domain , "http" , &hints ,&servinfo)) != 0) 
	{
		//fprintf(stderr, &quot;getaddrinfo: %s\n&quot;, gai_strerror(rv));
		return;
	}
//	h = (struct sockaddr_in *) servinfo->ai_addr;

if(servinfo == NULL)
{
	printf("host NOT found");
					c->tcp.client_state = TERMINATE;
					freeaddrinfo(servinfo);
					return;
	
}
else
{
	if (servinfo->ai_family == AF_INET)
					{
						inet_ntop(AF_INET,(struct sockaddr_in *)servinfo->ai_addr, address, sizeof(address));
						struct sockaddr_in a=*(struct sockaddr_in *)servinfo->ai_addr;
						c->tcp.daddr.ip4 = a.sin_addr.s_addr;
						c->tcp.version = 4;
						// printf("IP:%s\n",address);
					}
					else
					{
						//inet_ntop(AF_INET6, (struct sockaddr_in6 *)servinfo->ai_addr, address, sizeof(address));
						c->tcp.daddr.ip6 = ((struct sockaddr_in6*) servinfo->ai_addr)->sin6_addr;//*(struct in6_addr *) servinfo->ai_addr;
						c->tcp.version = 6;
					}
}
	
	freeaddrinfo(servinfo); // all done with this structure
				
			
			
				break;
			case 4:
				addr_Len = 16;
				for (i = 1; i < addr_Len; i++)
				{
					DST_Addr[i] = getByte(c->client);
					domain[i - 1] = DST_Addr[i];
				}

				c->tcp.version = 6;
				char ip_6[255];
				memcpy(&c->tcp.daddr.ip6,DST_Addr,16);
			//	c->tcp.daddr.ip6 = *(struct in6_addr *) DST_Addr;
				ipv6_to_str_unexpanded(ip_6, &c->tcp.daddr.ip6);
				printf("ipv6: %s", ip_6);
				break;
			default:
				c->tcp.client_state = TERMINATE;
				return;

				break;
		}
		read(c->client, DST_Port, 2);
		c->tcp.dest = *(short*) DST_Port;
		//search for blocked
		//struct blocked_ip *a=args->ctx->blocked;
    
	//if(c->tcp.version==4)
		//inet_ntop(AF_INET,(struct sockaddr_in *)c->tcp.daddr.ip4, address, sizeof(address));
	
	
		if(COMMAND == CONNECT_COMMAND)
		{
		sock = connectToServer(c);
		if (sock < 0)
		{
			c->tcp.client_state = TERMINATE;
			return;
		}

		c->remote = sock;
		// Monitor events
		memset(&c->ev_r, 0, sizeof(struct epoll_event));

		c->ev_r.events = EPOLLERR | EPOLLOUT;
		c->ev_r.data.fd = c->remote;

		if (epoll_ctl(epollfd, EPOLL_CTL_ADD, c->remote, &c->ev_r))
			printf("epoll add remote error %d: %s",
				errno, strerror(errno));

		else
		{
			c->tcp.socks5 = SOCKS5_CONNECT;
		//	printf("remote sock added\n");
		}
		
		}
		else if(COMMAND == BIND_COMMAND)
		{
			printf("its a bind command");
		}
		else if(COMMAND == UDP_COMMAND)
		{
			printf("udp command");
			//create a udp sock
			int udpsock,t=0;
			struct sockaddr_in servaddr;
			  
    // Creating socket file descriptor 
    if ( (udpsock = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
       printf("udp socket creation failed");
	   c->tcp.client_state=CLOSED;
	   return;
        //exit(EXIT_FAILURE); 
		} 
		int port;
		 memset(&servaddr, 0, sizeof(servaddr)); 
		 while(t<10)
			 {
				port=c->tcp.source+t*55;
		   // Filling server information 
			servaddr.sin_family    = AF_INET; // IPv4 
			servaddr.sin_addr.s_addr = INADDR_ANY; 
			servaddr.sin_port = htons(port); 
			// Bind the socket with the server address 
    if ( bind(udpsock, (const struct sockaddr *)&servaddr,  
            sizeof(servaddr)) < 0 ) 
    { 
        printf("\nbind failed on port:%d",port); 
       // exit(EXIT_FAILURE); 
    } 
	else{
		printf("udp bounded to %d",port);
		break;
	}
	 t++;
	}
     
		}
		}
	
	
}
int connectToServer(struct session *c)
{
	int sock;
	int version = c->tcp.version;
	//printf("version:%d", c->tcp.version);
	// Get TCP socket
	if ((sock = socket(version == 4 ? AF_INET : AF_INET6, SOCK_STREAM, 0)) < 0)
	{
		printf("socket error %d: %s", errno, strerror(errno));
		return -1;
	}

	// Protect
	//   if (protect_socket(args, sock) < 0)
	//  return -1;

	int on = 1;
	if (setsockopt(sock, SOL_TCP, TCP_NODELAY, &on, sizeof(on)) < 0)
		printf("setsockopt TCP_NODELAY error %d: %s",
			errno, strerror(errno));

	// Set non blocking
	int flags = fcntl(sock, F_GETFL, 0);
	if (flags < 0 || fcntl(sock, F_SETFL, flags | O_NONBLOCK) < 0)
	{
		printf("fcntl socket O_NONBLOCK error %d: %s",
			errno, strerror(errno));
		return -1;
	}
	
	// Build target address
	struct sockaddr_in addr4;
	struct sockaddr_in6 addr6;
	if (version == 4)
	{
		addr4.sin_family = AF_INET;
		addr4.sin_addr.s_addr = c->tcp.daddr.ip4;
		addr4.sin_port = c->tcp.dest;
	}
	else
	{
		addr6.sin6_family = AF_INET6;
		memcpy(&addr6.sin6_addr, &c->tcp.daddr.ip6, 16);
		addr6.sin6_port = c->tcp.dest;
		printf("vesrion6 trying to connect");
	}
	// Initiate connect
	int err = connect(sock,
		(version == 4 ? (const struct sockaddr *) &addr4 :
			(const struct sockaddr *) &addr6),
		(socklen_t)(version == 4 ?
			sizeof(struct sockaddr_in) :
			sizeof(struct sockaddr_in6)));
	
	if (err < 0 && errno != EINPROGRESS)
	{
		printf("connect error %d: %s", errno, strerror(errno));
		return -1;
	}
	//else
		//printf("\nnot yet error no:%d", errno);

	return sock;

}
int SetSocketBlockingEnabled(int fd, int blocking)
{
	if (fd < 0) return 0;

	#ifdef _WIN32
	unsigned long mode = blocking ? 0 : 1;
	return (ioctlsocket(fd, FIONBIO, &mode) == 0) ? 1 : 0;
	#else
		int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1) return 0;
	flags = blocking ? (flags &~O_NONBLOCK) : (flags | O_NONBLOCK);
	return (fcntl(fd, F_SETFL, flags) == 0) ? 1 : 0;
	#endif
}