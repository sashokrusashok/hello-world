#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h> 
#include <stdio.h> 

char message[] = "Hello there!\n";
char buf[sizeof(message)];

int main()
{
    int sock;
    struct sockaddr_in addr;
    char *ip_serv = "10.0.2.15";

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
        perror("socket");
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(3425); // или любой другой порт...
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

    send(sock, message, sizeof(message), 0);
    recv(sock, buf, sizeof(message), 0);
    printf("messege_reply: %s",buf);
    close(sock);

    return 0;
}