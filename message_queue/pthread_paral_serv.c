#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>  
#include <stdio.h> 
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/poll.h>
#include <strings.h>
#include <signal.h>
#include <pthread.h>
#include <arpa/inet.h>

char buffer[1024];
struct msgbuf { 
    long type; 
    int fd; 
    int employment[2][3]; 
    struct sockaddr_in addr;
    char *proto;
} mymsg;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
void * tcp_thread(void *a)
{
	char buf[1024];
	key_t ipckey;
	int bytes_read,mq_id, *index_pthread,struct_len;
	index_pthread = (int *)a;
	ipckey = ftok("/tmp/q",'m');
    mq_id = msgget(ipckey, IPC_CREAT | 0666);
    while(1)
    {
	  	msgrcv(mq_id, &mymsg, sizeof(mymsg), 1, 0);
	  	if (strcmp(mymsg.proto,"tcp") == 0)
	  	{
		   	while(1)
		    {
			  	bytes_read = recv(mymsg.fd, buf, 1024, 0);
			  	if(bytes_read <= 0) 
			     	break;
			  	send(mymsg.fd, buf, bytes_read, 0); 
			}
			close(mymsg.fd);
		}
		else
		{
			if (strcmp(mymsg.proto,"udp") == 0)
			{
				struct_len = sizeof(mymsg.addr);	
					write(1,"\nsdf\n",6);	
					sendto(mymsg.fd, buffer, sizeof(buffer), 0,(struct sockaddr *)&mymsg.addr, sizeof(mymsg.addr));
			}
		}
	  	printf("\nPthread %d service to client\n", *index_pthread);
	}
  pthread_exit(NULL);
}

