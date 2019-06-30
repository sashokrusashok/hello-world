#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/poll.h>
#include <strings.h>
#include <signal.h>
#define SA struct sockaddr
#define MAXLINE 1024
int  main(int argc, char **argv)
{
	int tcpsock, connfd, udpsock, maxfdp1,bytes_read,ret;
	char mesg[MAXLINE];
	pid_t childpid;
	ssize_t n;
	socklen_t len;
	struct sockaddr_in cliaddr, servaddr;
	struct pollfd fds[2];
	// создание прослушиваемого сокета TCP 
	tcpsock = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(3425);
	bind(tcpsock, (SA*)&servaddr, sizeof(servaddr));
	listen(tcpsock, 5);
	  	// создание сокета UDP 
	udpsock = socket(AF_INET, SOCK_DGRAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(3425);
	bind(udpsock, (SA*)&servaddr, sizeof(servaddr));
	fds[0].fd = tcpsock;//дискриптор сокета на котором должно рпоизойти событие
    fds[0].events = POLLIN;//указывает на битовую маску событийсобытия,чтение, POLLOUT запись
    fds[1].fd = udpsock;
    fds[1].events = POLLIN;
	printf("server waiting\n");
	while(1) 
	{
		printf("sdfsd");
		ret = poll(fds, 2, -1);
		if ( ret < 1) //второй параметр - кол-во задаваемых структур, третий таймаут в милисек, отриц - бесконечный таймер
		{
 			close(udpsock);
			close(tcpsock);
			exit(EXIT_FAILURE);
	   	}
	   	else
	   		if ( !ret ) //если функция возвращ ноль, то произашел тайм-аут, но не в этом примере
	   		{
        		printf("\nTIMEOUT\n");
        		return 0;
    		}
    		else
				if ( fds[0].revents == POLLIN ) 
			   	{
			   		fds[0].revents = 0;
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
					if ( fds[1].revents == POLLIN ) 
				    {
				    	fds[1].revents = 0;
			    		printf("\nudp\n");
					    len = sizeof(cliaddr);
		       			//while(1)
			            //{
			            	printf("\n--\n");
			                bytes_read = recvfrom(udpsock, mesg, MAXLINE, 0, (SA*)&cliaddr, &len);
			                if(bytes_read <= 0) 
			                	break;
			                sendto(udpsock, mesg, n, 0, (SA*)&cliaddr, len);
			           // }
			   	 	}
	}
}