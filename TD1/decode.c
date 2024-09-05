#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

/**
 * Decompresse les octets contenus dans buffer_in
 *
 * @param buffer_in buffer contenant les octets à décompresser. Les deux
 * premiers octets représentent la taille totale (en octets) des données
 * décompressées
 * @param buffer_out buffer où écrire les données décompressées
 *
 * @return le nombre d'octets décompressés (écrits dans buffer_out)
 */
size_t carmack_decode(unsigned char *buffer_in, unsigned char *buffer_out) {
  // à compléter
  return 0; // à modifier
}

int main() {
  // à compléter
  printf("Ça marche !\n");
}
