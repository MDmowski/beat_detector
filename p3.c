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

struct timespec timespec;
int counter;

int main()
{
    mqd_t mqd_log = mq_open_log_wrapper("/LOG_MSG_QUEUE_3");

    mqd_t mqd_2 = mq_open("/MSG_QUEUE_2", O_RDONLY, S_IRUSR | S_IWUSR, NULL);
    if(mqd_2 == (mqd_t) -1)
        handle_error("mq_open");

    struct log_msg msg;

    struct mq_attr attr;
    if (mq_getattr(mqd_2, &attr) == -1)
        handle_error("mq_getattr");

    char *buf = malloc(8192);
    if (buf == NULL)
        handle_error("malloc");


    while(1){

            
        ssize_t bytes_received = mq_receive(mqd_2, buf, 8192, NULL);
        if(bytes_received == -1)
            handle_error("mq_receive");
	clock_gettime(CLOCK_REALTIME, &timespec);
	FILE* receive_log_file = fopen("p3_receive.csv","a");
    	fprintf(receive_log_file,"%d,%jd.%ld\n",counter, timespec.tv_sec, timespec.tv_nsec);
	fclose(receive_log_file);
	counter++;
        printf("P3 got message\n");

	

        // Proccess data
        
    }

    mq_close(mqd_log);
    mq_close(mqd_2);

    printf("p3\n");

}
