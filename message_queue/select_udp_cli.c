#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
char msg1[] = "Hello there!\n";
char  buf[1024],msg2[] = "Bye bye!\n";
int main()
{
    int sock,len1,bytes_read;
    struct sockaddr_in addr,addr1;
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock < 0)
    {
        perror("socket");
        exit(1);
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(3426);
    addr.sin_addr.s_addr = inet_addr("10.0.2.15");
    sendto(sock, msg1, sizeof(msg1), 0,(struct sockaddr *)&addr, sizeof(addr));
    len1=sizeof(addr1);
    bytes_read = recvfrom(sock, buf, 1024, 0, (struct sockaddr *)&addr1, &len1);
    //buf[bytes_read] = '\0';
    printf("messege_reply udp: %s",buf);
    close(sock);
    return 0;
}