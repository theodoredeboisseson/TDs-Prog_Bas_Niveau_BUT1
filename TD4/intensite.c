#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

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
    if(argc != 4){
        printf("Number of arguments incorrect for %s, usage : input.pgm output.pgm k\n",argv[0]);
        return 1;
    }

    int fd_in = open(argv[1],O_RDONLY);  // descripteur de fichier du fichier ouvert en lecture
    int fd_out = open(argv[2],O_WRONLY|O_TRUNC|O_CREAT,0644); // descripteur de fichier du fichier ouvert en écriture
    if(fd_in == -1 | fd_out == -1){
        perror("Erreur lors de l'ouverture des fichiers\n");
        return 1;
    }

    int k = atoi(argv[3]);
    if(k <= 0){
        printf("k doit être positif!\n");
        return 1;
    }

    unsigned char *buffer = malloc(4096 * sizeof(unsigned char));    // buffer de lecture    
    int nbread;

    for(int i = 0; i < 3; i++){
        nbread = lireligne(fd_in,buffer,4096);
        write(fd_out,buffer,nbread);
    }

    nbread = 4096;
    while(nbread == 4096){
        nbread = read(fd_in,buffer,4096);
        for(int i = 0; i < nbread; i++){
            if(buffer[i] + k <= 255){
                buffer[i] = buffer[i] + k;
            } else {
                buffer[i] = 255;
            }
        }
        write(fd_out,buffer,nbread);
    }

    close(fd_in);
    close(fd_out);
    return 0;
}