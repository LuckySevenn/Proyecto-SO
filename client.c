#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
    
#define PUERTO 7079

// Función para manejar errores y finalizar el programa
void manejar_error(const char *mensaje) {
  perror(mensaje);
  exit(EXIT_FAILURE);
}

// Función para crear un socket
int crear_socket() {
  int socket_cliente = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_cliente == -1) {
    manejar_error("Error al crear el socket");
  }
  return socket_cliente;
}

// Función para configurar la dirección del servidor
struct sockaddr_in configurar_direccion_servidor() {
  struct sockaddr_in direccion_servidor;
  direccion_servidor.sin_family = AF_INET;
  direccion_servidor.sin_addr.s_addr =
      inet_addr("127.0.0.1"); // Cambiar a la IP del servidor
  direccion_servidor.sin_port = htons(PUERTO);
  return direccion_servidor;
}

// Función para conectar al servidor
void conectar_servidor(int socket_cliente,
                       struct sockaddr_in *direccion_servidor) {
  if (connect(socket_cliente, (struct sockaddr *)direccion_servidor,
              sizeof(*direccion_servidor)) == -1) {
    manejar_error("Error al conectar al servidor");
  }
  printf("Conectado al servidor en %s:%d\n",
         inet_ntoa(direccion_servidor->sin_addr),
         ntohs(direccion_servidor->sin_port));
}

// Función para enviar comandos al servidor y recibir respuestas
void enviar_comandos_y_recibir_respuesta(int socket_cliente) {
  char comando[1024];
  char buffer[1024];
  while (1) {
    printf("Ingrese un comando (o 'exit' para salir): ");
    fgets(comando, sizeof(comando), stdin);
    comando[strcspn(comando, "\n")] =
        '\0'; // Eliminar el carácter de nueva línea
    if (strcmp(comando, "exit") == 0) {
      break;
    }
    send(socket_cliente, comando, strlen(comando), 0);
    memset(buffer, 0, sizeof(buffer));
    ssize_t bytes_recibidos = recv(socket_cliente, buffer, sizeof(buffer), 0);
    if (bytes_recibidos <= 0) {
      break; // Si no hay datos, salir del bucle
    }
    printf("Respuesta del servidor:\n%s\n", buffer);
  }
}

// Función para cerrar la conexión
void cerrar_conexion(int socket_cliente) { close(socket_cliente); }

// Función principal
int main() {
  // Crear un socket
  int socket_cliente = crear_socket();
  // Configurar la dirección del servidor
  struct sockaddr_in direccion_servidor = configurar_direccion_servidor();
  // Conectar al servidor
  conectar_servidor(socket_cliente, &direccion_servidor);
  // Enviar comandos y recibir respuestas
  enviar_comandos_y_recibir_respuesta(socket_cliente);
  // Cerrar la conexión
  cerrar_conexion(socket_cliente);
  return 0;
}
