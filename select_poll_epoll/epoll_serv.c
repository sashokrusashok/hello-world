#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <strings.h>
#define SA struct sockaddr
#define MAXLINE 1024
int  main(int argc, char **argv)
{
	int tcpsock, connfd, udpsock, maxfdp1,bytes_read,ret, epollfd,on = 1;
	char mesg[MAXLINE];
	pid_t childpid;
	ssize_t n;
	socklen_t len;
	struct sockaddr_in cliaddr, servaddr;

	struct epoll_event ev, events[2];

	// создание прослушиваемого сокета TCP 
	tcpsock = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(3425);
	// setsockopt(tcpsock, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
	bind(tcpsock, (SA*)&servaddr, sizeof(servaddr));
	listen(tcpsock, 1);
	  	// создание сокета UDP 
	udpsock = socket(AF_INET, SOCK_DGRAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(3425);
	//setsockopt(udpsock, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
	bind(udpsock, (SA*)&servaddr, sizeof(servaddr));
	epollfd  = epoll_create(1);
	if ( epollfd  < 0 )
	{
		close(udpsock);
		close(tcpsock);
		exit(EXIT_FAILURE);
	}
	ev.events = EPOLLIN;
	ev.data.fd = tcpsock;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, tcpsock, &ev) == -1) 
	{
        perror("epoll_ctl: listen_sock");
        exit(EXIT_FAILURE);
    }
    ev.events = EPOLLIN;
	ev.data.fd = udpsock;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, udpsock, &ev) == -1) 
	{
        perror("epoll_ctl: listen_sock");
        exit(EXIT_FAILURE);
    }
	printf("server waiting\n");
	while(1) 
	{
		printf("\n111\n");
		ret = epoll_wait(epollfd, events, 1, -1);
		printf("\nret = %d\n",ret);
		if ( ret < 1) //второй параметр - кол-во задаваемых структур, третий таймаут в милисек, отриц - бесконечный таймер
		{
 			close(udpsock);
			close(tcpsock);
			exit(EXIT_FAILURE);
	   	}
			if ( events[0].data.fd == tcpsock ) 
			{printf("\n333\n");
			   	len = sizeof(cliaddr);
				connfd = accept(tcpsock, (SA*)&cliaddr, &len);
			    if(connfd < 0)
			    {
			        perror("accept");
				    exit(3);
				}
				if(fork()==0)
				{
				    close(udpsock);
				   	close(tcpsock);
				   	printf("\ntcp\n");
		       		while(1)
			        {
			            bytes_read = recv(connfd, mesg, 1024, 0);
			            if(bytes_read <= 0) 
			                break;
			            send(connfd, mesg, sizeof(mesg), 0);
			        }
				    close(connfd);
				    exit(0);
				}
				else
				{
				    close(connfd);
				}
			}
			else
			    if ( events[0].data.fd  == udpsock ) 
				{
			        write(1,"\nUDP\n",6);
					len = sizeof(cliaddr);
		       	   // while(1)
			      //  {
			            bytes_read = recvfrom(udpsock, mesg, MAXLINE, 0, (SA*)&cliaddr, &len);
			            if(bytes_read <= 0) 
			                break;
			            sendto(udpsock, mesg, n, 0, (SA*)&cliaddr, len);
			       // }
			   	}
	}
}