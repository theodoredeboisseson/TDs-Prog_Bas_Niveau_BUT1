#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#define BUFFER_SIZE 4096

int main(int argc, char **argv) {
	if(argc != 3){
        printf("Number of arguments incorrect for %s, usage : input.wav output.wav",argv[0]);
        return 1;
    }
	unsigned char *buffer = malloc(BUFFER_SIZE * sizeof(unsigned char));	// buffer utilisée pour les lectures/écritures
	int nbread;	// nombre d'octets lus après chaque appel à read


	// [1] Ouverture des fichiers en entrée et sortie
	// le nom du fichier à lire est passé en premier argument du programme, le
	// nom du fichier dans lequel écrire le résultat est passé en second
	// argument
    int fdin = open(argv[1],O_RDONLY);  // descripteur de fichier du fichier ouvert en lecture
    int fdout = open(argv[2],O_WRONLY|O_TRUNC|O_CREAT,0644); // descripteur de fichier du fichier ouvert en écriture
    if(fdin == -1 | fdout == -1){
        perror("Erreur lors de l'ouverture des fichiers");
        return 1;
    }

	// [2] Lecture de l'entête du fichier en entrée, modification des données
	// nécessaires dans le buffer et écriture de l'entête modifié dans le
	// fichier en sortie
    read(fdin, buffer, 44);

    short int *NbrCanaux = (short int*) (buffer+22);
    if(*NbrCanaux != 2){
        printf("Le nombre de canaux est différent de 2, conversion de stéréo vers mono est impossible\n");
        exit(1);
    }

    int *FileSize = (int*) (buffer+4);
    int *BytePerSec = (int*) (buffer+28);
    short int *BytePerBlock = (short int*) (buffer+32);
    int *DataSize = (int*) (buffer+40);

    printf("FileSize :%i NbrCanaux :%i BytePerSec :%i BytePerBlock :%i DataSize :%i\n",*FileSize, *NbrCanaux, *BytePerSec, *BytePerBlock, *DataSize);

    *FileSize = *FileSize/2 +22;
    *DataSize = *DataSize / 2;
    *NbrCanaux = 1;
    *BytePerSec = *BytePerSec / 2;
    *BytePerBlock = *BytePerBlock / 2;

    printf("FileSize :%i NbrCanaux :%i BytePerSec :%i BytePerBlock :%i DataSize :%i\n",*FileSize, *NbrCanaux, *BytePerSec, *BytePerBlock, *DataSize);

    write(fdout,buffer,44);

	// [3] Lecture des données du fichier en entrée (par blocs de 4096 octets)
	// et écriture dans le fichier en sortie des octets correspondant aux
	// échantillons du premier canal.
    unsigned char *halfBuffer = malloc(BUFFER_SIZE/2 * sizeof(unsigned char));
    nbread = BUFFER_SIZE;
    while(nbread == BUFFER_SIZE){
        nbread = read(fdin,buffer,BUFFER_SIZE);
        for(int i = 0; i < BUFFER_SIZE; i += 2){
            buffer[i] = buffer[2*i];
            buffer[i+1] = buffer[2*i+1];
        }
        write(fdout,buffer,nbread/2);
    }

	close(fdin);
	close(fdout);
    return 0;
}