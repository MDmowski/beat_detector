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

#include "messages.h"

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

int main()
{
    mqd_t mqd_log = mq_open_log_wrapper("/LOG_MSG_QUEUE_1");

    mqd_t mqd_1 = mq_open("/MSG_QUEUE_1", O_WRONLY, S_IRUSR | S_IWUSR, NULL);
    if(mqd_1 == (mqd_t) -1)
        handle_error("mq_open");

    struct log_msg log;
    struct p1_msg new_msg;

    for(int i = 0; i < 5; i++)
    {
        printf("Sending msg %d\n", i);
        log.sender = 1;

        // Fill in new data
        new_msg.frame[0] = 2;


        if(mq_send(mqd_log, (const char *)&log, sizeof(struct log_msg), 1) == -1)
            handle_error("mq_send");

        if(mq_send(mqd_1, (const char *)&new_msg, sizeof(struct p1_msg), 1) == -1)
            handle_error("mq_send");

        sleep(0.1);
    }

    mq_close(mqd_log);
    mq_close(mqd_1);

    printf("p1\n");

}
