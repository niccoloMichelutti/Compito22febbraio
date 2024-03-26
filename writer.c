#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BLOCK_DIM 1024 /**< Dimensione del blocco per la lettura/scrittura. */

/**
 * @brief Funzione principale del programma.
 * @param argc Il numero di argomenti passati da linea di comando.
 * @param argv Un array di stringhe contenente gli argomenti passati da linea di comando.
 * @return 1 se il programma è stato eseguito correttamente, 0 altrimenti.
 */
int main(int argc, char* argv[])
{
    FILE* file;
    unsigned char buffer[BLOCK_DIM];
    int n;
    int fifo;
 
    if (argc != 2)
    {
        printf("Uso: %s file\r\n", argv[0]);
        return 0;
    }

    // Apertura della FIFO in modalità di sola lettura
    fifo = open("my_fifo", O_RDONLY);
    if (fifo < 0)
    {
        printf("Errore apertura FIFO\r\n");
        return 0;
    }

    // Apertura del file di destinazione in modalità binaria di scrittura
    file = fopen(argv[1], "wb");
    if (file == NULL)
    {
        printf("Errore apertura file \"%s\"\r\n", argv[1]);
        close(fifo);
        return 0;
    }

    // Lettura dalla FIFO e scrittura nel file di destinazione
    while ((n = read(fifo, buffer, sizeof(buffer))) > 0)
        fwrite(buffer, 1, n, file);

    // Chiusura della FIFO e del file di destinazione
    close(fifo);
    fclose(file);
    
    return 1;
}
