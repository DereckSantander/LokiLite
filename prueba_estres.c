#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
void *reiniciar_servicio(void *arg){
	while(1){
		system("sudo systemctl restart vsftpd");
		sleep(10);
	}
}
int main(){
	pthread_t tid;
	int res = pthread_create(&tid, NULL, reiniciar_servicio, NULL);
	if(res!=0){
		perror("error al crear el hilo");
		exit(EXIT_FAILURE);
	}

	pthread_join(tid, NULL);
	return 0;
}
