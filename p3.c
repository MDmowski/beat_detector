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
#include <wiringPi.h>

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

int main()
{
    mqd_t mqd_log = mq_open_log_wrapper("/LOG_MSG_QUEUE_3");

    mqd_t mqd_2 = mq_open("/MSG_QUEUE_2", O_RDONLY, S_IRUSR | S_IWUSR, NULL);
    if(mqd_2 == (mqd_t) -1)
        handle_error("mq_open");

    struct log_msg msg;
    log_m.msg_id = 0;
    log_m.type = 0;
    log_m.sender = 3;

    struct mq_attr attr;
    if (mq_getattr(mqd_2, &attr) == -1)
        handle_error("mq_getattr");

    char *buf = malloc(8192);
    if (buf == NULL)
        handle_error("malloc");

    wiringPiSetup () ;
    pinMode (7, OUTPUT) ;

    while(1){
            
        ssize_t bytes_received = mq_receive(mqd_2, buf, 8192, NULL);
        if(bytes_received == -1)
            handle_error("mq_receive");

        if(mq_send(mqd_log, (const char *)&log_m, sizeof(struct log_msg), 1) == -1)
            handle_error("mq_send");
        log_m.msg_id++;

        digitalWrite (7, HIGH) ; delay (50) ;
        digitalWrite (7,  LOW);

        
    }

    mq_close(mqd_log);
    mq_close(mqd_2);

    printf("p3\n");

}
