#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>

char msg1[] = "Hello there!\n";
char msg2[] = "Bye bye!\n";

int main()
{
    int sock,len1,bytes_read;
    struct sockaddr_in addr;
    char *ip_serv = "192.168.23.4";

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock < 0)
    {
        perror("socket");
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(7778);
    if (inet_aton(ip_serv, (struct in_addr *)&addr.sin_addr) == 0) 
    {
        printf("Address of server is invalid!\n");
        return 0;
    }
    sendto(sock, msg1, sizeof(msg1), 0,(struct sockaddr *)&addr, sizeof(addr));
    len1=sizeof(addr);
    bytes_read = recvfrom(sock, msg1, sizeof(msg1), 0, (struct sockaddr *)&addr, &len1);
    puts(msg1);

    //printf("messege_reply udp: %s",buf);
    close(sock);

    return 0;
}