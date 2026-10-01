#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int malla;
void makegrandsons(int y,bool ultimo){
    for (size_t i = 0; i < y-1; i++)
    {
        switch (fork()){
        case -1:
            perror("error");
            break;
        case 0:
            //hijo
            if (i == y && ultimo)
            {
                sleep(10);
                execlp("pstree","pstree","-d", getpid(),NULL);
                exit(0);
            }
            
            break;
        default:
            //padre
            wait(NULL);
            break;
        }
    }
    
}
void makesons(pid_t pid,int x,int y){
    pid_t hijos[x]; // Creamos array
    bool ultimo=false; //Booleano para comprobar que estamos en el último
    for (size_t i = 0; i < x; i++)//Este bucle genera tantos hijos x que se pide
    {
        switch (fork())
        {
        case -1:
            perror("Hay un error");
            break;
        case 0:
            //Hijo
            if (x-1==i)
            {
                //Este  será el que tengamos que hacer el exec
                hijos[i] = getpid();
                ultimo = true;
                makegrandsons(y,ultimo);
            }else{
                hijos[i] = getpid(); // Ponemos el pid del hijo
                makegrandsons(y,ultimo);
            
            }

            
            break;
        default:
            //Padre

            wait(NULL);
            break;
        }   
    }
}


int main(int argc, char const *argv[])
{
    
    pid_t pid = getpid();
    malla = getpid();
    for (size_t i = 0; i < atoi(argv[1]); i++)
    {
        
        switch (fork())
        {
        case -1:
            perror("Hay un error");
            break;
        case 0:
            //Hijo
            for (size_t j = 0; j < atoi(argv[2])-1; j++)
            {
                if (i != atoi(argv[1])-1 && j != atoi(argv[2])-2)
                {
                    execlp("pstree","pstree","-c",NULL);
                    exit(0);
                }else{
                    switch (fork())
                    {
                    case -1:
                        perror("Hubo un error y");
                        break;
                    case 0:
                        exit(0);
                        break;
                    default:
                        wait(NULL);
                        break;
                    }
                }
                
            }
        default:
            //Padre
            continue;
            break;
        }

    }
    
    


    return 0;
}
