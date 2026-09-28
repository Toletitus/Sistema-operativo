#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
void alarma(){
    //Esto espera a que el no tenga hijos
    wait(NULL);
}
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

void ModuloY(int x,pid_t p,int tiempo){
    int array[x];
    //Este modulo genera un cantidad de hijos x en el proceso p y lo almacena en un array
    if (x > 0)
    {
       for (int i = 0; i < x; i++)
        {
            switch (fork())
            {
            case -1:
                perror("Ha ocurrido algo durante el proceso de ejecución de xyz que ha fallado");
                exit(1);
                break;
            case 0:
                //Este sería el proceso hijo
                if(i != 2){
                    wait(NULL);
                    array[i] = getpid();
                }else if (i == 2)
                {
                    //Duerme X tiempo
                    array[i] = getpid();
                    sleep(tiempo);
                    for (size_t j = 0; j < i; j++)
                    {
                        kill(array[i],SIGKILL);
                    }
                    
                }                
                break;
            default:
                //El padre no tiene que hacer nada por lo que
                continue;
                break;
            }
        }
    }
    
}



int main(int argc, char const *argv[])
{
    pid_t pid;
    pid = Modulox(pid); // Esto sería A
    pid = Modulox(pid); // Esto sería B
    ModuloY(3,pid,); // Esto sería la creación de xyz
    

    return 0;
}
