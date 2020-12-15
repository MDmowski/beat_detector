#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <stdlib.h>

#include <pthread.h> 
#include <mqueue.h> 
#include <signal.h>
#include <errno.h>
#include <sys/stat.h>

#include "messages.h"

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

static void read_msg(union sigval sv)
{
    printf("Received signal.\n");
    struct mq_attr attr;
    ssize_t bytes_received;
    char *buf;
    struct log_msg *log_msg_buf;
    mqd_t mqd = *((mqd_t *) sv.sival_ptr);

    // Register for notify again
    // Can't user wrapper because of error: device busy
    struct sigevent sev;
    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_function = &read_msg;
    sev.sigev_notify_attributes = NULL;
    sev.sigev_value.sival_ptr = sv.sival_ptr; 
    if (mq_notify(mqd, &sev) == -1)
        handle_error("mq_notify");

    if (mq_getattr(mqd, &attr) == -1)
        handle_error("mq_getattr");

    buf = malloc(attr.mq_msgsize);
    if (buf == NULL)
        handle_error("malloc");

    // For notifes to work we have to empty the queue
    while(1)
    {
        bytes_received = mq_receive(mqd, buf, attr.mq_msgsize, NULL);
        if (bytes_received != sizeof(struct log_msg))
        {
            if (errno != EAGAIN)
                handle_error("mq_receive");
            else
                break; // Expected error, queue is empty
        }

        // To read contents of struct we have to cast it to appropiate type
        log_msg_buf = (struct log_msg *) buf;

        // Simple log, delete later
<<<<<<< HEAD
        printf("Read %zd bytes from MQ: %u\n", bytes_received, log_msg_buf->sender);
=======
        //printf("Read %zd bytes from MQ: %u\n", bytes_received, log_msg_buf->msg_id);
>>>>>>> producer-develop
    }

    free(log_msg_buf);
}


int main()
{
    printf("parent start\n");

    // Make sure queues are deleted
    mq_unlink("/LOG_MSG_QUEUE_1");
    mq_unlink("/LOG_MSG_QUEUE_2");
    mq_unlink("/LOG_MSG_QUEUE_3");
    mq_unlink("/MSG_QUEUE_1");
    mq_unlink("/MSG_QUEUE_2");

<<<<<<< HEAD
    mqd_t mqd_log_1 = mq_create_log_wrapper("/LOG_MSG_QUEUE_1");
    mq_notify_wrapper(&mqd_log_1, &read_msg);
=======
mqd_t mq_open_wrapper(const char *name)
{
    mqd_t mqd = mq_open(name, O_CREAT | O_EXCL | O_RDWR , S_IRUSR | S_IWUSR, NULL);
    if(mqd == (mqd) -1)
        handle_error("mq_open");
>>>>>>> producer-develop

    mqd_t mqd_log_2 = mq_create_log_wrapper("/LOG_MSG_QUEUE_2");
    mq_notify_wrapper(&mqd_log_2, &read_msg);

    mqd_t mqd_log_3 = mq_create_log_wrapper("/LOG_MSG_QUEUE_3");
    mq_notify_wrapper(&mqd_log_3, &read_msg);

<<<<<<< HEAD
    mq_create_wrapper("/MSG_QUEUE_1");
    mq_create_wrapper("/MSG_QUEUE_2");

    // Create first process
    pid_t pid = fork();
    if( pid == 0){
        execve("./p1.o", NULL, NULL);
        printf("execve error\n");
    }

    pid = fork();
    if( pid == 0){
        execve("./p2.o", NULL, NULL);
        printf("execve error\n");
    }


=======
    mqd_t mqd_log_1 = mq_open_wrapper("/TEST");
    if(mqd_log_1 == -1)
	    perror("Open:");
    //mq_notify_wrapper(mqd_log_1);
    float msg[4096];
    while(1){
	size_t size = mq_receive(mqd_log_1, msg, sizeof(float)*4096, NULL);
	printf("%s\n",msg);
    }
    // Create first process
    pid_t pid2 = fork();
    char* argv[] = {"./playback","../../test2.wav", NULL};
    if( pid2 == 0){
       if( execve("./playback", argv, NULL)==-1)
	printf("Couldn't open");
    }
>>>>>>> producer-develop
    // TODO: Add waiting for all children
    int status;
    wait(&status);

    mq_close(mqd_log_1);
    mq_unlink("/MSG_QUEUE_1");

    mq_close(mqd_log_2);
    mq_unlink("/LOG_MSG_QUEUE_2");

    mq_close(mqd_log_3);
    mq_unlink("/LOG_MSG_QUEUE_3");

    mq_unlink("/MSG_QUEUE_1");
    mq_unlink("/MSG_QUEUE_2");

    printf("parent end\n");
}
