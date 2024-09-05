#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
/**
 * Copie le contenu du fichier message.txt dans un fichier copie.txt
 */
int main(int argc, char **argv) {
    const char *nomFichierSource = argv[1]; //Chemin
    const char *nomFichierDestination = argv[2];

    int fichierSource = open(nomFichierSource, O_RDONLY); //Read Only


    int fichierDestination = open(nomFichierDestination, O_WRONLY | O_CREAT | O_TRUNC,0644);
    //En read only, le créé si inexistant ou le overwrite si déja présente, ajoute les droits rw pour u et r pour le reste

    char buffer[BUFFER_SIZE]; //Tampon pour stocker les valeurs du read

    ssize_t bytesRead;
    while((bytesRead = read(fichierSource,buffer,BUFFER_SIZE)) > 0){ //Tant que le nb de bytes lu est > 0
        write(fichierDestination,buffer,bytesRead); //écris le contenu de buffer dans stdout_fileno (console)
    }
}
