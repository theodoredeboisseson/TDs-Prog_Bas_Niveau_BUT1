# TD n°3 - Entrées / Sorties
 
Dans ce TD, nous allons utiliser les fonctions `open`, `close`, `read`, `write`, et `lseek` pour manipuler les fichiers 
directement en utilisant les appels système. Il ne faut donc pas utiliser les fonctions plus haut niveau telles que 
`fopen`, `fgetc`, `fputc`, `printf`, etc.

Si nécessaire, vous pouvez vous référer aux transparents du second cours.


## Lecture et copie

1. Cherchez à l'aide de la commande `man` les bibliothèques à inclure pour utiliser les appels système 
`open`, `close`, `read`, `write` et `lseek`.
	
	**Remarque :** Pour indiquer que l'on veut la documentation des appels système, il faut utiliser l'option
	`2` dans `man` (par exemple « `man 2 read` »).

1. Écrivez le programme `readfile.c` qui ouvre le fichier `message.txt` du répertoire courant et affiche son contenu 
à l'écran.
	
	**Indication :** Pour détecter que l'on arrive à la fin du fichier il faut regarder le résultat renvoyé par 
	`read`, qui correspond au nombre de caractères lus.
	
	**Rappel :** les descripteurs de fichier 0, 1 et 2 correspondent respectivement à l'entrée standard, la sortie 
	standard et la sortie d'erreur. Vous pouvez donc par exemple utiliser la fonction `write` en lui passant le 
	descripteur 1 pour écrire du texte à l'écran.

1. Écrivez le programme `copy.c` qui copie le contenu du fichier `message.txt` dans un fichier `copie.txt`.
	
	**Indication :** Il faut ouvrir le premier fichier en lecture et le second en écriture (en le créant si nécessaire)
	et copier le contenu du premier dans le second.
	
	**Attention :** Si le fichier de destination n'existe pas, il faut le créer en lui donnant des permissions. Ces 
	permissions sont données à la fonction `open` en quatrième argument. Vous pouvez utiliser la notation en octal 
	comme pour la fonction `chmod` (par exemple 0644) ou utiliser des *flags*, dont vous trouverez le détail dans la 
	page de manuel de `open`.

## Lecture d'une ligne

On veut maintenant écrire une fonction `int lireligne(int fd, char *s, int size)` pour lire une ligne d'un 
fichier :

- la fonction lit des caractères dans le fichier correspondant au descripteur `fd` jusqu'à lire un retour à la ligne 
(`\n`) ou avoir lu `size` caractères ;
- elle place les octets lus dans le tableau `s` passé en argument (on suppose que le tableau `s` peut contenir `size` 
caractères) ;
- le résultat renvoyé par la fonction est le nombre de caractères effectivement lus dans le fichier, ou -1 en cas d'erreur.

1. En utilisant les appels système, écrivez la fonction `lireligne` dans le fichier `lireligne.c` en lui faisant lire
    les caractères un par un afin de pouvoir s'arrêter facilement si elle rencontre un retour à la ligne.
	
	**Indication :** Pour lire les caractères un par un, vous pouvez déclarer un tableau d'un caractère `char c[1]`
	ou bien déclarer un caractère `char c` et passer l'adresse du caractère `&c` à la fonction `read`.

1. Vérifiez le bon fonctionnement de `lireligne` en lui faisant lire des lignes du fichier `message.txt`. Essayez 
    de lire à la fois des lignes plus courtes et des lignes plus longues que la valeur indiquée.

Chaque appel à l'une des commandes `open`, `close`, `read`, `write` et `lseek` provoque un appel système, qui est 
très coûteux en nombre d'opérations effectuées puisqu'il faut sauvegarder l'état du processus, donner le contrôle au 
système pour effectuer l'action puis recharger l'état du processus pour reprendre le fonctionnement.

Il est donc préférable de réduire autant que possible le nombre d'appels systèmes effectués. En particulier, il est 
très inefficace de lire le contenu d'un fichier en faisant un appel à `read` pour chaque caractère. Pour lire un 
fichier, on créé donc un tableau pouvant contenir des caractères (un tampon ou *buffer*) et on lit les octets en 
remplissant le tampon à chaque lecture (tant qu'on n'est pas au bout du fichier).

Si la taille du tampon est trop petite, on va effectuer beaucoup d'appels système. Si elle est trop grande, on va 
utiliser de la mémoire inutilement. La taille optimale est difficile à déterminer. Les documentations *Unix* 
conseillent d'utiliser un tampon de 4096 octets.

Dans le cas de la fonction `lireligne`, on peut utiliser le tableau passé en argument comme tampon, et donc lire 
`size` caractères à la fois.

