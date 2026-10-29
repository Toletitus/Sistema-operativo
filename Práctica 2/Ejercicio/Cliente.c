#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PUERTO 9999

int conectar(char *ip);
void recibir(int sock);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s IP_Servidor\n", argv[0]);
        return 1;
    }
    int sock = conectar(argv[1]);
    recibir(sock);
    close(sock);
    return 0;
}

// Crea el socket y se conecta al servidor
int conectar(char *ip) {
    struct sockaddr_in servidor;
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    servidor.sin_family = AF_INET;
    servidor.sin_port = htons(PUERTO);
    servidor.sin_addr.s_addr = inet_addr(ip);

    if (connect(sock, (struct sockaddr *)&servidor, sizeof(servidor)) < 0) {
        perror("Error en connect");
        exit(1);
    }
    return sock;
}

// Lee lo que manda el servidor y lo muestra por pantalla
void recibir(int sock) {
    char buffer[1024];
    int n;
    while ((n = read(sock, buffer, sizeof(buffer))) > 0)
        write(1, buffer, n);
}