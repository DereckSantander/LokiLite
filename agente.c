#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <pthread.h>

pthread_mutex_t mutex;

int main(int argc, char *argv[]) {
	if(argc < 3 ) {
		printf("Uso: %s <servicio1> <servicio2> ... <prioridad> \n", argv[0]);
		return 1;
	}
	
	//Logs de los servicios filtrados por prioridad

	char *prioridad = argv[argc-1]; //Obtener la prioridad
	
	//Crear semaforo para proteger el acceso a la variable count
	pthread_mutex_init(&mutex, NULL);

	//Mostrar los logs por cada servicio especificado filtrados por la prioridad dada
	for(int i = 1; i < argc-1; i++){
		printf("Servicio: %s\n", argv[i]);

		int pipefd[2];
		if(pipe(pipefd) == -1 ) {
			perror("Error en la función pipe");
			return 1;
		}

		pid_t pid = fork();

		if(pid == -1){
			perror("Error en la funcion hijo");
			return 1;
		} else if(pid == 0) { //Proceso hijo
			close(pipefd[0]); //Cerrar lectura del pipe
			
			dup2(pipefd[1], STDOUT_FILENO); //Redirrecion al extremo de escritura del pipe
			close(pipefd[1]); //Cerrar escritura del pipe
			
			char *args[] = {"journalctl", "-b", "-l", "-p", prioridad, "-u", argv[i], NULL};

			execvp(args[0], args);

			//Si execvp falla
			perror("Error al ejecutar execvp");
			exit(1);
		} else { //Proceso padre
			close(pipefd[1]); // Cerrar escritura del pipe
			
			pthread_mutex_lock(&mutex); //Bloquear el semaforo en el proceso de lectura y conteo
			char buffer;
			int count = 0;

			while(read(pipefd[0], &buffer, 1) > 0) {
				if(buffer == '\n'){
					count++;
				}
				write(STDOUT_FILENO, &buffer, sizeof(buffer)); //Imprimir salida
			}

			printf("Cantidad de mensajes de prioridad %s: %d\n",prioridad ,count);
			pthread_mutex_unlock(&mutex); //Desbloqueo del semaforo
			close(pipefd[0]); //Cerrar lectura del pipe
		}
	}

	return 0;
}