Si les caractères lus contiennent un retour à la ligne, la fonction renvoie le nombre de caractères lus jusqu'au 
retour à la ligne. Le problème est que pour remplir le tampon, la fonction `read` a avancé le curseur de lecture 
dans le fichier. Si l'on trouve un retour à la ligne dans les octets lus, il faut donc replacer le curseur juste 
après ce retour à la ligne pour ne pas perturber les lectures suivantes dans le fichier (qui reprendront donc au 
début de la ligne suivante). On utilise pour cela la fonction `lseek`.

3. Modifiez la fonction `lireligne` pour qu'elle lise les caractères du fichier par blocs de `size` caractères à 
    la fois.
	
	**Indication :** La fonction doit donc utiliser `read` pour remplir le tableau `s`, puis parcourir le tableau 
	afin de localiser le premier retour à la ligne. Quand un retour à la ligne est trouvé, il faut remettre le curseur 
	de lecture du fichier au bon emplacement. La fonction doit également renvoyer le nombre d'octets lus jusqu'au 
	retour à la ligne.

## Passage de paramètres

Jusqu'ici, tous les programmes que nous avons écrits avaient une fonction `main` ne prenant aucun argument : 
`int main()`. Il est toutefois possible de lui passer deux arguments, un entier et un tableau de chaînes de 
caractères : `int main(int argc, char **argv)`[^1]. Dans ce cas, si l'exécutable est
appelé depuis un terminal avec des arguments, l'entier `argc` correspond au nombre d'arguments, et le tableau `argv` 
contient tous les arguments sous forme de chaînes de caractères.

Le nom de l'exécutable qui a été appelé est toujours considéré comme le premier argument (donc il y a toujours au 
moins un argument). Si par exemple on a créé un exécutable `prog` et qu'on l'exécute avec la commande
```bash
$ ./prog -l toto
```
alors `argc` vaut 3 et le tableau `argv` contient les chaînes de caractères « `prog` », « `-l` » et « `toto` » aux 
indices 0, 1 et 2 respectivement.

1. Modifiez les programmes `readfile.c` et `copy.c` pour qu'ils prennent en paramètres les noms des fichiers sur 
    lesquels ils travaillent (le premier attend un nom de fichier, le second en attend deux). Faites en sorte que 
    le programme s'interrompe en affichant un message d'erreur si le nombre d'arguments reçus ne correspond pas au 
    nombre attendu.
	
	**Indication :** Pour interrompre l'exécution d'un programme, vous pouvez utiliser la commande « `exit(1);` » de 
	la bibliothèque `<stdlib.h>`.

## Gestion des erreurs (`errno` et `perror`)

Lorsqu'une erreur se produit pendant l'exécution d'un appel système, la fonction appelée renvoie en général le 
résultat -1. Pour connaître la nature de l'erreur qui s'est produite, il faut regarder la valeur de la variable 
`errno`.

1. Regardez rapidement la page de documentation correspondant à `errno`. Quelle bibliothèque faut-il ajouter à 
    votre programme si vous voulez utiliser cette variable ?

Lorsqu'une erreur se produit, la fonction qui a généré l'erreur donne une valeur à la variable `errno` puis 
renvoie un code d'erreur (dans le cas des appels systèmes c'est -1). La documentation des fonctions utilisant 
ce mécanisme donne en général la liste des erreurs pouvant se produire.

2. Regardez la documentation de l'appel système `open` et cherchez la section donnant la liste des erreurs.

Chaque erreur correspond à un nom, qui est en réalité un mot clé associé à un entier (les valeurs associées à chaque 
mot-clé sont définies dans les bibliothèques mais il n'est pas nécessaire de les connaître puisqu'on doit justement 
utiliser les mots-clés).

On trouve par exemple dans la documentation de `open` (éventuellement en français sur votre machine) :
```
       ENOENT O_CREAT is not set  and  the  named  file  does  not
              exist.   Or,  a directory component in pathname does
              not exist or is a dangling symbolic link.
```
qui indique que lorsqu'on essaie d'ouvrir un fichier qui n'existe pas sans donner l'option `O_CREAT` (qui force la 
création des fichiers inexistants), l'erreur `ENOENT` est placée dans la variable `errno` et la fonction `open` 
renvoie -1.

Pour connaître l'erreur qui s'est produite, on peut alors tester l'égalité de `errno` et de chacune des erreurs 
possibles : « `if (errno == ENOENT)` ».

3. Modifiez le programme `readfile.c` pour qu'il affiche un message d'erreur spécifique lorsque le fichier à 
    ouvrir n'existe pas.
	
	**Indication :** Il faut tester si le résultat de `open` est -1, et si c'est le cas, regarder si `errno` est égal 
	à `ENOENT`.

Cette façon de faire est cependant très pénible à réaliser (dans la question précédente, on ne tient compte que d'une 
seule erreur possible, mais il faudrait toutes les faire). Fort heureusement, il existe une fonction permettant de 
décrire automatiquement l'erreur qui s'est produite : `perror`.

