#include <sys/types.h>
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
#define SEMAPHORE_NAME "/my_named_semaphore"
int main()
{
    int client_sockfd, listener, status, struct_len,flag=0,j,z,flag1=0,ret;
    struct sockaddr_in addr,client_address;
    char buf[1024], *ip_serv = "10.0.2.15",*p = "start serv";
    int bytes_read,i,mq_id;
    pid_t pid;
    key_t ipckey;
    sem_t *sem;
    fd_set rset;
    struct msgbuf { 
      long type; 
      int fd; 
      int employment[2][3]; 
    } mymsg;
    struct pollfd fds[1];
    ipckey = ftok("/tmp/q",'m');
    printf("My key is %d\n", ipckey);
    // Set up the message queue 
    mq_id = msgget(ipckey, IPC_CREAT | 0666);
    mymsg.type = 1;
    /*if ( (sem = sem_open(SEMAPHORE_NAME, O_CREAT, 0777, 0)) == SEM_FAILED ) 
    {
        perror("sem_open");
        return 1;
    }*/
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
   // printf ("start serv\n");
    //msgctl(mq_id, IPC_RMID, 0);
   // sleep(20);
    fds[0].fd = listener;//дискриптор сокета на котором должно рпоизойти событие
    fds[0].events = POLLIN;//указывает на битовую маску событийсобытия,чтение, POLLOUT запись
    while(1)
    {
      if(flag == 1)
      {
        ret = poll(fds, 2, -1);
        if ( ret < 1) //второй параметр - кол-во задаваемых структур, третий таймаут в милисек, отриц - бесконечный таймер
        {
          close(listener);
          exit(EXIT_FAILURE);
        }
        else
          if ( fds[0].revents == POLLIN ) 
          {
            fds[0].revents = 0;
            write(1,"\n1\n",4);
          }
      }
      else
      {
        for(i=0; i < 3; i++)
        {  
          switch(pid = fork())
          {
            case -1:
              perror("fork");
              break;
            //сокета есть счетчик дискрипторов, открытых в настоящий момент, так как исп функция fork() то вместо одного дискриптора стало два, поэтому чтобы закрыть сокет нужно закрыть все дискрипторы
            case 0:
              while(1)
              {
                  client_sockfd= accept(listener, (struct sockaddr *)&client_address, (socklen_t *)&struct_len);
                  if(client_sockfd < 0)
                  {
                    write(1, "star1:\n", 9);
                    perror("accept");
                    exit(3);
                  } 
                  bytes_read = recv(client_sockfd, buf, 1024, 0);
                  if(bytes_read == 0) 
                  {
                    printf("\nSTOP child:%d\n",getpid());
                   // return 0;
                  }
                  send(client_sockfd, "\nbuf\n", 6, 0);
                  //write(1, "close_client:", 13);
                  printf ("close client by child:%d\n",getpid());
                  close(client_sockfd);//обслужив клиента закрываем дискриптор по работе с клиентом
                 /* msgrcv(mq_id, &mymsg, sizeof(mymsg), 1, 0);
                  for(j=0;j<3;j++)
                  {
                    if(mymsg.employment[0][j] == getpid())
                    {
                      mymsg.employment[1][j] = 1;
                    }
                  }
                  msgsnd(mq_id, &mymsg, sizeof(mymsg), 0);
                  printf("child_%d: \n",getpid());
                  for(j=0;j<2;j++)
                  {
                    for(j=0;z<3;j++) 
                      printf("%d",mymsg.employment[j][z]);
                   // printf("\n");
                  }
                 // printf("\n");*/
                //printf ("child:%d\n",getpid());
               // if (sem_wait(sem) == 0 )
                //  perror("sem_wait");
                //msgrcv(mq_id, &mymsg, sizeof(mymsg), 1, 0);
               // close(listener);
               // client_sockfd = accept(listener, (struct sockaddr *)&client_address, (socklen_t *)&struct_len);
               // msgrcv(mq_id, &mymsg, sizeof(mymsg), 1, 0);
              //  printf("\nfd child = %d:\n",mymsg.fd);
              //  for(j=0;j<2;j++)
              //  {
              //    for(z=0;z<3;z++) 
              //      printf("%d ",mymsg.employment[j][z]);
              //    printf("\n");
              //  }
               // write(1,"\n-2-\n",4);
                
                //printf("-");
                  //return 0;
                  //msgrcv(mq_id, &mymsg, sizeof(mymsg), 1, 0);
                  //printf("\n mymsg.a = %d\n",mymsg.a);
                  /*if(mymsg.a == 0)
                  {
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
                  }  */ 
                  //write(1,"\n-3-\n",4);            
              }
            default:
             // printf ("getpid:%d getppid:%d\n",getpid(),getppid());
              mymsg.employment[0][i] = pid;
              printf("\n%d\n",pid);
              mymsg.employment[1][i] = 0;
          }
        }
        flag = 1;
      }
    }
  //  for( i = 0; i < 2; i++ ) 
  //  {
    //    wait( NULL ); 
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
