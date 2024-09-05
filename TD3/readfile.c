#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h> //open
#include <unistd.h>

#include <errno.h> //erreurs

#define BUFFER_SIZE 1024
/**
 * Ouvre le fichier message.txt du répertoire courant et affiche son contenu à l'écran.
 */
int main(int argc, char **argv) {
    if(argc != 2){
        printf("Le nombre d'arguments passés en parametres est incorrect");
        exit(1);
    }

    const char *nomFichier = argv[1]; //Chemin
    int fd = open(nomFichier, O_RDONLY); //Read Only
    if(fd == -1){
        perror(argv[1]); //GESTION D'ERREURS
        exit(1);
    }
    
    
    char buffer[BUFFER_SIZE]; //Tampon pour stocker les valeurs du read

    ssize_t bytesRead;
    while((bytesRead = read(fd,buffer,BUFFER_SIZE)) > 0){ //Tant que le nb de bytes lu est > 0
        write(1,buffer,bytesRead); //écris le contenu de buffer dans stdout_fileno (console)
    }
    printf("\n");

    close(fd);
}
