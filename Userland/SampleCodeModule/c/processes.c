#include <standardlib.h>

void startProcess(int argc, char* argv[]){
	while(1){
		printf("Simple process running.\nargc: %d\n", argc);
	}
}

void testFunc(int argc, char* argv[]){
	//while(1){
		printf("Function %d running!\n", argc);
	//}
}

void bloqueadoFunc(int argc, char* argv[]){
	uint8_t sem = sem_open("test", 1);
	int i;
	while(1){
		printf("Bloqueado: Bloqueando...");
		sem_wait(sem);
		printf("Bloqueado: Desbloqueado");
		i = 0;
		while(i++ < 10000){
		
		}	
	}
}

void liberadorFunc(int argc, char* argv[]){
	uint8_t sem = sem_open("test", 1);
	int i;
	while(1){
		printf("Liberador: Desbloqueando...");
		sem_post(sem);
		i = 0;
		while(i++ < 50000){
		
		}	
	}
}