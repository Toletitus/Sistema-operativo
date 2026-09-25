#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
pid_t Modulox(pid_t p){
    p = fork();
    switch (p)
    {
    case -1:
        perrior("Existe un error");
        break;
    case 0:
        //Hijo
        printf("Hola soy el hijo y mi pid es %d y el de mi padre es %d", getpid(),getppid());
        break;
    default:
        //Padre
        printf("Hola soy el padre y mi pid es %d",getpid());
        break;
    }
    wait(NULL);
}

void ModuloY(pid_t p){
    for(int i = 0; i < 3 ; i++){
        p = fork();
        if(p == 0){
            continue;
        }else if(i == 2){
            execlp("ver contenido", "pstree","-c",NULL);
            aniquilarHijos();
        }
    }
    wait(NULL);
}

int main(int argc, char const *argv[])
{
    pid_t pid;
    pid = Modulox(pid); // Esto sería A
    pid = Modulox(pid); // Esto sería B
    ModuloY(pid); // Esto sería la creación de xyz
    

    return 0;
}
