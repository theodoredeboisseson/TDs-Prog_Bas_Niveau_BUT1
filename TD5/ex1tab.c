#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
    int i, j, s, tabpid[3];
    printf("[avant fork] PID: %d\n", getpid());
    for(j = 0; j < 3; j++) {
        tabpid[j] = fork();
        if (tabpid[j] != 0 ) {
            printf("PID: %d, retour fork: %d \n",getpid(),tabpid[j]);
        } else {
            printf("PID: %d \n",getpid());
            exit(j);
        }
    }
    for(j = 0; j < 3; j++) {
        i= waitpid(tabpid[j], &s, 0);
        if (i > 0) {
            printf("terminé PID: %d\n", i);
        }
        sleep(1);
    }
}