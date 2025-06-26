#include <standardlib.h>
#include <nanoshell.h>
#include <stddef.h>
#include <pipelib.h>
#include <videolib.h>

extern throw_zero_division();
extern throw_invalid_opcode();
#define MAX_PROCESSES 256
static char *todo[] = {
// help
"Check that they are on date",
// registers
"",
// time
"",
// echo
"Make it be able to print other things (echo test_malloc for example)",
// clear
"",
// test_zero_divition
"",
// test_invalid_opcode
"",
// test_malloc
"",
// todo
"write a lot of TODOs\n\
make it return a random todo",
// functions
"Not implemented",
// mini_process
"Make easier to understand",
// test_priority
"",

// Useful

// malloc
"",
// realloc
"Not implemented",
// calloc
"Not implemented",
// free
"",
// createProcess
"Not implemented",
// getPriority
"",
// setPriority
"",
};

static char *help_text = "Here's a list of all available commands:\n\
- help --> Help display with all commands\n\
- registers --> Displays the lastest backup of registers\n\
- time --> Displays time and date\n\
- echo [string] --> Prints the [string] argument in the display\n\
- clear --> clears the display\n\
- test_zero_division --> Test for the Zero Division exception\n\
- test_invalid_opcode --> Test for the Invalid Opcode exception\n\
- test_malloc --> starts the malloc test\n\
- todo --> displays a random thing that has to be done\n\
- functions --> displays every page inside the manual\n\
- mini_process --> creates a new process according to simpleProcess.c\n\
- test_priority --> test that the priority system is working correctly\n\
- test_semaphore --> test that the semaphore system is working correctly\n\
- test_pipe --> test that the pipe system is working correctly\n\
- mem --> shows the memory state NOT DONE\n\
";

static char *help_text2 = "- ps --> prints a list of every running process with some data from each NOT DONE\n\
- loop --> makes a process run and print its id along with a greeting every [count] of seconds\n\
- kill [pid] --> kills a process based on its [pid] NOT TESTED\n\
- nice [pid] [new priority] --> changes the [pid] process to be of [new priority] priority NOT TESTED\n\
- block [pid] --> changes the [pid] process between blocked and unblocked NOT TESTED\n\
- cat --> prints the stdin\n\
- wc --> counts the amount of lines in the input\n\
- filther --> filthers the vowels from the input \n\
- phylo --> starts running the phylosofers problem. \"a\" to add 1, \"r\" to remove one NOT DONE\n\
";

// add new command or useful here
static char *functions = "\
Commands: help, registers, time, echo, clear, test_zero_division\n\
test_invalid_opcode, test_malloc, todo, functions, mini_process, test_priority\n\n\n\
Useful: malloc, realloc, calloc, free, getPriority, setPriority\n";


void bloqueadoFunc(int argc, char* argv[]){
	uint8_t sem = sem_open("test", 0);
	int j;
	for (int i = 0; i < 10; i++)
	{
		printf("(por ahi) Me bloqueo! \n");
		sem_wait(sem);
		printf("Me desbloquie :)\n");
		sleep(100);
	}
}

void liberadorFunc(int argc, char* argv[]){
	uint8_t sem = sem_open("test", 0);
	int j;
	for (int i = 0; i < 10; i++)
	{
		sem_post(sem);
		printf("Liberador sem_post value: %d\n",j );
		sleep(800);
	}
}

void hablaFunc(int argc, char* argv[]){
	uint8_t pipeId = pipe_open("hola");
	uint8_t readyToPrint= sem_open("readyToPrint", 0); // Initialize semaphore for the pipe
	uint8_t readyToWrite= sem_open("readyToWrite", 0); // Initialize semaphore for the listener
	changeProcessFd(getMyPID(), pipeId, 1); // Set write end of pipe for this process
	for (int i = 0; i < 10; i++)
	{
		printf("AAAAAA\n");
		sem_post(readyToPrint); 
		sem_wait(readyToWrite); 
		sleep(100);
	}
}

