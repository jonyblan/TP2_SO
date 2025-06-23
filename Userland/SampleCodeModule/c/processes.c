#include <standardlib.h>
#include <pipelib.h>

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

void helpFunc(int argc, char* argv[]){
	printf("This is a sample process that runs in userland.\n");
	printf("It can be used to test the system calls and the process management.\n");
	printf("You can create processes, semaphores, pipes, and more.\n");
	printf("Use the 'test' command to run this process.\n");
}

void bloqueadoFunc(int argc, char* argv[]){
	uint8_t sem = sem_open("test", 1);
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
	uint8_t sem = sem_open("test", 1);
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
		pipe_read(pipeId,buffer,7); // Read from the pipe
		printf("%s\n", buffer);
		sem_post(readyToWrite); 
		sleep(100);
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