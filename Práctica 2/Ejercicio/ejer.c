#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>

// Variables globales
pid_t pidejec, pidA, pidB, pidX, pidY, pidZ;
int segunditos;

void suicidio(int s){
    //Matar al proceso -> Una especie de sleep con kill
    printf("Soy el proceso " + getpid());
    printf(" y me muero \n");
    kill(getpid(),SIGKILL);

}

// Z solo avisa a A
void alarma_Z(int sig){
    kill(pidA, SIGUSR1);
}

// A lanza pstree en un hijo auxiliar para no destruirse a sí mismo
void manejador_pstree_A(int sig) {
    char pid_str[16];
    snprintf(pid_str, sizeof(pid_str), "%d", pidA);
    
    execlp("pstree", "pstree", "-c", pid_str, NULL);
    exit(0);
    
    
    
}

void crearhijosxyz(){
    for (size_t i = 0; i < 3; i++) {
        pid_t hijo = fork(); // Guardamos el PID devuelto por fork
        
        if (hijo == -1) {
            perror("Error xyz");
        } else if (hijo == 0) {
            // Hijo
            if (i == 0) {
                printf("Soy el proceso X: mid pid es %d,mi padre es %d,mi abuelico es %d,mi bisabuelo es %d \n",pidX,pidB,pidA,pidejec);
                pause();
                exit(0);
            } else if (i == 1) {
                printf("Soy el proceso Y: mid pid es %d,mi padre es %d,mi abuelico es %d,mi bisabuelo es %d \n",pidY,pidB,pidA,pidejec);
                pause();
                exit(0);
            } else if (i == 2) {
                printf("Soy el proceso Z: mid pid es %d,mi padre es %d,mi abuelico es %d,mi bisabuelo es %d \n",pidZ,pidB,pidA,pidejec);
                signal(SIGALRM, alarma_Z);
                alarm(segunditos);
                pause();
                exit(0);
            }
        } else {
            // El padre guarda los pid de los hijos para luego matarlos a todos
            if (i == 0) pidX = hijo;
            if (i == 1) pidY = hijo;
            if (i == 2) pidZ = hijo;
        }
    }
    
    // B espera a que le llegue una señal
    pause(); 
    
    // B mata a sus hijos
    kill(pidX, SIGKILL);
    kill(pidY, SIGKILL);
    kill(pidZ, SIGKILL);
    
    // B espera a que mueran los 3
    for(int j = 0; j < 3; j++) {
        wait(NULL);
    }
}

int main(int argc, char const *argv[]) {
    if (argc != 2)
    {
        printf("Upsiii,no has puesto un numerín \n Porfavor envía un numerín \n");
        return 1;
    }
    
        printf("Soy el proceso ejec. Mi pid es %d \n",getpid());
        segunditos = atoi(argv[1]);
        pidejec = getpid();
        
        pidA = fork();
        if(pidA == 0){
            pidA = getpid(); // A actualiza su PID en su memoria
            printf("Soy el proceso A y este es mi pid %d.Mi padre es %d \n",pidA,pidejec);
            // A se prepara para recibir la señal de Z
            signal(SIGUSR1, manejador_pstree_A);
            signal(SIGALRM,suicidio);
            
            pidB = fork();
            if(pidB == 0){
                pidB = getpid();
                printf("Soy el proceso B y este es mi pid %d,mi padre %d ,mi abuelico %d \n",pidB,pidA,pidejec);
                crearhijosxyz();
                wait(NULL); //B espera a sus hijos
                printf("Soy el proceso B (" + getpid());
                printf(" y me muero");
                exit(0);
            }
            wait(NULL); // A espera a B
            printf("Soy el proceso A y me muero");
            exit(0);
        }
        wait(NULL); // ejec espera a A
        
        return 0;
}