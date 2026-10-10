#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>

// Variables globales
pid_t pidejec, pidA, pidB;
int segunditos;



/**
 * Está vacía para despertar
 */
void despertar(int s){

}
// Z solo avisa a A
void alarma_Z(int sig){
    kill(pidA, SIGUSR1);
}

// A lanza pstree en un hijo auxiliar para no destruirse a sí mismo
void manejador_pstree_A(int sig) {
    char pid_str[16];
    snprintf(pid_str, sizeof(pid_str), "%d", pidejec);
    if(fork() == 0){
        execlp("pstree", "pstree", "-c", pid_str, NULL);
        exit(0);
    }
    
    alarm(segunditos);
    
    
}



void CreacionXYZ(){
    pid_t hijos[3];
    pid_t hijo;
    for (size_t i = 0; i < 3; i++)
    {
        hijo = fork();
        switch (hijo)
        {
        case -1:
            perror("Upsiii,hubo un error \n");
            break;
        case 0:
            if (i == 0)
            {
                printf("Soy el proceso X(%d),mi padre es %d.Mi abuelo es %d,el de mi bisaabuelo es %d\n",getpid(),getppid(),pidA,pidejec);
            }else if(i==1){
                printf("Soy el proceso Y(%d),mi padre es %d.Mi abuelo es %d,el de mi bisaabuelo es %d\n",getpid(),getppid(),pidA,pidejec);
            }else if(i==2){
                printf("Soy el proceso Z(%d),mi padre es %d.Mi abuelo es %d,el de mi bisaabuelo es %d\n",getpid(),getppid(),pidA,pidejec);
                kill(pidejec,SIGUSR2);
            }
            
            pause();
            if (i == 0)
            {
                printf("Soy el proceso X(%d),y me muero\n",getpid());
            }else if(i==1){
                printf("Soy el proceso Y(%d),y me muero\n",getpid());
            }else if(i==2){
                printf("Soy el proceso Z(%d),y me muero\n",getpid());
            }

            
            exit(0);
            break;
            
        default:
            hijos[i] = hijo;
            continue;
            break;
        }
    }
    pause();
    for (size_t i = 0; i < 3; i++)
    {
        kill(hijos[i],SIGUSR1);
    }
    wait(NULL);
    printf("Soy el proceso B(%d) y me muero\n",getpid());
    exit(0);
}
int main(int argc, char const *argv[]) {
    //Validarargumentos

    segunditos = atoi(argv[1]);
    signal(SIGUSR1,despertar);
    signal(SIGALRM,alarma_Z);
    signal(SIGUSR2,manejador_pstree_A);
    pidejec = getpid();
    printf("Hola soy ejec y mi pid es %d\n",getpid());
    //Creación de A,Posteriormente B,Posteriormente XYZ
    pid_t pid = fork();
    if (pid != 0) {
        pidA = pid; // El proceso padre guarda correctamente el PID de A
    }
    if (pid == 0) //Esto sería A
    {
        printf("Hola soy A: mi pid es %d, el de mi padre es %d\n",getpid(),getppid());   
        pid = fork();
        if (pid == 0)//Esto sería B
        {
            printf("Hola soy B: mi pid es %d, el de mi padre es %d,el de mi abuelo es %d\n",getpid(),getppid(),pidejec);
            CreacionXYZ();
        }
        pause();
        kill(pid,SIGUSR1);
        wait(NULL);
        printf("Hola soy el proceso A(%d) y me muero\n",getpid());
        exit(0);
    }
    pause();
    pause();
    kill(pid,SIGUSR1);
    waitpid(pidA, NULL, 0);
    printf("Soy ejec(%d) y muero\n",getpid());
    exit(0);

    
        
    return 0;
}