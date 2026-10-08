//Librerías que estoy utilizando
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdbool.h>

int colum;
int filas;
pid_t pidmalla;

void despertador(int s){

}

/**
 * Valida los argumentos de entrada
 */
bool Validarargumentos(int cantidad,int x,int y){
        // Validar argumentos
    if (cantidad!=3)
    {
        perror("No has pasado los argumentos bien \n");
        return false;
    }

    if (x < 0 || y < 0)
    {
        perror("Los datos están en negativos\n");
        return false;
    }
    return true;
}

/**
 * Crea los verticales
 */
void CrearVertical(int x,int y){
    for (size_t j = 0; j < y; j++)
    {
        pid_t pid = fork();

        if (pid > 0)
        {
            //Padre
            pause();
            kill(pid,SIGUSR1);
            wait(NULL);
            break;
        }else{
            continue;
        }
    }
    if (x != colum && y != filas)
    {
        pause();
        //Cuando le llegue la señal se muere el último
        exit(0);
    }else{
        kill(pidmalla,SIGUSR1); //EL ultimo
        pause();
        exit(0);
    }
    
}

void CrearHorizontal(int x,int y,pid_t hijo[]){
    pid_t pidhijo;
    for (size_t i = 0; i < x; i++)
    {
        pid_t pidhijo;
        switch (pidhijo = fork())
        {
        case -1:
            perror("Error en CrearHoritontal\n");
            break;
        case 0:
            //Hijo
            CrearVertical(i,y-1);
            break;
        default:
            //Padre
            hijo[i] = pidhijo;
            continue;
            break;
        }
        pause();
        kill(pidhijo,SIGKILL);
        wait(NULL);
        exit(0);
    }

    
    
}


int main(int argc, char const *argv[])
{
    //Validar argumentos
    if (!Validarargumentos(argc,atoi(argv[1]),atoi(argv[2])))
    {
        return 1;
    }
    

    //Muestra ID
    pidmalla = getpid();
    
    colum = atoi(argv[1]);
    filas = atoi(argv[2]);
    printf("Hola soy malla y soy %d",getpid());
    // Signal()
    signal(SIGKILL,despertador);

    //Necesitamos saber el pid de los hijos
    pid_t hijo[colum];
    //Crearhorizontal

    CrearHorizontal(colum,filas,hijo);
    //Pause
    pause();


    char pidpadre[16];
    snprintf(pidpadre,sizeof(char)*16,pidpadre);
    execlp("pstree","pstree","-c",pidpadre,NULL);
    for (size_t i = 0; i < colum; i++)
    {
        kill(hijo[i],SIGKILL);
    }
    
    //printmuero
    printf("Soy Malla (%d) y me muero\n",getpid());

    //exit
    exit(0);
    return 0;
}
