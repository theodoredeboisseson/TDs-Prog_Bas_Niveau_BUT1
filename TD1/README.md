# TD n°1 - Rappels de C, entrée / sortie

## Entrée standard, sortie standard et sortie d'erreur

La plupart des commandes _Unix_ affichent des messages à l'utilisateur (résultat de la commande ou message d'information) et certaines peuvent attendre des données de l'utilisateur. Ces messages sont affichés dans le terminal et sont gérés par trois flux de données :

-   l'entrée standard (_standard input_ ou _stdin_) : c'est le flux de données par lequel le programme reçoit des données de l'utilisateur. Par défaut, l'entrée standard est associée au clavier.
-   la sortie standard (_standard output_ ou _stdout_) : c'est le flux de données par lequel le programme envoie des données à l'utilisateur. Par défaut, les données envoyées dans la sortie standard sont affichées dans le terminal.
-   la sortie d'erreur (_standard error_ ou _stderr_) : c'est le flux de données par lequel le programme envoie des messages d'erreur à l'utilisateur. Par défaut, les données envoyées dans la sortie d'erreur sont également affichées dans le terminal.

1. Exécutez la commande `ls *.c blop.txt` dans le répertoire du TD. Parmi les lignes affichées dans le terminal, lesquelles sont affichées dans la sortie stantdard et lesquelles sont affichées dans la sortie d'erreur ?

2. Réexécutez la commande précédente mais en redirigeant la sortie standard dans un fichier `sortie.txt` et la sortie d'erreur dans un fichier `erreur.txt` (vous pouvez chercher sur internet comment faire des redirections dans le _shell_ si vous avez oublié). Vérifiez la réponse à la question précédente en regardant le contenu des fichiers `sortie.txt` et `erreur.txt`.

3. Exécutez la commande `grep toto`. Essayez ensuite d'entrer des lignes dans le terminal (certaines contenant le mot «&nbsp;toto&nbsp;» et d'autres non). Que se passe-t-il ? Pourquoi ? (pour quitter la commande vous pouvez utiliser le raccourci-clavier `Ctrl-D`)