void escuchaFunc(int argc, char* argv[]){
	uint8_t pipeId = pipe_open("hola");
	uint8_t readyToPrint= sem_open("readyToPrint", 0); // Initialize semaphore for the pipe
	uint8_t readyToWrite= sem_open("readyToWrite", 0); // Initialize semaphore for the listener
	changeProcessFd(getMyPID(), pipeId, 0); // Set read end of pipe for this process
	for(int i=0; i < 10; i++){
		sem_wait(readyToPrint); 
		char buffer[7];
		scanf("%s",buffer); // Read from the pipe
		printf("%s\n", buffer);
		sem_post(readyToWrite); 
		sleep(100);
	}
}

void loopFunc(int argc, char* argv[]){
	pid_t pid;
	pid=getMyPID();
	
	int time=2;
	while(1){
		sleep(120*time); //tick frecuncy * time
		printf("Pid from loop function: %d\n", pid);
	}
}

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

void timeFunc(int argc, char* argv[]){
	printCurrentTime();
}

void echoFunc(int argc, char* argv[]){
    for (int i = 1; i < argc; i++) {  // arrancás desde 1 para ignorar "echo"
        printf("%s", argv[i]);
        if (i != argc - 1)
            printf(" "); 
    }
    printf("\n");
}


void registersFunc(int argc, char* argv[]){
	getRegisters();
}

void helpFunc(int argc, char* argv[]){
	printf("%s", help_text);
	printf("%s", help_text2);	
}

void clearFunc(int argc, char* argv[]){
	clearScreen();
}

void testZeroDivisionFunc(int argc, char* argv[]){
	throw_zero_division();
}

void testInvalidOpcodeFunc(int argc, char* argv[]){
	throw_invalid_opcode();
}

void testMallocFunc(int argc, char* argv[]){
	printf("%d\n", testMalloc());
}

void todoFunc(int argc, char* argv[]){
	TimeStamp* ts ;
	ts=getTime();
	while(todo[ts->seconds%INSTRUCTION_COUNT] == ""){
		ts->seconds++;
	}
	printf("%d\n", ts->seconds);
	printf("%s\n", todo[ts->seconds%INSTRUCTION_COUNT]);
}

void functionsFunc(int argc, char* argv[]){
	printf("%s\n", functions);
}

void testPriorityFunc(int argc, char* argv[]){
	pid_t pid1, pid2, pid3;
	pid1 = (pid_t)createProcess(&testFunc, 1, argv, "test1");
	pid2 = (pid_t)createProcess(&testFunc, 2, argv, "test2");
	pid3 = (pid_t)createProcess(&testFunc, 3, argv, "test3");
	printf("priorities: %d: %d, %d: %d, %d: %d\n\n", pid1, getPriority(pid1), pid2, getPriority(pid2), pid3, getPriority(pid3));
	setPriority(pid1, 1);
	setPriority(pid2, 7);
	setPriority(pid3, 7);
	printf("priorities: %d: %d, %d: %d, %d: %d\n\n", pid1, getPriority(pid1), pid2, getPriority(pid2), pid3, getPriority(pid3));
	wait(pid1);
	wait(pid2);
	wait(pid3);
}

void testSemaphoreFunc(int argc, char* argv[]){
	pid_t pid1, pid2;
	pid1 = (pid_t)createProcess((void*)bloqueadoFunc, 1, argv,"Bloqueado");
	pid2 = (pid_t)createProcess((void*)liberadorFunc, 2, argv,"Liberador");
	wait(pid1);
	wait(pid2);
}

void testPipeFunc(int argc, char* argv[]){
	pid_t pid1, pid2;
	pid1 = (pid_t)createProcess((void*)hablaFunc, 1, argv,"Hablador");
	pid2 = (pid_t)createProcess((void*)escuchaFunc, 2, argv,"escuchante");
	printf("Termine\n");
	/* wait(pid1);
	wait(pid2); */
}

