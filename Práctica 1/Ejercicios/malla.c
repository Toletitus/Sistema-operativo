#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
int main(int argc, char const *argv[])
{
    int x,y;
    x = atoi(argv[1]);
    y = atoi(argv[2]);
    pid_t pid;
    for(int i=0; i<x; i++){
        pid = fork();
        if(pid == 0){
            for(int j=0; j<y; j++){
                printf("Soy el proceso %d y mi padre es %d\n", getpid(), getppid());
            }
        }
    }

    return 0;
}
