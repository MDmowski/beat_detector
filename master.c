#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>

int main()
{
    printf("parent start\n");
    pid_t pid = fork();

    if( pid == 0)
        execve("./p1.o", 0, 0);

    int status;
    wait(&status);

    printf("parent end\n");
}
