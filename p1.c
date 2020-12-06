#include <stdio.h>
#include <stdlib.h>
#include <sys/prctl.h>
#include <unistd.h>
#include <signal.h>

int main()
{
    /* int r = prctl(PR_SET_PDEATHSIG, SIGTERM); */
    /* if (r == -1) { perror(0); exit(1); } */
    // race condtion
    /* if (getppid() != ppid_before_fork) */
    /*     exit(1); */

    sleep(10);
    printf("p1\n");
}
