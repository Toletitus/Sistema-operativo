#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>


void Generarhorizontal(int x,int y,pid_t pidpadre){
    pid_t array[x-1];
    for(size_t i = 0; i<x;i++){
        switch(fork()){

            case -1:
                perror("Hubo un error al crear la primera generación");
                break;
            case 0:
                //Este es el hijo,el hijo realizará el bucle

                //Creamos array: 
                
                for(size_t j = 0;j<y-1;j++){
                    switch(fork()){
                        case -1:
                            perror("Ha ocurrido un error");
                            break;
                        case 0:
                            //EL hijo
                            if(j != y-2){
                                continue;
                            }else if(j == y-2 && i!=x){
                                array[i] = getpid();
                                pause();//Aquí esperamos a que nos envien una señal
                            }else if(j == y-2 && i==x){
                                //Este sería el que tiene que ejecutar el exec y posteriormente enviarle una señal a las señales que estan en pause
                                for(size_t k = 0;k<x-1;k++){
                                    kill(array[k],SIGKILL); //Esto sería para matar los procesos
                                }
                                execlp("pstree","pstree","-c ",pidpadre,NULL);
                                
                                //Posteriormente podemos hacer un exit(0) para finalizar el programa y los demás procesos se borrarán debido a que esan en wait y los hijos han muerto todos
                                exit(0);
                            }
                            break;
                        default:
                            //El padre tiene que esperar a los hijos
                            wait(NULL);
                            break;
                    }
                }
                break;
            default:
                //El padre se espera a que finalice los hijos
                if(i==x-1){wait(NULL);}
                break;
        }
    }
}
int main(int argc, char const *argv[])
{
    pid_t p = getpid();
    if(argc == 3){
        Generarhorizontal(atoi(argv[1]),atoi(argv[2]),p);
    }else{
        printf("No has puesto dos argumentos");
    }
    return 0;
}
