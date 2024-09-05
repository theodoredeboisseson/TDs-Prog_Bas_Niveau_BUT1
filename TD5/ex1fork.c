#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char **argv) {
    int i;
    printf("PID: %d (avant fork)\n", getpid());
    i = fork();
    if (i != 0) {
        printf("PID: %d, résultat du fork: %d\n",getpid(),i);
    } else {
        printf("PID: %d, résultat du fork: %d \n",getpid(),i);
    }
    printf("PID: %d (après fork)\n", getpid());
}
