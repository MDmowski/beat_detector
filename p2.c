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
    mqd_t mqd_log = mq_open_log_wrapper("/LOG_MSG_QUEUE_2");

    mqd_t mqd_1 = mq_open("/MSG_QUEUE_1", O_RDONLY, S_IRUSR | S_IWUSR, NULL);
    if(mqd_1 == (mqd_t) -1)
        handle_error("mq_open");

    mqd_t mqd_2 = mq_open("/MSG_QUEUE_2", O_WRONLY, S_IRUSR | S_IWUSR, NULL);
    if(mqd_2 == (mqd_t) -1)
        handle_error("mq_open");

    struct log_msg msg;

    struct mq_attr attr;
    if (mq_getattr(mqd_1, &attr) == -1)
        handle_error("mq_getattr");

    char *buf = malloc(attr.mq_msgsize);
    if (buf == NULL)
        handle_error("malloc");

    while(1){

        ssize_t bytes_received = mq_receive(mqd_1, buf, attr.mq_msgsize, NULL);
        printf("P2 bytes: %d\n", bytes_received);

        printf("P2 got message\n");

        // Proccess data
        
        // Send new data
        /* if(mq_send(mqd_2, (const char *)&new_msg, sizeof(struct p2_msg), 1) == -1) */
        /*     handle_error("mq_send"); */

        /* sleep(0.5); */
    }

    mq_close(mqd_log);
    mq_close(mqd_1);
    mq_close(mqd_2);

    printf("p1\n");

}