La fonction `perror` prend un argument qui est une chaîne de caractères et affiche cette chaîne de caractères suivie 
d'une description en langage courant de l'erreur qui se trouve dans la variable `errno`. Ainsi, si l'on appelle 
« `perror(argv[1]);` », le programme affichera
```bash
$ ./a.out toto.txt
toto.txt: No such file or directory
```
si le fichier `toto.txt` n'existe pas.

4. Modifiez à nouveau le programme `readfile.c` pour qu'en cas d'échec d'ouverture du fichier il affiche une 
description de l'erreur produite et s'interrompe.

1. Modifiez tous les programmes écrits dans ce TD pour gérer les éventuelles erreurs après les appels systèmes.
	
De manière générale, il est très important de toujours vérifier qu'il n'y a pas eu d'erreur après un appel à `open`. 
Pour les autres il est toujours conseillé de surveiller les erreurs éventuelles mais elles se produisent moins 
fréquemment.

## Informations sur les fichiers

La fonction `stat` correspond à un appel système permettant d'obtenir des informations concernant l'inode d'un 
fichier dont le nom est passé en paramètre. Son prototype est :
```
SYNOPSIS
       #include <sys/types.h>
       #include <sys/stat.h>
       #include <unistd.h>

       int stat(const char *path, struct stat *buf);
```
Les informations sur le fichier sont placées dans la structure `buf` passée en argument. Les champs de la structure 
sont décrits dans la page de manuel.

Par exemple, pour obtenir le numéro du propriétaire du fichier `blop.txt`, on peut utiliser les instructions 
suivantes :
```cpp
struct stat buf;
uid_t proprietaire;
if (stat("blop.txt", &buf) == 0) {
	proprietaire = buf.st_uid;
} else {
	perror("blop.txt");
}
```

1. Écrivez le programme `bigfich.c` qui prend en argument une liste de noms de fichiers et affiche le nom du plus 
    grand d'entre eux.
	
	Le nombre de noms de fichiers passé en argument doit être au moins 1 mais peut être arbitrairement grand. 
	Affichez un message d'erreur si aucun fichier n'est passé en argument, ainsi qu'un message d'erreur pour chaque 
	nom de fichier pour lequel on ne peut pas obtenir d'informations à l'aide de la fonction `stat`.

## (Bonus) Lecture avec un *buffer*
	
Le but de cet exercice est de réécrire la fonction `getc` (que nous nommerons `mygetc`) de deux façons différentes 
afin de comparer les performances des deux versions lorsque l'on effectue un grand nombre d'appels à `mygetc`.

La spécification de `mygetc` est la suivante : `int mygetc(int fd)` retourne le prochain caractère du fichier désigné 
par le descripteur `fd` sous forme d'entier (c'est à dire retourne son code ASCII), ou retourne -1 en cas de fin du 
fichier ou de problème à la lecture.

1. Écrivez la fonction `mygetc` du fichier `mygetc.c` de manière naïve en utilisant l'instruction `read` pour lire un
    caractère.

On veut maintenant réécrire cette fonction pour qu'elle fasse moins d'appels à `read` si on l'utilise plusieurs fois 
de suite.

2. Définissez une constante `BUF_SIZE` (valant par exemple 5), puis écrivez la fonction `mygetc2` ayant la même 
    spécification que `mygetc` de telle sorte qu'elle n'appelle la fonction `read` qu'une fois si on appelle `BUF_SIZE` 
    fois de suite la fonction `mygetc2`.

    **Indications :**
    - La première fois, l'appel à `read` doit lire `BUF_SIZE` octets et les placer dans un tableau qui ne doit pas 
    être perdu entre deux appels à `mygetc2`. Vous pouvez utiliser le mot-clé `static` lors de la déclaration d'une 
    variable locale dans une fonction. Cherchez la documentation en ligne.
	- Il faut également utiliser des variables permettant de savoir quels caractères de ce *buffer* ont déjà été lus, 
	afin de pouvoir adopter la stratégie suivante :
	    - si il n'y a plus de nouveau caractère à lire dans le *buffer*, refaire un `read` pour le remplir ;
     	- sinon, retourner simplement le prochain caractère de votre *buffer* sans faire appel à `read`.

1. Comparez l'efficacité de `mygetc` et `mygetc2` de la façon suivante :
	- Créez un fichier texte de plusieurs Mo.
	- Dans la fonction `main`, ouvrez ce fichier, et lisez-le caractère par caractère en utilisant `mygetc`.
	- Chronométrez le temps d'exécution, puis recommencez en remplaçant `mygetc` par `mygetc2`. Faites varier la 
	valeur de la constante `BUF_SIZE` en utilisant les valeurs 5, 10 puis 1024. Que constatez vous ?
	
---
Notes :

[^1]: On peut également utiliser les déclarations `int main(int argc, char argv[][])` ou même 
`int main(int argc, char *argv[])` qui sont équivalentes.
