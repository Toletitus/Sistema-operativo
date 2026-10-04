#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>

// Funcioncita para imprimir errores sin usar printf
void print_msg(char *txt) {
    int i = 0;
    while (txt[i] != '\0') i++;
    write(1, txt, i);
}

// Para pasar el argumento del tamaño a int
int a_entero(char *str) {
    int num = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        num = num * 10 + (str[i] - '0');
    }
    return num;
}

// Genera el .h00, .h01, etc.
void nom_fichero(char *dest, char *orig, int i) {
    int j = 0;
    while (orig[j] != '\0') {
        dest[j] = orig[j];
        j++;
    }
    dest[j++] = '.'; 
    dest[j++] = 'h';
    dest[j++] = '0' + (i / 10);
    dest[j++] = '0' + (i % 10);
    dest[j] = '\0';
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        print_msg("Uso: ./hacha <archivo> <tamano>\n");
        return 1;
    }

    char *archivo = argv[1];
    int tam = a_entero(argv[2]);

    // abrimos el archivo original
    int fd = open(archivo, O_RDONLY);
    if (fd < 0) {
        print_msg("Error al abrir el archivo\n");
        return 1;
    }

    // pillo el tamaño total y vuelvo al principio
    off_t total = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    int trozos = total / tam;
    if (total % tam != 0) {
        trozos++;
    }

    char buf[4096]; 

    // bucle para ir creando cada cacho
    for (int i = 0; i < trozos; i++) {
        int tubo[2];
        
        if (pipe(tubo) == -1) {
            print_msg("Fallo en la tuberia\n");
            return 1;
        }

        pid_t pid = fork();

        if (pid < 0) {
            print_msg("Fallo en el fork\n");
            return 1;
        }

        if (pid == 0) {
            // --- HIJO ---
            close(tubo[1]); // solo va a leer

            char nombre[256];
            nom_fichero(nombre, archivo, i);

            int f_dest = open(nombre, O_CREAT | O_WRONLY | O_TRUNC, 0666);
            int leido;
            
            // lee del tubo y escribe en su archivo
            while ((leido = read(tubo[0], buf, sizeof(buf))) > 0) {
                write(f_dest, buf, leido);
            }

            close(tubo[0]);
            close(f_dest);
            exit(0);

        } else {
            // --- PADRE ---
            close(tubo[0]); // solo va a escribir

            int falta = tam;
            int leido;

            while (falta > 0) {
                int leer_ahora = (falta < sizeof(buf)) ? falta : sizeof(buf);
                
                leido = read(fd, buf, leer_ahora);
                if (leido <= 0) break; 
                
                write(tubo[1], buf, leido);
                falta -= leido;
            }

            close(tubo[1]);
            wait(NULL); // me quedo esperando a que acabe el hijo
        }
    }

    close(fd);
    return 0;
}