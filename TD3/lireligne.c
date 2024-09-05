#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFFER_SIZE 4096
/**
 * Lit une ligne du fichier correspondant au descripteur passé en argument jusqu'à lire un retour à la ligne ('\n') ou
 * avoir lu size caractères.
 *
 * @param fd descripteur de fichier ouvert en lecture
 * @param s buffer dans lequel les octets lus sont écrits
 * @param size nombre maximum d'octets à lire
 * @return le nombre d'octets effectivement lus
 */
int lireligne(int fd, char *s, int size) {
    int bytesRead = 0;
    char c = '\0';

    while(bytesRead < size && c != '\n'){
        read(fd,&c,1);
        s[bytesRead] = c;
        bytesRead++;
    }

    for(int i = 0; i < bytesRead; i++){
        printf("%c",s[i]);
    }

    return bytesRead;
}


int lireligneBloc(int fd, char *s, int size) {
    for(int i = 0; i < size; i++){s[i] = '\0';}
    char tab[size];
    int bytesRead = read(fd,tab,size);
    int i = 0;

    while(tab[i] != '\n' && i < bytesRead) {
        s[i] = tab[i];
        i++;
    }

    lseek(fd, -(bytesRead-i)+1,SEEK_CUR); //si trouve \n alors décale curseur
    printf("%s\n",s);
    return i;
}

int main() {
    int fd = open("message.txt", O_RDONLY);
    char buffer[BUFFER_SIZE];
    //printf("%i\n",lireligne(fd,buffer,BUFFER_SIZE));
    printf("%i\n",lireligneBloc(fd,buffer,BUFFER_SIZE));
    printf("%i\n",lireligneBloc(fd,buffer,BUFFER_SIZE));
    printf("%i\n",lireligneBloc(fd,buffer,BUFFER_SIZE));

    close(fd);
}