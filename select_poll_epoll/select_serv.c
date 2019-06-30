#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <signal.h>
//#include <errno.h>
#define SA struct sockaddr
#define MAXLINE 1024
int  main(int argc, char **argv)
{
	int tcpsock, connfd, udpsock, nready, maxfdp1,bytes_read;
	char mesg[MAXLINE];
	pid_t childpid;
	fd_set rset;
	ssize_t n;
	socklen_t len;
	const int on = 1;
	struct sockaddr_in cliaddr, servaddr;
	// создание прослушиваемого сокета TCP 
	tcpsock = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(3425);
	  	//SO_REUSEADDR - в случае, если на этом порте существуют соединения
	  	//Setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));/////////////////////
	bind(tcpsock, (SA*)&servaddr, sizeof(servaddr));
	listen(tcpsock, 5);
	  	// создание сокета UDP 
	udpsock = socket(AF_INET, SOCK_DGRAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(3425);
	bind(udpsock, (SA*)&servaddr, sizeof(servaddr));
	  //Итак, если вы хотите, чтобы родительский процесс периодически проверял, завершился ли конкретный дочерний 
	  //процесс, можно использовать следующий вызов: waitpid()Он вернет ноль, если дочерний процесс не завершился 
	  //и не остановлен, или child_pid, если это произошло.
	//signal(SIGCHLD, sig_chld); // требуется вызвать waitpid() //////////////////////////////////
	FD_ZERO(&rset);
	  //maxfdp1 = max(listenfd, udpfd) + 1;
	if(tcpsock > udpsock)
	  	maxfdp1=tcpsock+1;
	else
	  	maxfdp1=udpsock+1;
	printf("server waiting\n");
	while(1) 
	{
		FD_SET(tcpsock, &rset);
		FD_SET(udpsock, &rset);
		if ((nready = select(maxfdp1, &rset, NULL, NULL, NULL)) < 1) 
		{
	   	    if (nready < 1) 
		    {
			    perror("server5");
			    exit(1);
		 	}
	   	}
		if (FD_ISSET(tcpsock, &rset)) 
	   	{
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
			if (FD_ISSET(udpsock, &rset)) 
		    {
	    		printf("\nudp\n");
			    len = sizeof(cliaddr);
       			while(1)
	            {
	                bytes_read = recvfrom(udpsock, mesg, MAXLINE, 0, (SA*)&cliaddr, &len);
	                if(bytes_read <= 0) 
	                	break;
	                sendto(udpsock, mesg, n, 0, (SA*)&cliaddr, len);
	            }
	   	 	}
	}
}