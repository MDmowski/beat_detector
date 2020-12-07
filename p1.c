#include <stdio.h>
#include <stdlib.h>
#include <sys/prctl.h>
#include <unistd.h>
#include <signal.h>

#include <pthread.h> 
#include <mqueue.h> 
#include <signal.h>
#include <errno.h>
#include <sys/stat.h>

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

// TODO: Create header
struct log_msg
{
    unsigned int msg_id;
    struct timeval tv;
    enum MsgType {RECEIVED, SENT} type;
};

int main()
{
    mqd_t mqd = mq_open("/LOG_MSG_QUEUE_1", O_WRONLY, S_IRUSR | S_IWUSR, NULL);
    if(mqd == (mqd) -1)
        handle_error("mq_open(LOG_MSG_QUEUE_1)");

    struct log_msg msg;
    msg.msg_id = 0;

    if(mq_send(mqd, (const char *)&msg, sizeof(struct log_msg), 1) == -1)
        handle_error("mq_send");

    mq_unlink("/LOG_MSG_QUEUE_1");

    printf("p1\n");

}
