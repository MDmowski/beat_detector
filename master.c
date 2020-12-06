#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>

#include <pthread.h> 
#include <mqueue.h> 
#include <signal.h>
#include <errno.h>
#include <sys/stat.h>

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

struct log_msg
{
    unsigned int msg_id;
    struct timeval tv;
    enum MsgType {RECEIVED, SENT} type;
}

static void read_msg(union sigval sv)
{
    struct mq_attr attr;
    ssize_t bytes_received;
    char *log_msg_buf;
    mqd_t mqdes = *((mqd_t *) sv.sival_ptr);

    /* Determine max. msg size; allocate buffer to receive msg */

    if (mq_getattr(mqdes, &attr) == -1)
        handle_error("mq_getattr");
    log_msg_buf = malloc(attr.mq_msgsize);
    if (buf == NULL)
        handle_error("malloc");

    bytes_received = mq_receive(mqdes, log_msg_buf, attr.mq_msgsize, NULL);
    if (bytes_received != sizeof(struct log_msg))
        handle_error("mq_receive");

    printf("Read %zd bytes from MQ: %u\n", bytes_received, (struct log_msg *) log_msg_buf->msg_id);
    free(log_msg_buf);
}

void mq_notify_wrapper(mqd_t *mqd_ptr)
{
    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_function = &read_msg;
    sev.sigev_notify_attributes = NULL;
    sev.sigev_value.sival_ptr = mqd_ptr;

    if (mq_notify(mqdes, &sev) == -1)
        handle_error("mq_notify");
}

mqd_t mq_open_wrapper(const char *name)
{
    mqd_t mqd = mq_open(name, O_CREAT | O_EXCL | O_RDONLY, S_IRUSR | S_IWUSER, NULL);
    if(mqd == (mqd) -1)
        handle_error("mq_open(LOG_MSG_QUEUE_1)");

    return mqd;
}

int main()
{
    printf("parent start\n");

    mqd_t mqd_log_1 = mq_open_wrapper("/LOG_MSG_QUEUE_1");
    mq_notify_wrapper(&mqd_log_1);

    pid_t pid = fork();
    if( pid == 0)
        execve("./p1.o", NULL, NULL);


    int status;
    wait(&status);

    mq_close(mqd_log_1);

    printf("parent end\n");
}
