#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main (int argc, char const **argv) {
    pid_t pid;
    int fd[2], nbread;
    char buffer[4096];

    pipe(fd);   // création du tube
    pid = fork();
    if (pid != 0) {
        // envoi des données dans le tube
        write(fd[1], "Bonjour\n", 8);
    } else {
        // lecture des données dans le tube
        nbread = read(fd[0], buffer, 4096);
        // affichage des données reçues
        write(1, buffer, nbread);
    }
}