void useMemoryFunc(int argc, char* argv[]){
	int time = 10;
	int size = 0x1000;
	if(argc == 2 || argc == 3){
		time = (int)atoi(argv[1]);
	}
	if(argc == 3){
		size = (int)atoi(argv[2]);
	}
	int* ptr = malloc(size);
	printf("Usando %d bytes en memoria y esperando por %d segundos\n", size, time);
	sleep(120*time);
	printf("Liberando %d bytes en memoria\n", size);
	free(ptr);
	
}

void memFunc(int argc, char* argv[]){
	return;
}
void psFunc(int argc, char* argv[]){
	processInfo array[MAX_PROCESSES];
	uint8_t count;
	count= ps(array);
	char *msg[] = {"READY", "RUNNING", "BLOCKED", "TERMINATED"};
	int index;
	printf("PID\tNombre\tEstado\tPrioridad\tStack Base\t Stack Pointer\n");
	for (uint8_t i = 0; i < count; i++)
	{
		index=0;
		switch (array[i].state) {
			case RUNNING:;
				index = 1;
				break;
			case BLOCKED:;
				index = 2;
				break;
			case TERMINATED:;
				index=3;
				break;
			default:
				break;
		}
		printf("%d\t | %s\t |  %s\t   | %d\t      |    %x\t     |    %x\n",array[i].pid,array[i].name,msg[index],array[i].priority,array[i].stackBase,array[i].stackPointer);
	}
	printf("perdon\n");
	return;
}

void killFunc(int argc, char* argv[]){
	if (argc < 2) {
		printf("Uso: kill [pid]\n");
		return;
	}
	pid_t pid = (pid_t)atoi(argv[1]);
	if (pid <= 0) {
		printf("Invalid PID: %s\n", argv[1]);
		return;
	}
	killProcess(pid);
	printf("Proceso con PID %d killed.\n", pid);
}

void niceFunc(int argc, char* argv[]){
	if (argc < 3) {
		printf("Uso: nice [pid] [new priority]\n");
		return;
	}
	pid_t pid = (pid_t)atoi(argv[1]);
	int newPriority = atoi(argv[2]);
	if (pid <= 0 || newPriority < 0) {
		printf("PID o prioridad inválidos: %s %s\n", argv[1], argv[2]);
		return;
	}
	setPriority(pid, newPriority);
	printf("Proceso con PID %d cambiado a prioridad %d.\n", pid, newPriority);
}

void blockFunc(int argc, char* argv[]){
	if (argc < 2) {
		printf("Uso: block [pid]\n");
		return;
	}
	pid_t pid = (pid_t)atoi(argv[1]);
	if (pid <= 0) {
		printf("PID inválido: %s\n", argv[1]);
		return;
	}
	blockProcess(pid);
	printf("Proceso con PID %d bloqueado\n", pid);
}

void unblockFunc(int argc, char* argv[]){
	if (argc < 2) {
		printf("Uso: unblock [pid]\n");
		return;
	}
	pid_t pid = (pid_t)atoi(argv[1]);
	if (pid <= 0) {
		printf("PID inválido: %s\n", argv[1]);
		return;
	}
	unblockProcess(pid);
	printf("Proceso con PID %d desbloqueado\n", pid);
}

void catFunc(int argc, char* argv[]){
	char buffer[100];
	scanf("%s",buffer);
	printf("%s\n",buffer);
	return;
}

void wcFunc(int argc, char* argv[]){
	char buffer[256];

    scanf("%s", buffer); 

    int lines = 0;
    for (int i = 0; buffer[i] != 0; i++) {
        if (buffer[i] == '\n') {
            lines++;
        }
    }

    if (lines == 0 && buffer[0] != 0)
        lines = 1;

    printf("Cantidad de líneas: %d\n", lines);
}

void filterFunc(int argc, char* argv[]){
	char buffer[256];

    scanf("%s", buffer);  

    for (int i = 0; buffer[i] != 0; i++) {
        char c = buffer[i];
        if (c != 'a' && c != 'e' && c != 'i' && c != 'o' && c != 'u' && c != 'A' && c != 'E' && c != 'I' && c != 'O' && c != 'U') {
            putChar(c); 
        }
    }
    putChar('\n');
}

void phyloFunc(int argc, char* argv[]){
	printf("not implemented\n");
}

