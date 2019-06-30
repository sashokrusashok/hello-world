#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h> 
#include <stdio.h> 
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/wait.h>
int main()
{
    int client_sockfd, listener, status, struct_len;
    struct sockaddr_in addr,client_address;
    char buf[1024], *ip_serv = "10.0.2.15",*p = "start serv";
    int bytes_read,i,mq_id;
    pid_t pid[2];
    key_t ipckey;
    struct msgbuf { long type; int a; } mymsg;
        ipckey = ftok("/tmp/q",'m');
        printf("My key is %d\n", ipckey);
 
        // Set up the message queue 
        mq_id = msgget(ipckey, IPC_CREAT | 0666);
        mymsg.type = 1;
       // close(listener);
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
    //write(1, "serv:\n", 7);
    //printf ("%s\n",p);
    printf ("start serv\n");
        mymsg.type = 1;
        mymsg.a = 0;
       // msgctl(mq_id, IPC_RMID, 0);
        msgsnd(mq_id, &mymsg, sizeof(mymsg), 0);
        //msgctl(mq_id, IPC_RMID, 0);
    for(i=0; i < 3; i++)
    {
        
        //write(1, "start:\n", 8);
        if( fork() == 0)
        {
  
                
                //сокета есть счетчик дискрипторов, открытых в настоящий момент, так как исп функция fork() то вместо одного дискриптора стало два, поэтому чтобы закрыть сокет нужно закрыть все дискрипторы
                while(1)
                {
                        client_sockfd = accept(listener, (struct sockaddr *)&client_address, (socklen_t *)&struct_len);
                        if(client_sockfd < 0)
                        {
                            write(1, "star1:\n", 9);
                            perror("accept");
                            exit(3);
                        }  

                        bytes_read = recv(client_sockfd, buf, 1024, 0);
                        if(bytes_read <= 0) 
                                break;
                        send(client_sockfd, buf, bytes_read, 0);
                        
                        write(1, "close_client:", 13);
                        printf ("parent:%d child:%d\n",getppid(),getpid());
                        close(client_sockfd);//обслужив клиента закрываем дискриптор по работе с клиентом
                        msgrcv(mq_id, &mymsg, sizeof(mymsg), 1, 0);
                          // mymsg.type = 1;
                            //    mymsg.a = 1; 
                        //msgsnd(mq_id, &mymsg, sizeof(mymsg), 0); 
                        printf("\n mymsg.a = %d\n",mymsg.a);
                        //sleep(0.25);
                        if(mymsg.a == 0)
                        {
                          //      printf("fsdgdfg");
                                mymsg.type = 1;
                                mymsg.a = 1; 
                             msgsnd(mq_id, &mymsg, sizeof(mymsg), 0);  
                            // while(1)
                             //{
                                sleep(5);
                              // printf("\nq\n");
                            // } 
                        }
                        else
                        {
                                mymsg.type = 1;
                                mymsg.a = 1; 
                             msgsnd(mq_id, &mymsg, sizeof(mymsg), 0);  

                        }
                        //printf("parent: %d ,received = %d, (%d) (%d)\n",getppid(),mymsg.a);
                       // sleep(0.5);
                       
                }
        }
    }
  //  for( i = 0; i < 2; i++ ) 
  //  {
        wait( NULL ); 
    //    write(1, "\n111\n", 6);
     //   }
        msgctl(mq_id, IPC_RMID, 0);
    return 0;
}
/*
#include <stdio.h>
#include <stdlib.h>
#include <linux/ipc.h>
#include <linux/msg.h>


typedef struct mymsgbuf
{
  long mtype;
  int int_num;
} mess_t;
int main()
{
  int qid;
  key_t msgkey;
  mess_t sent;
  mess_t received;
  int length;
 
  srand (time (0));

  length = sizeof(mess_t) - sizeof(long);
  msgkey = ftok(".",'m');

  
  qid = msgget(msgkey, IPC_CREAT | 0660);

  printf("QID = %d\n", qid);

 
  sent.mtype = 1;
  sent.int_num = 0;


  msgsnd(qid, &sent, length, 0);
  printf("MESSAGE SENT...\n");


  msgrcv(qid, &received, length, sent.mtype, 0);
  printf("MESSAGE RECEIVED...\n");


  printf("Integer number = %d (sent %d) -- ", received.int_num,sent.int_num);
  if(received.int_num == sent.int_num) 
        printf(" OK\n");
  else 
        printf("ERROR\n");


  msgctl(qid, IPC_RMID, 0);
}
/*
#include <stdio.h>
#include <stdlib.h>
#include <linux/ipc.h>
#include <linux/msg.h>
#include <sys/types.h>

typedef struct mymsgbuf
{
  long mtype;
  int num;
} mess_t;

int main()
{
  int qid;
  key_t msgkey;
  pid_t pid;

  mess_t buf;

  int length;
  int cont;

  length = sizeof(mess_t) - sizeof(long);

  msgkey = ftok(".",'m');

  qid = msgget(msgkey, IPC_CREAT | 0660);

  if(!(pid = fork()))
  {
    printf("SON - QID = %d\n", qid);

   // srand (time (0));

   // for(cont = 0; cont < 10; cont++)
    //{
    //  sleep (rand()%4);
     msgrcv(qid, &buf, length, 1, 0);
      
      printf("SON - MESSAGE NUMBER : %d\n", buf.num);
        qid = msgget(msgkey, IPC_CREAT | 0660);
                buf.mtype = 1;
      buf.num = 1;
      msgsnd(qid, &buf, length, 0);


   // }

    return 0;
  }

  printf("FATHER - QID = %d\n", qid);

 // for(cont = 0; cont < 10; cont++)
 // {
   // sleep (rand()%4);
        buf.mtype = 1;
      buf.num = 0;
      msgsnd(qid, &buf, length, 0);
    
    printf("FATHER - MESSAGE NUMBER : %d\n",  buf.num);
    sleep(2);
    msgrcv(qid, &buf, length, 1, 0);
    printf("FATHER - MESSAGE NUMBER : %d\n",  buf.num);
 // }

  msgctl(qid, IPC_RMID, 0);

  return 0;
}*/
