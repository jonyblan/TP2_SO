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

void hablaFunc(int argc, char* argv[]){
	int fds[2];
	int i;
	uint8_t pipeId = pipe_open("hola", fds);
	while(1){
		printf("Hablador habla");
		char* msg = "Hello world\0";
		(void)pipe_write(fds[1], msg, 12);
		i = 0;		
		while(i++ < 50000){

		}
	}
}

void escuchaFunc(int argc, char* argv[]){
	int fds[2];
	int i;
	uint8_t pipeId = pipe_open("hola", fds);
	while(1){
		printf("Escuchador escucha");
		char buffer[64];
		(void)pipe_read(fds[0], buffer, 12);
		i = 0;
		while(i++ < 10000){
			
		}
	}
}

void loopFunc(int argc, char* argv[]){
	int fd[2];
	pipe_open("loopPipe", fd);
	char buf[10];
	pipe_read(fd[0], buf, 10);
	char seconds[10];
	int it = 0;
	uint32_t time = unsigned_str_to_num(&it, 10, seconds);
	while(1){
		wait(18*time);
		printf("Pid from loop function: %d\n", buf);
	}
}