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

    char *buf = malloc(2048*sizeof(float));
    if (buf == NULL)
        handle_error("malloc");
    double* frames = malloc(1024*sizeof(double));
    if (frames == NULL)
        handle_error("malloc");
    uint frames_so_far = 0;
    while(1){

        ssize_t bytes_received = mq_receive(mqd_1, buf, attr.mq_msgsize, NULL);
	uint frames_number = bytes_received/sizeof(float);
	float* array = (float*)buf;
        printf("P2 bytes: %d\n", bytes_received);
	uint free_space = 1024 - frames_so_far;
	if(free_space >= frames_number){
		for(int i=0; i < frames_number; i++)
			frames[frames_so_far + i] = (double)array[i];
		if(free_space == frames_number){
			frames_so_far = 0;
		//	BTrack
		}
		else
			frames_so_far += frames_number;
	}
	else{
		for(int i=0; i < free_space; i++)
			frames[frames_so_far + i] = (double)array[i];
		uint frames_left = frames_number - free_space;
		//BTrack
		for(int i=0; i < frames_left; i++)
			frames[i] = (double)array[free_space + i];
		frames_so_far = frames_left;
	}
        printf("P2 got message\n");
	//for(int i=0; i<1024; i++){
	//	printf("c%d:%f ",i, frames[i]);
	//}
	//printf("\n\n");
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
