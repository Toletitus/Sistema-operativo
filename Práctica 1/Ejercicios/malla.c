#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

void Generarhorizontal(int x, int y, pid_t pidpadre) {
    // 1. El proceso principal (malla) crea 'y' columnas
    for(size_t i = 0; i < y; i++){
        switch(fork()){
            case -1:
                perror("Hubo un error al crear la columna");
                exit(1);
            case 0:
                // hijos de forma vertical
                for(size_t j = 0; j < x - 1; j++){
                    switch(fork()){
                        case -1:
                            perror("Ha ocurrido un error en el fork vertical");
                            exit(1);
                        case 0:
                            // Soy el nuevo hijo (eslabón inferior). 
                            // Continúo el bucle para crear a mi propio hijo.
                            continue;
                        default:
                            // Soy el padre en esta rama vertical.
                            // Me detengo a esperar que mi único hijo muera para no dejar zombis.
                            wait(NULL); 
                            exit(0);
                    }
                }
                
                if(i == y - 1) {
                    // El último proceso de la última columna ejecuta pstree
                    char str_padre[16];
                    snprintf(str_padre, sizeof(str_padre), "%d", pidpadre);
                    execlp("pstree", "pstree", "-c", str_padre, NULL);
                } else {
                    // Mantener vivo a los otros
                    sleep(2);
                    exit(0);
                }
                break;
            default:
                break;
        }
    }

    //El proceso principal (malla) espera a que mueran todos sus hijos
    for(size_t i = 0; i < y; i++){
        wait(NULL);
    }
}

int main(int argc, char const *argv[])
{
    pid_t pidpadre = getpid();
    
    if(argc == 3){
        Generarhorizontal(atoi(argv[1]), atoi(argv[2]), pidpadre);
    } else {
        printf("Faltan o sobran argumentos\n");
    }
    
    return 0;
}