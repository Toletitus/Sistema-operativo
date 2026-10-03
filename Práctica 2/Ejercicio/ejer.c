#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>


//Variables globlales
pid_t pidejec,pidA,pidB,pidX,pidY,pidZ;

void alarma(){
    execlp("pstree","pstree","-c",pidA,NULL);
}
void expansionymuerte(){
    if(getpid() == pidA){

    }else if(getpid() == pidB){
        
    }
}


void crearhijosxyz(pid_t p){
    for (size_t i = 0; i < 3; i++)
    {
        switch (fork())
        {
        case -1:
            perror("Error xyz");
            break;
        case 0:
            switch (i)
            {
            case 0:
                pidX = getpid();
                wait(NULL);
                break;
            case 1:
                pidY = getpid();
                wait(NULL);
                break;
            case 2:
                pidZ = getpid();
                sleep(15);
                signal(pidA,SIGUSR1);
                break;
            }
            break;
        default:
            wait(NULL);
            break;
        }
    }
    
}

int main(int argc, char const *argv[])
{
    pidejec = getpid();
    
    for (size_t i = 0; i < 2; i++)
    {
        switch (fork())
        {
        case -1:
            perror("Hubo un error");
            break;
        case 0:
            if (i == 0)
            {
                pidA = getpid();
            }
            else if(i == 1){
                pidB = getpid();
                crearhijosxyz(pidB);
            }
            
            break;
        default:
            wait(NULL);
            break;
        }
    }
    
    
    
    
    return 0;
}
