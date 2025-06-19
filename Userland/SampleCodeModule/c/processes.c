#include <standardlib.h>

void startProcess(int argc, char* argv[]){
	for (int i = 0; i < 10; i++)
	{
		printf("Simple process running.\nargc: %d\n", argc);
	}
}

void testFunc(int argc, char* argv[]){
	for (int i = 0; i < 10; i++)
	{
		printf("Function %d running!\n", argc);
	}	
}

void bloqueadoFunc(int argc, char* argv[]){
	uint8_t sem = sem_open("test", 1);
	for (int i = 0; i < 10; i++)
	{
		printf("Bloqueado sem_wait value: %d\n", sem_wait(sem));
		sleep(500);
	}
}

void liberadorFunc(int argc, char* argv[]){
	uint8_t sem = sem_open("test", 1);
	for (int i = 0; i < 10; i++)
	{
		printf("Liberador sem_post value: %d\n", sem_post(sem));
		sleep(800);
	}
}

void hablaFunc(int argc, char* argv[]){
	uint8_t pipeId = pipe_open("hola");
	for (int i = 0; i < 10; i++)
	{
		printf("Hablador habla \n" ,pipeId );
		char* msg = "Hello world\0\n";
		(void)pipe_write(pipeId, msg, 12);
		i = 0;		
		sleep(1000);
	}
}

void escuchaFunc(int argc, char* argv[]){
	uint8_t pipeId = pipe_open("hola");

	for(int i=0; i < 10; i++){
		printf("Escuchador escucha %d\n", pipeId);
		char buffer[64];
		(void)pipe_read(pipeId, buffer, 12);
		i = 0;
		printf("Escuchador received: %s\n", buffer);
		sleep(1000);
	}
}

void loopFunc(int argc, char* argv[]){
	pid_t pid;
	pid=getMyPID();
	
	int time=5;
	while(1){
		sleep(120*time); //tick frecuncy * time
		printf("Pid from loop function: %d\n", pid);
	}
}