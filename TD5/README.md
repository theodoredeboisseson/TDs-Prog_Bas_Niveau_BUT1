# TD n°5 - Manipulation de processus

L'objectif de ce TD est de vous familiariser avec l'utilisation des appels système `fork` (création de processus) et la famille `exec` (exécution du code d'un exécutable).

## Exercice 1. Copie du processus courant
	
On considère le programme suivant (`ex1fork.c`) :
```cpp
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char **argv) {
    int i;
    printf("PID: %d (avant fork)\n", getpid());
    i = fork();
    if (i != 0) {
        printf("PID: %d, résultat du fork: %d\n",getpid(),i);
    } else {
        printf("PID: %d, résultat du fork: %d \n",getpid(),i);
    }
    printf("PID: %d (après fork)\n", getpid());
}
```

1. Compilez et exécutez le programme. Expliquez les résultats observés.

1. En utilisant la fonction `unsigned int sleep(unsigned int seconds)` de la bibliothèque `unistd.h`, faites en sorte que les lignes d'affichage du processus fils s'exécutent 5 secondes après celles du processus père.
	
	Que se passe-t-il lorsque l'on exécute le programme ?

1. Exécutez le programme de la question précédente puis lancez la commande `ps -l` avant que le fils ait fini de s'exécuter (vous avez 5 secondes). Que remarquez-vous ?
	
	**Indication :** Combien de processus sont en cours, quels sont leurs parents ?

On considère maintenant le programme suivant (`ex1tab.c`) :
```cpp
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
    int i, j, s, tabpid[3];
    printf("[avant fork] PID: %d\n", getpid());
    for(j = 0; j < 3; j++) {
        tabpid[j] = fork();
        if (tabpid[j] != 0 ) {
            printf("PID: %d, retour fork: %d \n",getpid(),tabpid[j]);
        } else {
            printf("PID: %d \n",getpid());
            exit(j);
        }
    }
    for(j = 0; j < 3; j++) {
        i= waitpid(tabpid[j], &s, 0);
        if (i > 0) {
            printf("terminé PID: %d\n", i);
        }
        sleep(1);
    }
}   
```

4. Que fait ce programme ?

1. Que récupère la variable `s` ? Modifiez le programme pour qu'il affiche le code de retour de chacun des processus fils.
	
	**Indication :** Il faut chercher comment trouver le code de retour d'un processus dans la documentation de `waitpid`.

1. Modifiez le programme pour que chacun des fils affiche les trois valeurs contenues dans le tableau `tabpid` avant de terminer. Observez ce qui se passe lors de l'exécution. Comment l'expliquez-vous ?

1. Reprenez le programme `negative.c` écrit au TD 4. Rajouttez un appel à `fork` juste après avoir recopié l'en-tête du fichier, puis faites en sorte que le processus père génère le négatif de l'image tandis que le fils recopie l'image sans la modifier. Observez le fichier obtenu. Comment expliquez-vous cela ?

1. Exécutez le même programme avec une image en couleur. Expliquez le résultat.

## Exercice 2. Exécution

1. En utilisant la fonction `execlp`, écrivez le programme `ex2psl.c` qui exécute la commande `ps -l` lorsqu'on le lance.
	
	**Indication :** Il faut appeler la fonction `execlp` pour remplacer le code du processus courant par celui de la commande `ps`, tout en lui donnant les bons arguments pour lui passer l'option `-l`.

1. Écrivez le programme `ex2lancer.c` qui crée un nouveau processus à l'aide de `fork` et dont le processus fils exécute l'exécutable passé en paramètres avec ses arguments. Par exemple l'appel `./a.out ls -l *` doit exécuter la commande `ls -l *`. Le père doit attendre la fin de l'exécution du fils avant de terminer.
	
	**Indication :** Il faut utiliser l'appel `fork` puis utiliser une des commandes de la famille `exec` pour remplacer le code du processus fils par celui du programme passé en argument, en lui transmettant également tous les autres arguments.

## Exercice 3. Tubes et redirections

L'appel système `pipe` permet de créer un *tube anonyme* (ou *pipe* en anglais). La commande renvoie deux descripteurs de fichiers, l'un permettant d'écrire des octets dans le tube (avec `write`) et l'autre permettant de lire les octets dans le tube (avec `read`).

L'intérêt principal d'un tube anomnyme est de permettre à deux processus apparentés d'échanger des données. En effet, si un tube est créé par un processus avant de faire un appel à `fork`, les deux processus résultants (le père et le fils) ont accès au tube par l'intermédiaire des descripteurs de fichiers.

1. Regardez la documentation de l'appel système `pipe` (`man 2 pipe`) pour comprendre comment il s'utilise (vous pouvez aussi chercher des exemples)

1. Regardez le programme `ex3pipe.c`. Que fait-il ?

On veut maintenant reproduire à l'aide des tubes anonymes le comportement du *shell* lorsqu'il reçoit deux instructions séparées par le caractère `|` (redirection de la sortie standard de la première commande vers l'entrée standard de la seconde commande). L'idée est de

- créer un tube ;
- lancer les deux processus qui vont exécuter chacune des commandes ;
- remplacer la sortie standard du premier processus par l'entrée du tube ;
- remplacer l'entrée standard du second processus par la sortie du tube ;
- utiliser la commande `exec` pour que chacun des deux processus exécute la commande souhaitée

(les fichiers ouverts et les descripteurs de fichiers sont conservés lors de l'appel à `exec`)

3. Quels sont les descripteurs de fichiers correspondant à l'entrée standard et la sortie standard d'un processus ?

1. Écrivez le programme `ex3tube.c` qui prend en argument :
    - le nom d'un exécutable
    - une liste d'options pour ce premier exécutable
    - la chaîne de caractères «`--pipe`»
    - le nom d'un second exécutable
    - les options du deuxième exécutable
    
    et lance deux processus fils, exécutant chacun des deux exécutables passés en argument avec leurs options respectives en redirigeant la sortie standard du premier vers l'entrée standard du second.
    
    Par exemple, la commande `./a.out ls -R --pipe grep toto` doit afficher toutes les lignes du résultat de la commande `ls -R` qui contiennent «`toto`» (ça doit faire la même chose que la commande `ls -R | grep toto`).
    
    **Remarques :**
    - Regardez la documentation des fonctions `dup` et `dup2`, qui permettent d'assigner une valeur spécifique de descripteur de fichier à un fichier ouvert.
    - Le processus père doit lancer deux fils. En utilisant les fonctions `wait` ou `waitpid` assurez-vous que le père termine après ses fils.
    - Les commandes qui travaillent sur l'entrée standard (comme par exemple `grep`) se terminent lorsque leur entrée est fermée. Si votre programme ne se termine pas correctement, vérifiez que votre programme ferme bien **tous** les descripteurs de fichiers qui correspondent à l'entrée du second processus.

1. Écrivez le programme `ex3redirections.c` qui prend en argument :
    - le nom d'un exécutable
    - une liste d'options pour cet exécutable
    - (optionnel) `--input` suivi d'un nom de fichier
    - (optionnel) `--output` suivi d'un nom de fichier
    
    et lance un processus fils qui exécute l'exécutable passé en argument avec les options correspondantes. Si le programme a reçu l'option `--input` il faut rediriger l'entrée du processus fils vers le fichier indiqué après l'option, et s'il a reçu l'option `--output` il faut rediriger sa sortie vers le fichier correspondant.
    
    Par exemple la commande `./a.out ls -R --output result.txt` doit créer un fichier `result.txt` contenant le résultat de `ls -R` (équivalent à `ls -R > result.txt` dans le shell) et la commande `./a.out grep toto --input fichier.txt` doit afficher toutes les lignes du fichier `fichier.txt` contenant «`toto`» (équivalent à `grep toto < fichier.txt` dans un shell).
    
    **Indications :** Il faut ouvrir les fichiers correspondants en lecture ou écriture et rediriger l'entrée ou la sortie du processus fils vers les descripteurs obtenus, comme à la question précédente.
