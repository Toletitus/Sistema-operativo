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

int* ModuloY(int x,pid_t p){

    //Este modulo genera un cantidad de hijos x en el proceso p y lo almacena en un array
    //Devuelve el puntero de esa array
    int* array = (int*) malloc( x * sizeof(int));
    if (array == NULL)
    {
        printf("Hemos comido polla\n");
        printf("No sabemos donde coño esta el puntero con los procesos");
    }
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
                //El proceso hijo va enviarle su pid al array que hemos creado
                array[i] = getpid();
                break;
            default:
                //El padre no tiene que hacer nada por lo que
                break;
            }
            if(i != 2){
                wait(NULL);
            }else if (i == 2)
            {
                //Básicamente ejecuta el pstree para mostrar todos los procesos del programa
                execlp("pstree -c", "pstree", "-c",NULL);
                //En esta parte hay que empezar a matar a todos los hijos
                aniquilarHijos();
            }
            
        }
    }
    

    
    

}



int main(int argc, char const *argv[])
{
    pid_t pid;
    pid = Modulox(pid); // Esto sería A
    pid = Modulox(pid); // Esto sería B



    return 0;
}
