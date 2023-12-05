#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080
#define MAX_CLIENTS 10

void *client_handler(void *socket_desc){
	int client_socket = *((int*)socket_desc);
	char message[1024];

	strcpy(message, "Hola, soy el servidor");
	send(client_socket, message, strlen(message), 0);

	close(client_socket);
	pthread_exit(NULL);
}

void *execute_program(void *args){
	
	struct program_args {
		char *program_name;
		int interval;
	};
	
	struct program_args *params = (struct program_args *)args;
	char *program = params->program_name;
	int interval = params->interval;

	//Variables para manejar sockets
	int server_socket, client_socket[MAX_CLIENTS], new_socket;
	struct sockaddr_in server_addr, client_addr;
	int opt = 1;
	int addrlen = sizeof(server_addr);
	
	//Crear socket para el servidor
	if((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("Error al crear socket del servidor");
		exit(EXIT_FAILURE);
	}

	//Configurar opciones del socket
	if(setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
		perror("Error al configurar opciones del socket");
		exit(EXIT_FAILURE);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(PORT);
	
	//Asignar direccion y puerto al socket del servidor
	if(bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		perror("Error al asignar direccion al socket del servidor");
		exit(EXIT_FAILURE);
	}

	//Escuchar conexiones
	if(listen(server_socket, MAX_CLIENTS) < 0) {
		perror("Error al escuchar conexiones");
		exit(EXIT_FAILURE);
	}
	
	printf("Servidor esperando conexiones...\n");


	//Ejecutar el ./agente
	int i=0;
	while(1){
		int client_addr_len = sizeof(client_addr);
		if ((new_socket = accept(server_socket, (struct sockaddr *)&client_addr, (socklen_t *)&client_addr_len)) < 0){
			perror("Error al aceptar la conexion");
			exit(EXIT_FAILURE);
		}
		
		//Enviar salida al cliente
		while(1){
			FILE *fp = popen(program, "r");
			char buffer[1024];
			char output[8192]="";

			while(fgets(buffer, sizeof(buffer), fp) != NULL) {
				strcat(output,buffer);
			}
			pclose(fp);
			send(new_socket, output, strlen(output), 0);
			sleep(interval); //interval
		}
		//Cerrar conexion
		close(new_socket);
	}
}

int main(int argc, char *argv[]) {
	if(argc != 3){
		printf("Uso: %s <programa_a_ejecutarse> <intervalo_en_segundos> \n", argv[0]);
		return EXIT_FAILURE;
	}
	
	char *program_to_execute = argv[1];
	int interval = atoi(argv[2]);
	
	if(interval <= 0){
		printf("El intervalo debe ser mayor a 0\n");
		return EXIT_FAILURE;
	}
	
	struct program_args {
		char *program_name;
		int interval;
	};

	struct program_args args = {program_to_execute, interval};

	pthread_t tid;
	pthread_create(&tid, NULL, execute_program, (void *)&args);
	pthread_join(tid,NULL);

	return 0;

}
