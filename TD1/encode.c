/**
 * Ce fichier est donné à titre indicatif. Il n'est pas nécessaire de le lire
 * pour réaliser le TD mais présente une façon naïve de compresser des données
 * selon l'algorithme de Carmack.
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#define NEAR_POINTER_BYTE 0xA7
#define FAR_POINTER_BYTE 0xA8

/**
 * Compresse les octets contenus dans buffer_in en utilisant l'algorithme de
 * compression de Carmack.
 *
 * Prérequis: la taille des données en entrée doit être paire et < 65536.
 *
 * @param buffer_in buffer contenant les octets à compresser
 * @param buffer_out adresse d'un buffer pour que la fonction puisse y placer le
 * tableau compressé
 * @param length longueur des données dans buffer_in (doit être paire et <
 * 65536)
 *
 * @return nombre d'octets écrits dans le buffer pointé par buffer_out
 */
size_t carmack_encode(unsigned char *buffer_in, unsigned char **buffer_out,
                      size_t input_size) {
  // tableau en entrée interprété comme un tableau de mots de 16 bits
  uint16_t *words_in = (uint16_t *)buffer_in;
  // nombre de mots (16 bits) dans les données en entrée
  size_t word_size = input_size >> 1;
  // index courant (en mots) dans words_in
  size_t index_in = 0;
  // index courant (en octets) dans buffer_out
  size_t index_out = 0;

  size_t output_size = (size_t)(1.5 * input_size) + 2;
  *buffer_out = (unsigned char *)malloc(output_size);

  memcpy((*buffer_out), &input_size, 2);
  index_out += 2;

  while (index_in < word_size) {
    // position (relative) de la meilleure séquence "proche" trouvée
    size_t best_near_offset = 0;
    // longueur de la meilleure séquence "proche" trouvée
    size_t best_near_length = 0;
    // position (absolue) de la meilleure séquence "lointaine" trouvée
    size_t best_far_offset = 0;
    // longueur de la meilleure séquence "lointaine" trouvée
    size_t best_far_length = 0;

    size_t i, j;
    for (i = 0; i < index_in; i++) {
      j = 0;
      while (j < 255 && i + j < index_in && index_in + j < word_size &&
             words_in[i + j] == words_in[index_in + j]) {
        j++;
      }
      if (index_in - i <= 255) {
        // comparer à la meilleur séquence "proche"
        if (j > best_near_length) {
          best_near_offset = index_in - i;
          best_near_length = j;
        }
      } else {
        // comparer à la meilleur séquence "lointaine"
        if (j > best_far_length) {
          best_far_offset = i;
          best_far_length = j;
        }
      }
    }

    if (best_near_length >= 2 && best_near_length >= best_far_length) {
      // pointeur proche
      (*buffer_out)[index_out++] = best_near_length;
      (*buffer_out)[index_out++] = NEAR_POINTER_BYTE;
      (*buffer_out)[index_out++] = best_near_offset;
      index_in += best_near_length;
    } else if (best_far_length >= 2) {
      // pointeur lointain
      (*buffer_out)[index_out++] = best_far_length;
      (*buffer_out)[index_out++] = FAR_POINTER_BYTE;
      memcpy((*buffer_out) + index_out, &best_far_offset, 2);
      index_out += 2;
      index_in += best_far_length;
    } else {
      // mot copié directement
      switch (words_in[index_in] >> 8) {
        case NEAR_POINTER_BYTE:
        case FAR_POINTER_BYTE:
          // cas spécial si le mot a 0xA7 ou 0xA8 en second octet
          (*buffer_out)[index_out++] = 0;
          (*buffer_out)[index_out++] = words_in[index_in] >> 8;
          (*buffer_out)[index_out++] = words_in[index_in] & 0xFF;
          break;
        default:
          (*buffer_out)[index_out++] = words_in[index_in] & 0xFF;
          (*buffer_out)[index_out++] = words_in[index_in] >> 8;
          break;
      }
      index_in++;
    }
  }
  return index_out;
}

int main(int argc, char **argv) {
  int fd_in = open(argv[1], O_RDONLY);
  int fd_out = open(argv[2], O_CREAT | O_WRONLY, 0644);

  // obtenir la taille du fichier en entrée
  struct stat st;
  fstat(fd_in, &st);
  off_t input_size = st.st_size;
  printf("Input size: %lld\n", input_size);

  // allocation des buffers
  unsigned char *buffer_in = malloc(input_size);
  read(fd_in, buffer_in, input_size);
  unsigned char
      *buffer_out;  // le buffer sera alloué par la fonction carmack_encode

  // compression de l'entrée dans un buffer, et écriture dans le fichier en
  // sortie
  size_t out_length = carmack_encode(buffer_in, &buffer_out, input_size);
  printf("Output size: %ld\n", out_length);
  write(fd_out, buffer_out, out_length);

  // nettoyage (pas nécessaire car fin du processus)
  free(buffer_in);
  free(buffer_out);
  close(fd_in);
  close(fd_out);
  return 0;
}
