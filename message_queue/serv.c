#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <string.h>
#include <stdio.h>
 
int main (void) {
 
        key_t ipckey;
        int mq_id;
        struct msgbuf { long type; int a; } mymsg;
 
        /* Generate the ipc key */
        ipckey = ftok("/tmp/foo", 42);
        printf("My key is %d\n", ipckey);
 
        /* Set up the message queue */
        mq_id = msgget(ipckey, IPC_CREAT | 0666);
        printf("Message identifier is %d\n", mq_id);
 
        /* Send a message */
        //memset(mymsg.text, 0, 100); /* Clear out the space */
        //strcpy(mymsg.text, "Hello, world!");
        mymsg.a = 5;
        mymsg.type = 1;
        msgsnd(mq_id, &mymsg, sizeof(mymsg), 0);
        return 0;
 
}