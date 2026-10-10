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

//Comprobar que los argumentos esten bien (devuelve 1 si hay error)
int validar_argumentos(int argc, char *argv[], int *x, int *y) {
    if (argc != 3) {
        printf("Faltan o sobran argumentos\n");
        return 1;
    }

    *x = atoi(argv[1]);
    *y = atoi(argv[2]);

    if (*x <= 0 || *y <= 0) {
        printf("Alguno de los argumentos son números negativos");
        return 1;
    }
    return 0;
}

//obtener y vincular segmento de memoria compartida
DatosCompartidos *crear_memoria(int *shmid) {
    //obtener segmento de memoria compartida
    *shmid = shmget(IPC_PRIVATE, sizeof(DatosCompartidos), IPC_CREAT | 0666);//IPC_PRIVATE para que los procesos y sus descendientes puedna entrar
    if (*shmid == -1) {
        perror("Error shmget");
        exit(1);
    }

    //vincular segmento de memoria compartida
    DatosCompartidos *shm = (DatosCompartidos *) shmat(*shmid, NULL, 0);//Creamos un puntero con el formato DatosCompartidos y lo vinculamos al programa NULL sirve para que el programa encuentre automaticamente un sitio él mismo y el 0 es el comportamiento por defecto lectura y escritura
    if (shm == (void *) -1) {
        perror("Error shmat");
        exit(1);
    }
    return shm;
}

// Proceso Hoja
void proceso_hoja(DatosCompartidos *shm, int j, int x) {
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

// Último proceso de la cadena vertical (nivel X): crea las Y hojas
void ultimo_nivel(DatosCompartidos *shm, int x, int y) {
    for (int j = 0; j < y; j++) {
        pid_t pid_hoja = fork();

        if (pid_hoja < 0) {
            perror("Error fork en subhijo");
            exit(1);
        } else if (pid_hoja == 0) {
            proceso_hoja(shm, j, x);
        }
    }

    // El último proceso vertical espera a que terminen sus Y subhijos
    for (int j = 0; j < y; j++) {
        wait(NULL);
    }

    shmdt(shm);
    exit(0);
}

// Construcción de la cadena vertical (X niveles)
// Solo vuelve el superpadre (i == 0); el resto de procesos terminan dentro
void cadena_vertical(DatosCompartidos *shm, int x, int y) {
    for (int i = 0; i < x; i++) {
        pid_t pid = fork();

        if (pid < 0) {
            perror("Error fork");
            exit(1);
        } else if (pid == 0) {
            // Proceso hijo en la cadena vertical
            shm->ancestros[i] = getpid();

            if (i == x - 1) {
                ultimo_nivel(shm, x, y);
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
}

// El proceso superpadre muestra la recolección de los subhijos finales
void mostrar_resultado(DatosCompartidos *shm, int y, int shmid) {
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
}

int main(int argc, char *argv[]) {
    int x, y, shmid;

    if (validar_argumentos(argc, argv, &x, &y)) {
        return 1;
    }
    DatosCompartidos *shm = crear_memoria(&shmid);
    cadena_vertical(shm, x, y);
    mostrar_resultado(shm, y, shmid);

    return 0;
}