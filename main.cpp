#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/prctl.h>
#include <unistd.h>

#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <mqueue.h>
#include "BTrack/BTrack.h"

struct log_msg
{
    unsigned int msg_id;
    struct timeval tv;
    enum MsgType {RECEIVED, SENT} type;
};

double* addToBuffer(int size, int &currentSize, float newFrame[], double helperFrame[], double frame[])
{
    //return helperFrame pointer
    if(size == 0 && newFrame == nullptr){
        return frame;
    }
    //cast floats to doubles
    for(int i = 0; i < size; i++){
        helperFrame[currentSize + i] = (double) newFrame[i];
    }

    //check size
    currentSize += size;
    if(currentSize < 1024){
        return nullptr;
    } else {
        //copy helperFrame to frame and move helperFrame by 1024
        memcpy(frame, helperFrame, sizeof(double)*1024);
        for (int i = 0; i < 1024; i++){
            helperFrame[i] = helperFrame[1024 + i];
        }
        currentSize -= 1024;
        return frame;
    }
}

int main(int argc, const char * argv[])
{

    //audio frame size 1024

    double *frameptr = nullptr; //*frame is a pointer to array of doubles, size 1024
    double frame[1024]; //frame holding all 1024 sample values
    int currentSize = 0; //current size of processed frame
    double helperFrame[2048]; //helper frame for holding extra samples
    int messageCounter = 0; //counter of received frames
    int size = 0; //size of buffer
    float newFrame[2048]; //buffer for received messages from queue12 (float array without specific size, values between -1.0 and 1.0)

    //open log message queue for p2
    mqd_t logqueue2 = mq_open("/LOG_MSG_QUEUE_2", O_WRONLY, S_IRUSR | S_IWUSR, NULL);
    if(logqueue2 == -1){
        printf("error opening: %s\n", strerror(errno));
    } else {
        printf("successful opening LOG_MSG_QUEUE2\n");
    }

    //log messages
    struct log_msg msg_sent, msg_received;
    msg_received.type = log_msg::RECEIVED;
    msg_sent.type = log_msg::SENT;

    //open message queue between p1 and p2
    mqd_t queue12 = mq_open("/MSG_QUEUE_12", O_RDONLY, S_IRUSR | S_IWUSR, NULL);
    if(queue12 == -1){
        printf("error opening: %s\n", strerror(errno));
    } else {
        printf("successful opening\n");
    }

    //open message queue between p2 and p3
    mqd_t queue23 = mq_open("/MSG_QUEUE_23", O_WRONLY, S_IRUSR | S_IWUSR, NULL);
    if(queue23 == -1){
        printf("error opening: %s\n", strerror(errno));
    } else {
        printf("successful opening\n");
    }

    //initializing with frame size 1024 and hop size 512
    BTrack b;

    while(1){

        size = 0;

        //receive message from queue12
        //size = mq_receive(queue12, newFrame, sizeof(float)*2048, NULL);
        if(size == -1){
            printf("error receiving: %s\n", strerror(errno));
        } else {
            printf("successful receive from MSG_QUEUE_12\n");
            messageCounter++;

            //send log receive message to parent process
            msg_received.msg_id = messageCounter;

            if(mq_send(logqueue2, (const char *)&msg_received, sizeof(struct log_msg), 1) == -1){
                printf("error sending: %s\n", strerror(errno));
            } else {
                printf("successful send to LOG_MSG_QUEUE_2\n");
            }
        }



        //check if new length of frame < 1024
        while (addToBuffer(size, currentSize, newFrame, helperFrame, frame) == nullptr){

            //receive another message
            size = 0;

            //receive message from queue12
            //size = mq_receive(queue12, newFrame, sizeof(float)*2048, NULL);
            if(size == -1){
                printf("error receiving: %s\n", strerror(errno));
            } else {
                printf("successful receive from MSG_QUEUE_12\n");
                messageCounter++;

                //send log receive message to parent process
                msg_received.msg_id = messageCounter;

                if(mq_send(logqueue2, (const char *)&msg_received, sizeof(struct log_msg), 1) == -1){
                    printf("error sending: %s\n", strerror(errno));
                } else {
                    printf("successful send to LOG_MSG_QUEUE_2\n");
                }
            }
        }
        //if yes, get pointer to array
        frameptr = addToBuffer(0, currentSize, nullptr, helperFrame, frame);


        //process frame
        b.processAudioFrame(frameptr);
        //check if current frame has beat in it
        if (b.beatDueInCurrentFrame())
        {
            //if yes, send message to queue 23 signaling beat
            int message = 1;
            if(mq_send(queue23, (const char *)&message, sizeof(int), 1) == -1){
                printf("error sending: %s\n", strerror(errno));
            } else {
                printf("successful send to MSG_QUEUE_23\n");
            }

            //send log sent message to parent process
            msg_sent.msg_id = messageCounter;

            if(mq_send(logqueue2, (const char *)&msg_sent, sizeof(struct log_msg), 1) == -1){
                printf("error sending: %s\n", strerror(errno));
            } else {
                printf("successful send to LOG_MSG_QUEUE_2\n");
            }
        }
    }
    mq_close(queue23);
    mq_close(queue12);
    mq_close(logqueue2);

    return 0;
}