En C, l'entrée standard et les sorties standard et erreur sont respectivement associées aux descripteurs de fichiers 0, 1 et 2. Ces descripteurs sont automatiquement ouverts lorsqu'un programme est lancé et sont par défaut associés au terminal depuis lequel le programme est exécuté. Ainsi, dans un programme on peut directement lire des octets avec `read` dans le descripteur 0 (le programme attend alors que l'utilisateur entre des données au clavier) et écrire des octets avec `write` dans les descripteurs 1 et 2 (qui seront affichés dans le terminal) sans avoir à utiliser `open`.

4. Écrivez un programme `mycat.c` qui lit des octets depuis l'entrée standard et les écrit dans la sortie standard. Le programme doit s'arrêter lorsque l'entrée standard est fermée (on détecte ça au fait que la fonction `read` renvoie 0).

5. Compilez le programme `mycat.c` et exécutez-le. Vérifiez que le programme fonctionne correctement en entrant des données au clavier (à chaque fois que vous appuyez sur `Entrée` le programme devrait répéter la ligne).

    Pour quitter le programme vous pouvez utiliser le raccourci-clavier `Ctrl-D` qui envoie un signal de fin de fichier à l'entrée standard (ce qui indique au programme qu'il n'y a plus rien à lire).

6. Vérifiez que votre programme fonctionne aussi avec des redirections : essayez de lire depuis le fichier README.md et d'écrire dans un fichier `sortie.txt`.

7. Modifiez le programme `mycat.c` pour qu'il lise les arguments qui lui sont passés depuis le shell. S'il reçoit au moins un argument il doit lire les données depuis le fichier dont le nom est passé en premier argument (sinon il lit depuis l'entrée standard). S'il reçoit au moins deux arguments, il doit écrire les données lues dans un fichier dont le nom est passé en deuxième argument (sinon il écrit dans la sortie standard).

    **Rappels :** Les arguments passés à un programme C sont accessibles par les paramètres passés à la fonction [`main`](https://www.gnu.org/software/c-intro-and-ref/manual/html_node/The-main-Function.html)

    **Indication :** Il y a plusieurs façons de faire, mais une façon simple est d'associer les descripteurs de fichiers 0 et 1 à des fichiers ouverts au lieu de l'entrée et sortie standard. Pour cela vous pouvez soit fermer les descripteurs 0 et 1 avant d'ouvrir les fichiers (`open` utilise toujours la première valeur disponible) soit utiliser la fonction [`dup2`](https://linux.die.net/man/2/dup2).

## Compression de Carmack

L'algorithme de compression de Carmack a été développé par [John Carmack](https://en.wikipedia.org/wiki/John_Carmack) pour les premiers jeux _id Software_ (notamment _Commander Keen_ et _Wolfenstein 3D_). Ce format de compression est fait pour être utilisé sur des fichiers d'au plus 64 ko, et dont les données sont principalement représentées par blocs de 2 octets que l'on appelle des _mots_.

L'idée est de détecter des motifs répétés dans les données à compresser et de décrire ces motifs à l'aide d'un pointeur vers leur précédente occurrence. Bien que cette idée soit similaire à d'autres algorithmes de compression (en particulier les algorithmes de [Lempel et Ziv](https://en.wikipedia.org/wiki/LZ77_and_LZ78) utilisés dans le format de compression _zip_), une particularité de l'algorithme de Carmack est d'utiliser deux types de pointeurs : des pointeurs «&nbsp;proches&nbsp;» et des pointeurs «&nbsp;lointains&nbsp;».

Les pointeurs « proches&nbsp;» indiquent une adresse relative à la position courante dans les données décompressées. La distance (en mots) est codée sur un octet, il n'est donc possible de faire référence qu'aux 255 derniers _mots_ décodés. Les pointeurs « lointains&nbsp;» indiquent une adresse absolue (en mots) comptée à partir du début des données décodées. La distance est codée sur deux octets, ce qui permet de faire référence à n'importe quel _mot_ décodé (puisque la taille totale des données est inférieure à 64 ko).

Les deux premiers octets du fichier compressé représentent la taille totale en octets des données à décompresser[^1]. Ensuite, pour décompresser les données, tant qu'il reste des données à décoder il faut :

-   lire deux octets _x_ et _y_
-   si _y ≠ `0xA7`_ et _y ≠ `0xA8`_, il ne s'agit pas d'un pointeur donc on recopie simplement le mot _xy_ dans les données décompressées
-   si _y = `0xA7`_, il s'agit d'un pointeur «&nbsp;proche&nbsp;». Il faut alors lire un octet supplémentaire _z_ qui représente la distance en mots (blocs de 2 octets) qu'il faut remonter dans les données décompressées pour trouver le début du motif à copier. Il faut alors copier _x_ mots depuis cette position dans les données décompressées.
-   si _y = `0xA8`_, il s'agit d'un pointeur «&nbsp;lointain&nbsp;». Il faut alors lire deux octets supplémentaires indiquant la position absolue en mots depuis le début des données décodées d'où commence le motif à copier. Il faut alors recopier _x_ mots à partir de cette position dans les données décodées.

Une dernière subtilité est que comme les octets `0xA7` et `0xA8` sont utilisés pour indiquer des pointeurs, on utilise un cas particulier pour représenter directement un mot qui aurait une de ces valeurs comme deuxième octet. Ainsi, dans le décodage, lorsqu'on lit deux octets _x_ et _y_, si _x = 0_ et _y = `0xA7`_ ou _y = `0xA8`_, on lit un octet supplémentaire _z_ et on recopie alors le mot _zy_ dans les données décompressées.

### Fonction de décompression

On va ici écrire la fonction `size_t carmack_decode(unsigned char *buffer_in, unsigned char *buffer_out)` du fichier `decode.c` qui décode des données compressées à l'aide de l'algorithme de Carmack. La fonction reçoit deux arguments :

-   `buffer_in` : Un pointeur vers le début des données compressées. Les données compressées commencent par deux octets représentant la taille totale des données à décompresser, suivis des octets à décompresser.
-   `buffer_out` : Un pointeur indiquant où écrire les données décompressées.

La fonction renvoie la taille des données décompressées (nombre d'octets écrits dans `buffer_out`).

1. Écrivez le code permettant de lire les deux premiers octets de `buffer_in` sous la forme d'un entier sur 16 bits (vous pouvez utiliser le type `uint16_t` de la librairie `<stdint.h>`). Stockez cette valeur dans une variable `output_size`, et ajoutez une instruction à la fin de la fonction pour renvoyer cette valeur.

2. Écrivez la structure de la boucle principale de la fonction sans écrire le code spécifique à chacun des sous-cas rencontrés. Cette boucle doit s'exécuter tant que la taille des données décompressées est strictement inférieure à `output_size`. À chaque itération, il faut regarder les deux prochains octets _x_ et _y_ de `buffer_in` :

    - si _x = `0xA7`_ il faudra traîter le cas d'un pointeur «&nbsp;proche&nbsp;» (à compléter plus tard)
    - si _x = `0xA8`_ il faudra traîter le cas d'un pointeur «&nbsp;lointain&nbsp;» (à compléter plus tard)
    - sinon il faut écrire les deux octets _x_ et _y_ dans `buffer_out`

3. Complétez le code du dernier cas de la boucle principale (recopie d'un mot de `buffer_in` dans `buffer_out`).

4. Complétez le code de la boucle principale correspondant à un pointeur «&nbsp;proche&nbsp;». Ici il faut regarder un octet supplémentaire _z_ et recopier _2\*x_ octets (_x_ mots) de `buffer_in` dans `buffer_out` en partant de 2\*_z_ octets avant la position courante dans `buffer_in`.

    Pour copier les _2\*x_ octets, vous pouvez soit écrire une boucle soit utiliser la fonction [`memcpy`](https://linux.die.net/man/3/memcpy) de la librairie `<string.h>`.

5. Complétez le code de la boucle principale correspondant à un pointeur «&nbsp;lointain&nbsp;».Cette fois-ci il faut regarder deux octets supplémentaires _w_ et recopier _2\*x_ octets de `buffer_in` dans `buffer_out` en partant de la position _2\*w_ dans `buffer_in`.

6. Ajoutez le code nécessaire pour traiter les exceptions où _x = 0_ et _y = `0xA7`_ ou _y = `0xA8`_. Dans ce cas il faut lire un octet supplémentaire _z_ et recopier le mot _zy_ dans `buffer_out`.

La fonction `carmack_decode` devrait maintenant être complète et prête à être utilisée dans la fonction `main` du programme `decode.c`.

### La fonction `main`

Nous allons maintenant écrire le corps du programme `decode.c` qui appellera la fonction `carmack_decode` pour décompresser des données. On veut dans un premier temps que le programme lise les données compressées dans son entrée standard et écrive les données décompressées dans sa sortie standard.

1. Dans la fonction `main` du fichier `decode.c`, allouez deux _buffers_ `buffer_in` et `buffer_out` de 98304 octets et 65536 octets respectivement (ajoutez également en fin de fonction les instructions pour libérer ces _buffers_).

    Ces tailles permettent de gérer les cas maximaux de taille des données décompressées (max. 64 ko d'après les spécifications de l'algorithme) et des données compressées où le pire cas est celui où tous les mots du fichier en entrée ont `0xA7` ou `0xA8` comme deuxième octet et sont représentés par 3 octets dans le fichier compressé.

2. Écrivez le code permettant de lire toutes les données de l'entrée standard dans `buffer_in`. Pour cela vous ferez une boucle qui utilise la fonction [`read`](https://linux.die.net/man/2/read) en lisant des blocs de 4096 octets à la fois et s'arrête lorsqu'il n'y a plus rien à lire dans l'entrée standard.

3. Appelez la fonction `carmack_decode` pour qu'elle décompresse les données lues dans `buffer_in` et écrive les données décompressées dans `buffer_out`.

4. Écrivez le code permettant d'écrire les données décompressées dans la sortie standard. Pour cela vous ferez une boucle qui utilise la fonction [`write`](https://linux.die.net/man/2/write) jusqu'à ce que toutes les données aient été écrites dans _stdout_.

    Attention, vous ne devez écrire que les octets de `buffer_out` qui correspondent à des données décompressées.

5. Compilez le programme `decode.c`. S'il compile sans erreur ni _warning_, exécutez-le pour décoder les fichiers `image.car` (qui devrait se décoder en une image au format `.bmp`) et `texte.car` (qui se décode en un fichier `.txt`) fournis dans le répertoire du TD.

    Étant donné que le programme attend son _input_ sur l'entrée standard et renvoie son _output_ sur la sortie standard, vous pouvez utiliser les redirections du _shell_ pour lire et écrire dans des fichiers.

6. Modifiez votre programme pour qu'il lise les arguments qui lui sont passés depuis le shell. S'il reçoit au moins un argument il doit lire les données compressées depuis le fichier dont le nom est passé en premier argument (sinon il lit depuis l'entrée standard). S'il reçoit au moins deux arguments, il doit écrire les données décompressées dans un fichier dont le nom est passé en deuxième argument (sinon il écrit dans la sortie standard).

---

Notes :

[^1]: Dans tout le sujet, on suppose que les entiers représentés sur plusieurs octets (ici deux octets) sont représentés en _little-endian_, c'est-à-dire que les octets de poids faible apparaissent en premier. Par exemple, l'entier 259 = 1 x 256 + 3 est représenté par les octets `0x01`et `0x03` dans cet ordre.
