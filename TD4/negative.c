#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 4096

int lireligne(int fd, char *buffer, int size) {
	ssize_t nbread = read(fd, buffer, size);
	if (nbread == -1) {
		return -1;
	}

	int i;
	for (i = 0; i < nbread; i++) {
		if (buffer[i] == '\n') {
			i++;
			break;
		}
	}
	lseek(fd, i - nbread, SEEK_CUR);
	return i;
}


int main(int argc, char **argv) {
    if(argc != 3){
        printf("Number of arguments incorrect for %s, usage : input.pgm output.pgm",argv[0]);
        return 1;
    }

    int fd_in = open(argv[1],O_RDONLY);  // descripteur de fichier du fichier ouvert en lecture
    int fd_out = open(argv[2],O_WRONLY|O_TRUNC|O_CREAT,0644); // descripteur de fichier du fichier ouvert en écriture

    if(fd_in == -1 | fd_out == -1){
        perror("Erreur lors de l'ouverture des fichiers");
        return 1;
    }


    int nbread;
    unsigned char *buffer = malloc(BUFFER_SIZE * sizeof(unsigned char));    // buffer de lecture
    for(int i = 0; i < 3; i++){
        nbread = lireligne(fd_in,buffer,BUFFER_SIZE);
        write(fd_out,buffer,nbread);
    }
    
    nbread = BUFFER_SIZE;
    while(nbread == BUFFER_SIZE){
        nbread = read(fd_in,buffer,BUFFER_SIZE);
        for(int i = 0; i < nbread; i++){
            buffer[i] = 255 - buffer[i];
        }
        write(fd_out,buffer,nbread);
    }

    close(fd_in);
    close(fd_out);
    return 0;
}