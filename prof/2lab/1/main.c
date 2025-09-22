#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(void){
    pid_t pid = getpid();
    pid_t ppid = getppid();
    pid_t pgid = getpgrp();
    uid_t ruid = getuid();
    gid_t rgid = getgid();
    uid_t euid = geteuid();
    gid_t egid = getegid();

    printf("Process ID (PID): %d\n", (int)pid);
    printf("Parent Process ID (PPID): %d\n", (int)ppid);
    printf("Process Group ID (PGID): %d\n", (int)pgid);
    printf("Real User ID (RUID): %d\n", (int)ruid);
    printf("Real Group ID (RGID): %d\n", (int)rgid);
    printf("Effective User ID (EUID): %d\n", (int)euid);
    printf("Effective Group ID (EGID): %d\n", (int)egid);

    return 0;
}
