#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h> 
#include <stdio.h> 

char message[] = "Hello there!\n";
char buf[sizeof(message)];

int main()
{
    int sock,bytes_read;
    struct sockaddr_in addr;
    char *ip_serv = "10.0.2.15";

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
        perror("socket");
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(3426); // или любой другой порт...
    if (inet_aton(ip_serv, (struct in_addr *)&addr.sin_addr) == 0) 
    {
        printf("Address of server is invalid!\n");
        return 0;
    }
    if(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("connect");
        exit(2);
    }
    write(1,"\n1\n",4);
    send(sock, message, sizeof(message), 0);
    write(1,"\n2\n",4);
    bytes_read = recv(sock, buf, sizeof(message), 0);
         if(bytes_read <= 0) 
                  {
                    printf("\nSTOP child:%d\n",getpid());
                   // return 0;
                  }
    write(1,buf,bytes_read);
    printf("messege_reply: %s",buf);
    close(sock);

    return 0;
}