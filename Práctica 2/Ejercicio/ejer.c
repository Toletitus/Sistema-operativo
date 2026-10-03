#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>

// Variables globales
pid_t pidejec, pidA, pidB, pidX, pidY, pidZ;

// Z solo avisa a A
void alarma_Z(int sig){
    kill(pidA, SIGUSR1);
}

// A lanza pstree en un hijo auxiliar para no destruirse a sí mismo
void manejador_pstree_A(int sig) {
    char pid_str[16];
    snprintf(pid_str, sizeof(pid_str), "%d", pidA);
    if (fork() == 0) {
        execlp("pstree", "pstree", "-c", pid_str, NULL);
        exit(0);
    }
    wait(NULL);
}

void crearhijosxyz(){
    for (size_t i = 0; i < 3; i++) {
        pid_t hijo = fork(); // Guardamos el PID devuelto por fork
        
        if (hijo == -1) {
            perror("Error xyz");
        } else if (hijo == 0) {
            // --- CÓDIGO DE LOS HIJOS ---
            if (i == 0) {
                pause();
                exit(0);
            } else if (i == 1) {
                pause();
                exit(0);
            } else if (i == 2) {
                signal(SIGALRM, alarma_Z);
                alarm(5);
                pause();
                exit(0);
            }
        } else {
            // --- CÓDIGO DEL PADRE (B) ---
            // B guarda los PIDs reales para poder matarlos luego
            if (i == 0) pidX = hijo;
            if (i == 1) pidY = hijo;
            if (i == 2) pidZ = hijo;
        }
    }
    
    // B espera un poco para que dé tiempo a la alarma y al pstree
    sleep(7); 
    
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
    pidejec = getpid();
    
    pidA = fork();
    if(pidA == 0){
        pidA = getpid(); // A actualiza su PID en su memoria
        
        // A se prepara para recibir la señal de Z
        signal(SIGUSR1, manejador_pstree_A);
        
        pidB = fork();
        if(pidB == 0){
            pidB = getpid();
            crearhijosxyz();
            exit(0);
        }
        wait(NULL); // A espera a B
        exit(0);
    }
    wait(NULL); // ejec espera a A
    
    return 0;
}