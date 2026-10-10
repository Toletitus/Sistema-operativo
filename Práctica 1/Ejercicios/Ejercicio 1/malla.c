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
bool Validarargumentos(int argc, char const *argv[]){
    if (argc != 3)
    {
        fprintf(stderr, "Uso: %s <columnas> <filas>\n", argv[0]);
        return false;
    }

    int x = atoi(argv[1]);
    int y = atoi(argv[2]);

    if (x <= 0 || y <= 0)
    {
        fprintf(stderr, "Los datos deben ser mayores que 0\n");
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
            exit(0);
            break;
        }else{
            continue;
        }
    }
    if (x != colum-1 && y != filas-2)
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
            //No tiene que salir ningun hijo,hay pauses en la funcion
            break;
        default:
            //Padre
            hijo[i] = pidhijo;
            continue;
            break;
        }
        
    }
    
}


int main(int argc, char const *argv[])
{
    //Validar argumentos
    if (!Validarargumentos(argc, argv))
    {
        return 1;
    }

    

    
    //Muestra ID
    pidmalla = getpid();
    
    colum = atoi(argv[1]);
    filas = atoi(argv[2]);
    printf("Hola soy malla y soy %d\n",getpid());
    // Signal()
    signal(SIGUSR1,despertador);

    //Necesitamos saber el pid de los hijos
    pid_t hijo[colum];
    //Crearhorizontal

    CrearHorizontal(colum,filas,hijo);
    //Pause
    pause();



    if(fork() == 0){
        char pidpadre[16];
        snprintf(pidpadre, sizeof pidpadre, "%d", getppid());
        execlp("pstree","pstree","-c",pidpadre,NULL);
        kill(getppid(),SIGUSR1);
        exit(1);
    }
    wait(NULL);
    
    for (size_t i = 0; i < colum; i++)
    {
        kill(hijo[i],SIGUSR1);
        wait(NULL);//Espera a que muera toda la columna
    }
    
    //printmuero
    printf("Soy Malla (%d) y me muero \n",getpid());

    //exit
    exit(0);
    return 0;
}
