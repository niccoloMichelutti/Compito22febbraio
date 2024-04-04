/**
 * @file reader.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BLOCK_DIM 1024 /**< Dimensione del blocco per la lettura/scrittura. */
#define PROGRAM "writer" /**< Nome del programma da eseguire. */

/**
 * @brief Funzione per creare un nuovo processo utilizzando fork e exec
 * @param argument Un array di stringhe contenente gli argomenti del programma.
 * @return 0 se il processo è stato creato correttamente, -1 in caso di errore.
 */
int spawn(char program[], char* argument[]) {
    int pid;
 
    pid = fork();
    if (pid < 0)
        return -1;
    if (pid > 0)
        return 0;
    execv(program, argument);
    abort();
}

/**
 * @brief Funzione principale del programma.
 * @param argc Il numero di argomenti passati da linea di comando.
 * @param argv Un array di stringhe contenente gli argomenti passati da linea di comando.
 * @return 1 se il programma è stato eseguito correttamente, 0 altrimenti.
 */
int main(int argc, char* argv[]) {
    FILE* file;
    int fifo;
    unsigned char buffer[BLOCK_DIM];
    int n;
    char* arg[3];

    if (argc != 3) {
        printf("Uso: %s file-origine file-destinazione\r\n", argv[0]);
        return 0;
    }

    // Creazione degli argomenti per il nuovo processo
    arg[0] = (char*)malloc(strlen(PROGRAM) + 1);
    strcpy(arg[0], PROGRAM);
    arg[1] = (char*)malloc(strlen(argv[2]) + 1);
    strcpy(arg[1], argv[2]);
    arg[2] = NULL;

    // Creazione del nuovo processo
    if (spawn(PROGRAM, arg) < 0) {
        printf("Errore creazione processo\r\n");
        free(arg[0]);
        free(arg[1]);
        return 0;
    }

    // Apertura della FIFO
    fifo = open("my_fifo", O_WRONLY);
    if (fifo < 0) {
        printf("Errore apertura FIFO\r\n");
        free(arg[0]);
        free(arg[1]);
        return 0;
    }

    // Apertura del file di origine
    file = fopen(argv[1], "rb");
    if (file == NULL) {
        printf("Errore apertura file \"%s\"\r\n", argv[1]);
        close(fifo);
        free(arg[0]);
        free(arg[1]);
        return 0;
    }

    // Lettura e scrittura del file
    while ((n = fread(buffer, 1, sizeof(buffer), file)) > 0)
        write(fifo, buffer, n);

    fclose(file);
    close(fifo);
    free(arg[0]);
    free(arg[1]);
    return 1;
}
