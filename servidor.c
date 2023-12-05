#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
#define SERVER_IP "192.168.1.8"

int main(){
	int client_socket = 0;
	struct sockaddr_in server_addr;
	char buffer[1024] = {0};

	//Crear socket
	if((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Error al crear socket");
		exit(EXIT_FAILURE);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);

	//IP a binario
	if(inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
		perror("Direccion no valida");
		exit(EXIT_FAILURE);
	}

	//Conectar al servidor 
	if(connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		perror("Error al conectar");
		exit(EXIT_FAILURE);
	}
	int x = 0;	
	//Recibir salida del programa ejecutando en el servidor
	while(1){
		memset(buffer, 0, sizeof(buffer));
		if(recv(client_socket, buffer, sizeof(buffer), 0) < 0) {
			perror("Error al recibir la salida del programa");
			exit(EXIT_FAILURE);
		}
		
		//Imprimir salida
		printf("Salida del programa: \n%s\n", buffer);

		//posible envio de mensaje a WS
		x++;
		if(x==5) {
			x=0;
			system("python3 mensaje.py"); // Envio de mensaje a ws
		}
	}
	close(client_socket);
	return 0;
}
