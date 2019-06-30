/*
Если сервер непараллельный, то когда сервер обслуживает клиента, то другие клиенты к нему присоединиться не могут, то есть о не может с ними работать 
*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h> 
#include <stdio.h> 

int main()
{
    int client_sockfd, listener, status, struct_len;
    struct sockaddr_in addr,client_address;
    char buf[1024], *ip_serv = "10.0.2.15";
    int bytes_read;
    pid_t pid;

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
    write(1, "serv:\n", 7);
    while(1)
    {
    	write(1, "start:\n", 8);
        client_sockfd = accept(listener, (struct sockaddr *)&client_address, (socklen_t *)&struct_len);
        if(client_sockfd < 0)
        {
        	write(1, "star1:\n", 9);
            perror("accept");
            exit(3);
        }

        switch(pid = fork())//создаем дочерний процесс, который будет обслуживать поступившего клиента, а родитель будет ждать говой заявки
        {
	        case -1:
	            perror("fork");
	            break;
	            
	        case 0:// дочерний процесс, обслуживает клиента,
	            close(listener);//поэтому закрываем сокет по работе с сервером, так как дочерний процесс не будет ожидать поступления заявки следующего клиента, это необходимо так как у кадого 
	            //сокета есть счетчик дискрипторов, открытых в настоящий момент, так как исп функция fork() то вместо одного дискриптора стало два, поэтому чтобы закрыть сокет нужно закрыть все дискрипторы
	            while(1)
	            {

	                bytes_read = recv(client_sockfd, buf, 1024, 0);
	                if(bytes_read <= 0) 
	                	break;
	                send(client_sockfd, buf, bytes_read, 0);
	            }
	            write(1, "close_client:\n", 14);
	            close(client_sockfd);//обслужив клиента закрываем дискриптор по работе с клиентом
	            exit(0);
	            
	        default: //если родитель
	        	write(1, "parent:\n", 9);
	            close(client_sockfd); //то закрываем дисриптор по работе с обслуживанием клиента
        }
    }
    
    close(listener);

    return 0;
}
    /*#include <sys/types.h>
    #include <sys/socket.h>
    #include <stdio.h>
    #include <netinet/in.h>
    #include <signal.h>
    #include <unistd.h>
    #include <stdlib.h>
    int main() 
    {
        int server_sockfd, client_sockfd,bytes_read;
        int server_len, client_len;
        struct sockaddr_in server_address;
        struct sockaddr_in client_address;
        char buf[1024];
        server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
        server_address.sin_family = AF_INET;
        server_address.sin_addr.s_addr = htonl(INADDR_ANY);
        server_address.sin_port = htons(3425);
        server_len = sizeof(server_address);
        bind(server_sockfd, (struct sockaddr *)&server_address, server_len);
        //2. Создайте очередь соединений, игнорируйте подробности завершения дочернего процесса и ждите запросов клиентов:
        listen(server_sockfd, 5);
        signal(SIGCHLD, SIG_IGN);
        while(1) 
        {
            char ch;
            printf("server waiting\n");
            //3. Примите запрос на соединение:
            client_len = sizeof(client_address);
            client_sockfd = accept(server_sockfd,(struct sockaddr *)&client_address, &client_len);
            //4. Вызовите fork с целью создания процесса для данного клиента и выполните проверку, чтобы определить, родитель вы 
            //или потомок:
            if (fork() == 0) 
            {
                close(server_sockfd);
                //5. Если вы потомок, то можете читать/писать в программе-клиенте на сокете client_sockfd.Пятисекундная задержка 
                //нужна для того, чтобы это продемонстрировать:
                //read(client_sockfd, &ch, 1);
                //sleep(2);
                //ch++;
                //write(client_sockfd, &ch, 1);

                bytes_read = recv(client_sockfd, buf, 1024, 0);
                   // if(bytes_read <= 0) 
                  //      break;
                buf[bytes_read]='\0';
                printf(buf);
                //sleep(1);
                send(client_sockfd, buf, bytes_read, 0);
                close(client_sockfd);
                exit(0);
            }
            //6. В противном случае вы должны быть родителем и ваша работа с данным клиентом закончена:
            else 
            {
                //sleep(1);
                //printf("sdf");
                close(client_sockfd);
            }
        }
    }

*/
/*#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
int main()
{
    int sock, listener;
    struct sockaddr_in addr,addr1;
    char buf[1024],msg[]="hi!!!\n";
    int bytes_read,len;
    listener = socket(AF_INET, SOCK_STREAM, 0);
    if(listener < 0)
    {
        perror("socket");
        exit(1);
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(3425);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        exit(2);
    }
    listen(listener, 1);
    len=sizeof(addr1);
    while(1)
    {
        sock = accept(listener, (struct sockaddr *)&addr1, &len);
        if(sock < 0)
        {
            perror("accept");
            exit(3);
        }
        while(1)
        {
            bytes_read = recv(sock, buf, 1024, 0);
            if(bytes_read <= 0) 
                break;
            buf[bytes_read] = '\0';
            printf(buf);
            send(sock, msg, sizeof(msg), 0);
        }
        close(sock);
    } 
    return 0;
}*/
/*#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
int main()
{
    int sock, listener,len;
    struct sockaddr_in addr,addr1;
    char buf[1024];
    int bytes_read;
    pid_t child_pid;
    int stat_val;
    listener = socket(AF_INET, SOCK_STREAM, 0);
    if(listener < 0)
    {
        perror("socket");
        exit(1);
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(3425);
    addr.sin_addr.s_addr = INADDR_ANY;
    if(bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        exit(2);
    }
    listen(listener, 1);
    while(1)
    {
        sock = accept(listener, (struct sockaddr *)&addr1, &len);
        if(sock < 0)
        {
            perror("accept");
            exit(3);
        }
        
        //printf("\n%d\n",child_pid);
        switch(fork())
        {
            case -1:
                perror("fork");
                break;  
            case 0:
                close(listener);
                printf("doch");
                while(1)
                {
                    bytes_read = recv(sock, buf, 1024, 0);
                    if(bytes_read <= 0) 
                        break;
                    buf[bytes_read]='\0';
                    printf(buf);
                    printf("\n");
                    send(sock, buf, bytes_read, 0);
                }
                //close(sock);
                _exit(0);
                break;
            default:
                printf("rod");
               // close(sock);
                //_exit(0);
                break;
        }
    }
    close(listener);
    return 0;
}*//*
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <stdio.h>
    #include <netinet/in.h>
    #include <signal.h>
    #include <unistd.h>
    #include <stdlib.h>
    int main() 
    {
        int server_sockfd, client_sockfd,bytes_read;
        int server_len, client_len;
        struct sockaddr_in server_address;
        struct sockaddr_in client_address;
        char buf[1024];
        server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
        server_address.sin_family = AF_INET;
        server_address.sin_addr.s_addr = htonl(INADDR_ANY);
        server_address.sin_port = htons(3425);
        server_len = sizeof(server_address);
        bind(server_sockfd, (struct sockaddr *)&server_address, server_len);
        //2. Создайте очередь соединений, игнорируйте подробности завершения дочернего процесса и ждите запросов клиентов:
        listen(server_sockfd, 5);
        signal(SIGCHLD, SIG_IGN);
        while(1) 
        {
            char ch;
            printf("server waiting\n");
            //3. Примите запрос на соединение:
            client_len = sizeof(client_address);
            client_sockfd = accept(server_sockfd,(struct sockaddr *)&client_address, &client_len);
            //4. Вызовите fork с целью создания процесса для данного клиента и выполните проверку, чтобы определить, родитель вы 
            //или потомок:
            if (fork() == 0) 
            {
                close(server_sockfd);
                //5. Если вы потомок, то можете читать/писать в программе-клиенте на сокете client_sockfd.Пятисекундная задержка 
                //нужна для того, чтобы это продемонстрировать:
                //read(client_sockfd, &ch, 1);
                //sleep(2);
                //ch++;
                //write(client_sockfd, &ch, 1);

                bytes_read = recv(client_sockfd, buf, 1024, 0);
                   // if(bytes_read <= 0) 
                  //      break;
                buf[bytes_read]='\0';
                printf(buf);
                //sleep(1);
                send(client_sockfd, buf, bytes_read, 0);
                close(client_sockfd);
                exit(0);
            }
            //6. В противном случае вы должны быть родителем и ваша работа с данным клиентом закончена:
            else 
            {
                //sleep(1);
                //printf("sdf");
                close(client_sockfd);
            }
        }*/
    