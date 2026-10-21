#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define PUERTO 9999
#define TAM_BUFFER 1024
#define ARCHIVO "Google.html"

// Declaración de módulos
int inicializar_servidor(int puerto);
void manejar_conexiones(int socket_servidor);
void transferir_archivo(int socket_cliente);

/**
 * =========================================================
 * FUNCIÓN PRINCIPAL
 * =========================================================
 */
int main() {
    int socket_servidor;

    // 1. Inicializar la infraestructura de red
    socket_servidor = inicializar_servidor(PUERTO);

    // 2. Iniciar el bucle de escucha y atención a clientes
    manejar_conexiones(socket_servidor);

    // 3. Finalización (Inalcanzable en un servidor continuo)
    close(socket_servidor);
    return 0;
}

/**
 * =========================================================
 * MÓDULO 1: Inicialización de la red (socket, bind, listen)
 * =========================================================
 */
int inicializar_servidor(int puerto) {
    int socket_servidor;
    struct sockaddr_in dir_servidor;

    // Crear el socket del servidor
    socket_servidor = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_servidor < 0) {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }

    // Permitir reutilizar el puerto inmediatamente
    int opcion = 1;
    setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &opcion, sizeof(opcion));

    // Configurar la estructura de la dirección
    memset(&dir_servidor, 0, sizeof(dir_servidor));
    dir_servidor.sin_family = AF_INET;
    dir_servidor.sin_addr.s_addr = INADDR_ANY;
    dir_servidor.sin_port = htons(puerto);

    // Publicar la dirección
    if (bind(socket_servidor, (struct sockaddr *)&dir_servidor, sizeof(dir_servidor)) < 0) {
        perror("Error en bind");
        close(socket_servidor);
        exit(EXIT_FAILURE);
    }

    // Disposición para aceptar conexiones
    if (listen(socket_servidor, 5) < 0) {
        perror("Error en listen");
        close(socket_servidor);
        exit(EXIT_FAILURE);
    }

    printf("Servidor iniciado y escuchando en el puerto %d...\n", puerto);
    return socket_servidor;
}

// =========================================================
// MÓDULO 2: Bucle de concurrencia (accept y fork)
// =========================================================
void manejar_conexiones(int socket_servidor) {
    int socket_cliente;
    struct sockaddr_in dir_cliente;
    socklen_t tam_dir = sizeof(struct sockaddr_in);

    // Evitar procesos zombi
    signal(SIGCHLD, SIG_IGN);

    while (1) {
        // Bloqueo hasta petición
        socket_cliente = accept(socket_servidor, (struct sockaddr *)&dir_cliente, &tam_dir);
        if (socket_cliente < 0) {
            perror("Error en accept");
            continue;
        }

        printf("Cliente conectado desde %s\n", inet_ntoa(dir_cliente.sin_addr));

        // Concurrencia: delegar en proceso hijo
        pid_t pid = fork();
        if (pid < 0) {
            perror("Error en fork");
            close(socket_cliente);
        } else if (pid == 0) {
            // Proceso hijo: transfiere y muere
            close(socket_servidor); 
            transferir_archivo(socket_cliente);
        } else {
            // Proceso padre: libera socket cliente y sigue escuchando
            close(socket_cliente); 
        }
    }
}

// =========================================================
// MÓDULO 3: Lógica de negocio (Lectura y envío de datos)
// =========================================================
void transferir_archivo(int socket_cliente) {
    int fd_archivo = open(ARCHIVO, O_RDONLY);
    if (fd_archivo < 0) {
        perror("Error al abrir Google.html");
        close(socket_cliente);
        exit(EXIT_FAILURE);
    }

    char buffer[TAM_BUFFER];
    ssize_t leidos;

    // Transmisión del archivo
    while ((leidos = read(fd_archivo, buffer, sizeof(buffer))) > 0) {
        if (write(socket_cliente, buffer, leidos) < 0) {
            perror("Error al enviar datos");
            break;
        }
    }

    close(fd_archivo);
    close(socket_cliente);
    exit(EXIT_SUCCESS);
}