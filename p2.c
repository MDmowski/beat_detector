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

/* #include "messages.h" */ // Cant include bcz C++
#include "BTrack/BTrack.h"

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

mqd_t mqd_log;

struct log_msg
{
    unsigned int msg_id;
    unsigned char sender;
    struct timeval tv;
    enum MsgType {RECEIVED, SENT} type;
};

struct log_msg log_m;

mqd_t mq_open_log_wrapper(const char *name)
{
    mqd_t mqd = mq_open(name, O_WRONLY, S_IRUSR | S_IWUSR, NULL);
    if(mqd == (mqd_t) -1)
        handle_error("mq_open");

    return mqd;
}

int main()
{
    mqd_log = mq_open_log_wrapper("/LOG_MSG_QUEUE_2");
    log_m.msg_id = 0;
    log_m.sender = 2;

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

    char *buf = (char *)malloc(2048*sizeof(float));
    if (buf == NULL)
        handle_error("malloc");
    double* frames = (double *)malloc(1024*sizeof(double));
    if (frames == NULL)
        handle_error("malloc");
    uint frames_so_far = 0;

    //initializing with frame size 1024 and hop size 512
    BTrack b;

    while(1){

        /* printf("MSG size: %d\n", attr.mq_msgsize); */
        ssize_t bytes_received = mq_receive(mqd_1, buf, 2048*sizeof(float), NULL);
        if(bytes_received == -1)
            handle_error("mq_receive");

        log_m.type = log_msg::RECEIVED;
        if(mq_send(mqd_log, (const char *)&log_m, sizeof(struct log_msg), 1) == -1)
            handle_error("mq_send");
        log_m.msg_id++;



	uint frames_number = bytes_received/sizeof(float);
	float* array = (float*)buf;
        printf("P2 bytes: %d\n", bytes_received);
	uint free_space = 1024 - frames_so_far;
	if(free_space >= frames_number){
		for(int i=0; i < frames_number; i++)
			frames[frames_so_far + i] = ((double)array[i]);
		if(free_space == frames_number){
			frames_so_far = 0;
		//	BTrack
                        b.processAudioFrame(frames);
		}
		else
			frames_so_far += frames_number;
	}
	else{
		for(int i=0; i < free_space; i++)
			frames[frames_so_far + i] = ((double)array[i]);
		uint frames_left = frames_number - free_space;
		//BTrack
                        b.processAudioFrame(frames);
		for(int i=0; i < frames_left; i++)
			frames[i] = ((double)array[free_space + i]);
		frames_so_far = frames_left;
	}
        printf("P2 got message\n");
        if (b.beatDueInCurrentFrame()){
            printf("!!!!!!!!!!!!!!!!!!! BEAT !!!!!!!!!!!!!!!!!!!!!!\n");
            printf("%f\n", b.getCurrentTempoEstimate());
            char msg = 'K';
            if(mq_send(mqd_2, (const char *)&msg, 1, 1) == -1)
                handle_error("mq_send");

            log_m.type = log_msg::SENT;
            if(mq_send(mqd_log, (const char *)&log_m, sizeof(struct log_msg), 1) == -1)
                handle_error("mq_send");
            log_m.msg_id++;
        }
    }

    mq_close(mqd_log);
    mq_close(mqd_1);
    mq_close(mqd_2);

    printf("p1\n");

}
