#include <stdio.h>
#include <unistd.h>

int main(void) {


    /*int pid;
    pid = fork();
    printf("%d\n", pid);


    fork();
    printf("Hi\n");
    fork();
    printf("Hi\n");


    if (fork() || fork()) fork();
    printf("forked! %d\n", fork());
     */

    if (fork() && (!fork())) {
        if (fork() || fork()) fork();
    }
    printf("52\n");

    return 0;
}

