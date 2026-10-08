#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

typedef struct {
    pid_t ancestros[100]; // PIDs padre a padre
    pid_t hojas[100];     // PID hijos
} DatosCompartidos;

int main(int argc, char *argv[]) {

    //Comprobar que los argumentos esten bien
    if (argc != 3) {
        printf("Faltan o sobran argumentos\n");
        return 1;
    }

    int x = atoi(argv[1]);
    int y = atoi(argv[2]);

    if (x <= 0 || y <= 0) {
        printf("Alguno de los argumentos son números negativos");
        return 1;
    }

    //obtener segmento de memoria compartida
    int shmid = shmget(IPC_PRIVATE, sizeof(DatosCompartidos), IPC_CREAT | 0666);//IPC_PRIVATE para que los procesos y sus descendientes puedna entrar
    if (shmid == -1) {
        perror("Error shmget");
        exit(1);
    }

    //vincular segmento de memoria compartida
    DatosCompartidos *shm = (DatosCompartidos *) shmat(shmid, NULL, 0);//Creamos un puntero con el formato DatosCompartidos y lo vinculamos al programa NULL sirve para que el programa encuentre automaticamente un sitio él mismo y el 0 es el comportamiento por defecto lectura y escritura
    if (shm == (void *) -1) {
        perror("Error shmat");
        exit(1);
    }

    // Construcción de la cadena vertical (X niveles)
    for (int i = 0; i < x; i++) {
        pid_t pid = fork();

        if (pid < 0) {
            perror("Error fork");
            exit(1);
        } else if (pid == 0) {
            // Proceso hijo en la cadena vertical
            shm->ancestros[i] = getpid();

            if (i == x - 1) {
                // Último proceso de la cadena vertical (nivel X): crea las Y hojas
                for (int j = 0; j < y; j++) {
                    pid_t pid_hoja = fork();

                    if (pid_hoja < 0) {
                        perror("Error fork en subhijo");
                        exit(1);
                    } else if (pid_hoja == 0) {
                        // Proceso Hoja
                        shm->hojas[j] = getpid();

                        // Imprimir mensaje de la hoja
                        printf("Soy el subhijo %d, mi padres son: ", getpid());
                        for (int k = 0; k < x; k++) {
                            printf("%d%s", shm->ancestros[k], (k == x - 1) ? "\n" : ", ");// %d es un número decimal(es el pid) en cambio %s es un String que va dependiendo si k==x-1 entonces imprime un \n,si no pones una coma
                        }

                        // Desvincular y salir
                        shmdt(shm);
                        exit(0);
                    }
                }

                // El último proceso vertical espera a que terminen sus Y subhijos
                for (int j = 0; j < y; j++) {
                    wait(NULL);
                }

                shmdt(shm);
                exit(0);
            }
        } else {
            // El proceso padre espera a que su hijo directo termine
            wait(NULL);

            if (i > 0) {
                // Los nodos intermedios de la cadena se desvinculan y terminan
                shmdt(shm);
                exit(0);
            }

            // El superpadre (i == 0) sale del bucle de creación para imprimir los resultados finales
            break;
        }
    }

    // El proceso superpadre muestra la recolección de los subhijos finales
    printf("Soy el superpadre (%d): mis hijos finales son: ", getpid());
    for (int j = 0; j < y; j++) {
        printf("%d%s", shm->hojas[j], (j == y - 1) ? "\n" : ", ");
    }

    // 3. Desvincular y eliminar la memoria compartida
    shmdt(shm);
    /**
     * //La opción IPC_RMID es para borrar memoria compartida NULL sirve porque la sintaxis 
     * requiere que entreguemos más argumento pero como nuestra única intención es eliminarlo no lo necesitamos
     */
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}