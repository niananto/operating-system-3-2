#include "kernel/types.h"
#include "user/user.h"
#include "kernel/riscv.h"

int main(int argc, char *argv[]) {
    int *shared_mem = (int *)sbrk(PGSIZE);
    *shared_mem = 0;

    int pid = fork();
    if (pid < 0) {
        printf("fork failed\n");
    } else if (pid == 0) {
        // child process
        printf("Child process: shared_mem value is %d\n", *shared_mem);
        *shared_mem = 1;
        printf("Child process: modified shared_mem value to %d\n", *shared_mem);
        exit(0);
    } else {
        // parent process
        wait(0);
        printf("Parent process: shared_mem value is %d\n", *shared_mem);
        *shared_mem = 2;
        printf("Parent process: modified shared_mem value to %d\n", *shared_mem);
    }

    exit(0);
}