int main()
{
	char client_message[2000];
	pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
	int client_sockfd, tcpsock, status, struct_len,flag = 0,ret,a[3],udpsock,rtn,on = 1;
    struct sockaddr_in servaddr,client_address;
    char buf[1024], *ip_serv = "10.0.2.15",*ip_client;
    int bytes_read,i,mq_id;
    pid_t pid;
    key_t ipckey;
    const int threads=3;
    pthread_t tid;
    struct pollfd fds[2];
 	tcpsock = socket(AF_INET, SOCK_STREAM, 0);
    if(tcpsock < 0)
    {
        perror("socket");
        exit(1);
    }
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(3426);
    if (inet_aton(ip_serv, (struct in_addr *)&servaddr.sin_addr) == 0) 
    {
      printf("Address of server is invalid!\n");
      return 0;
    }
    //servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    rtn = setsockopt(tcpsock, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
	if (rtn == -1) {
		perror("[TCP] Server: setsockopt(SO_REUSEADDR)");
		close(tcpsock);
		exit(EXIT_FAILURE);
	}
    if(bind(tcpsock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("bind");
        exit(2);
    }
    listen(tcpsock, 1);  
    udpsock = socket(AF_INET, SOCK_DGRAM, 0);
	rtn = setsockopt(udpsock, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
	if (rtn == -1) {
		perror("[TCP] Server: setsockopt(SO_REUSEADDR)");
		close(tcpsock);
		exit(EXIT_FAILURE);
	}
	bind(udpsock, (struct sockaddr *)&servaddr, sizeof(servaddr));
    fds[0].fd = tcpsock;//дискриптор сокета на котором должно рпоизойти событие
    fds[0].events = POLLIN;//указывает на битовую маску событийсобытия,чтение, POLLOUT запись
    fds[1].fd = udpsock;
    fds[1].events = POLLIN;
    for ( i=0; i<threads; i++)
    {
    	a[i] = i;
       	pthread_create(&tid,NULL,&tcp_thread,&a[i]);
    }
    ipckey = ftok("/tmp/q",'m');
    printf("My key is %d\n", ipckey);
    // Set up the message queue 
    mq_id = msgget(ipckey, IPC_CREAT | 0666);
    mymsg.type = 1;

    printf ("start serv\n");
    struct_len = sizeof(client_address);
    while(1)
    {
    	//write(1,"\n1\n",4);
    	ret = poll(fds, 2, -1);
    	write(1,"\n1\n",4);
		if ( ret < 1) //второй параметр - кол-во задаваемых структур, третий таймаут в милисек, отриц - бесконечный таймер
		{
 			close(udpsock);
			close(tcpsock);
			exit(EXIT_FAILURE);
	   	}
	   	else
	   		if ( fds[0].revents == POLLIN ) 
			{
			//	write(1,"\n1\n",4);
				fds[0].revents = 0;
	
		    	client_sockfd = accept(tcpsock, (struct sockaddr *)&client_address, (socklen_t *)&struct_len);
		        if(client_sockfd < 0)
		        {
		            perror("accept");
		            exit(3);
		        }
		        mymsg.fd = client_sockfd;
		        mymsg.proto = "tcp";
		        msgsnd(mq_id, &mymsg, sizeof(mymsg), 0);
		    }
		    else
		    	if ( fds[1].revents == POLLIN )
		    	{
		    		fds[1].revents = 0;
		    		//bzero(&client_address, sizeof(client_address));
//write(1,"\n1\n",4);
		    		//while(1)
		    		//{
			    		bytes_read = recvfrom(udpsock, buffer, 1024, 0, (struct sockaddr *)&client_address, &struct_len);
			    		//if(bytes_read <= 0) 
			     		//	break;
			    		//getpeername(udpsock,(struct sockaddr *)&client_address, (socklen_t *)&struct_len);
			    		//ip_client = inet_ntoa(client_address.sin_addr);
						//printf("\nclient (%s:%d)\n",  ip_client, ntohs(client_address.sin_port));
			    		//printf("\nwerr\n");
			    		mymsg.addr = client_address;
			    		mymsg.fd = udpsock;
			    		ip_client = inet_ntoa(mymsg.addr.sin_addr);
			    		mymsg.proto = "udp";
			    		printf("\nclient (%s:%d), mymsg.proto = %s\n",  ip_client, ntohs(mymsg.addr.sin_port),mymsg.proto);
			    		msgsnd(mq_id, &mymsg, sizeof(mymsg), 0);
			    	//}
		    		//return 0;
		    	}
        //pthread_join(tid,NULL);
    
    }
    close(udpsock);
	close(tcpsock);
}

/*#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h> 
#include <stdio.h> 
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/poll.h>
#include <strings.h>
#include <signal.h>
#include <pthread.h>
    struct msgbuf { 
      long type; 
      int fd; 
      int employment[2][3]; 
    } mymsg;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
void * tcp_thread(void *a)
{
	char buf[1024];
	key_t ipckey;
	int bytes_read,mq_id, *index_pthread;
	index_pthread = (int *)a;
	ipckey = ftok("/tmp/q",'m');
    mq_id = msgget(ipckey, IPC_CREAT | 0666);
    while(1)
    {
    	//printf("\nPthread %d captured to mutex\n", *index_pthread);
	   // pthread_mutex_lock(&lock);
    	//вызывающий процесс блокируется, пока в очередь не прибудет сообщение, соответствующее параметрам msgrcv()
	  	msgrcv(mq_id, &mymsg, sizeof(mymsg), 1, 0);
	  	//printf("\n%d\n",mymsg.fd);
	  	printf("\nPthread %d captured to mutex\n", *index_pthread);
	   	while(1)
	    {
		  	bytes_read = recv(mymsg.fd, buf, 1024, 0);
		  	if(bytes_read <= 0) 
		     	break;
		  	send(mymsg.fd, buf, bytes_read, 0); 
		}
	  	printf("\nPthread %d service to client\n", *index_pthread);
	  	close(mymsg.fd);
	  	//pthread_mutex_unlock(&lock);
	  	//sleep(2);
	}
  //pthread_exit(NULL);
}

int main()
{
	char client_message[2000];
	char buffer[1024];
	pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
	int client_sockfd, listener, status, struct_len,flag = 0,ret, val_sem,a[3];
    struct sockaddr_in addr,client_address;
    char buf[1024], *ip_serv = "10.0.2.15",*p = "start serv";
    int bytes_read,i,mq_id;
    pid_t pid;
    key_t ipckey;
    sem_t *sem;
    int sock;
    const int threads=3;
    pthread_t tid;
 	listener = socket(AF_INET, SOCK_STREAM, 0);
    if(listener < 0)
    {
        perror("socket");
        exit(1);
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(3425);
    if (inet_aton(ip_serv, (struct in_addr *)&addr.sin_addr) == 0) 
    {
      printf("Address of server is invalid!\n");
      return 0;
    }
    if(bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        exit(2);
    }
    listen(listener, 1);
    struct_len = sizeof(client_address);
    
    for ( i=0; i<threads; i++)
    {
    	a[i] = i;
       	pthread_create(&tid,NULL,&tcp_thread,&a[i]);
    }
    ipckey = ftok("/tmp/q",'m');
    printf("My key is %d\n", ipckey);
    // Set up the message queue 
    mq_id = msgget(ipckey, IPC_CREAT | 0666);
    mymsg.type = 1;
    printf ("start serv\n");
    while(1)
    {
    	client_sockfd = accept(listener, (struct sockaddr *)&client_address, (socklen_t *)&struct_len);
        if(client_sockfd < 0)
        {
        	write(1, "star1:\n", 9);
            perror("accept");
            exit(3);
        }
        mymsg.fd = client_sockfd;
        msgsnd(mq_id, &mymsg, sizeof(mymsg), 0);
        //pthread_join(tid,NULL);
    }
}*/