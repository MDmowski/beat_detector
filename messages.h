#ifndef MASSAGE_H
#define MASSAGE_H

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

struct log_msg
{
    unsigned int msg_id;
    unsigned char sender;
    struct timeval tv;
    enum MsgType {RECEIVED, SENT} type;
};


struct p1_msg
{
    float *frame;
    int framesNumber;

};

struct p2_msg
{
};

mqd_t mq_create_log_wrapper(const char *name)
{
    mqd_t mqd = mq_open(name, O_CREAT | O_EXCL | O_RDONLY | O_NONBLOCK, S_IRUSR | S_IWUSR, NULL);
    if(mqd == (mqd_t) -1)
        handle_error("mq_open");

    return mqd;
}

void mq_create_wrapper(const char* name)
{
    /* struct mq_attr attr; */
    /* attr.mq_flags = 0; */
    /* attr.mq_maxmsg = 10; */
    /* attr.mq_msgsize = 8192; */
    /* attr.mq_curmsgs = 0; */
    mqd_t mqd = mq_open(name, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, NULL);
    if(mqd == (mqd_t) -1)
        handle_error("mq_open");

    mq_close(mqd);
}

mqd_t mq_open_log_wrapper(const char *name)
{
    mqd_t mqd = mq_open(name, O_WRONLY, S_IRUSR | S_IWUSR, NULL);
    if(mqd == (mqd_t) -1)
        handle_error("mq_open");

    return mqd;
}

void mq_notify_wrapper(mqd_t *mqd_ptr, void (*callback)())
{
    struct sigevent sev;
    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_function = callback; 
    sev.sigev_notify_attributes = NULL;
    sev.sigev_value.sival_ptr = mqd_ptr;

    if (mq_notify(*mqd_ptr, &sev) == -1)
        handle_error("mq_notify");
}

#endif
