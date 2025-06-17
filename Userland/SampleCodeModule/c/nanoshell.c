#include <nanoshell.h>
#include <standardlib.h>
#include <videolib.h>
#include <processes.h>
#include <stddef.h>
#include <memStatus.h>

#define CMD_MAX_CHARS 100
#define CMD_NAME_MAX_CHARS 100
#define PROMPT "NanoShell $> "

void startProcess(int argc, char* argv[]);
void testFunc(int argc, char* argv[]);

void bloqueadoFunc(int argc, char* argv[]);
void liberadorFunc(int argc, char* argv[]);
void hablaFunc(int argc, char* argv[]);
void escuchaFunc(int argc, char* argv[]);
void loopFunc(int argc, char* argv[]);
	
// add new command or useful here
static char *instructions[] = {"help", "registers", "time", "eliminator", "echo", "clear", "change_font", "nano_song", "test_zero_division", \
"test_invalid_opcode", "test_malloc", "man", "todo", "functions", "mini_process", "test_priority", "sh", "mem", "ps", "loop", "kill", "nice", "block", "cat", \
"wc", "filther", "phylo", \
/*useful*/ "malloc", "realloc", "calloc", "free", "createProcess", "getPriority", "setPriority", 0,};

// add new command here
static char *help_text = "Here's a list of all available commands:\n\
- help --> Help display with all commands\n\
- registers --> Displays the lastest backup of registers\n\
- time --> Displays time and date\n\
- eliminator --> Starts the eliminator game\n\
- echo [string] --> Prints the [string] argument in the display\n\
- clear --> clears the display\n\
- change_font --> Changes the current font\n\
- nano_song --> Use command for a surprise\n\
- test_zero_division --> Test for the Zero Division exception\n\
- test_invalid_opcode --> Test for the Invalid Opcode exception\n\
- test_malloc --> starts the malloc test\n\
- man [command] --> displays the manual for the given command\n\
- todo --> displays a random thing that has to be done\n\
- functions --> displays every page inside the manual\n\
- mini_process --> creates a new process according to simpleProcess.c\n\
- test_priority --> test that the priority system is working correctly\n\
- sh -->\n\
- mem -->\n\
- ps -->\n\
- loop -->\n\
- kill -->\n\
- nice -->\n\
- block -->\n\
- cat -->\n\
- wc -->\n\
- filther -->\n\
- phylo -->";

// add new command or useful here
static char *man[] = {
"help: Displays a basic help with all commands. Use to get a list of them but use man to get a more detailed description\n\
of each command\n",
"registers: Displays the latest backup of registers. Press the esc key at ant time to back them up\n\
then write this command to see them\n",
"time: Displays time and date on GTM -3\n",
"eliminator: starts the eliminator game. Play alone or with a friend to get the highest\n\
score possible\n",
"echo [string]: prints the [string] argument in the display\n\
on later iterations of the OS\n",
"clear: clears the display\n",
"change_font: Changes the current format from small to big or from big to small\n",
"nano_song: autism goes BRRRRRR\n",
"test_zero_divition: Tests the Zero division exception\n",
"test_invalid_opcode: Test the Invalid Opcode exception\n",
"test_malloc: starts the malloc test\n\
return value: returns 0 if the test was passed, any other number is an error.\n\
look at the code to see where it broke\n",
"man [command]: displays the manual for [command]. Has useful info about how and why to use it\n\
Also, some things may not be implemented yet and may cause unexpected behavior. If that happens, the entry\n\
inside man will have \"TODO: Not implemented\"",
"todo: don't know what to do? run this command and you'll be given a random task in the TODO list\n",
"functions: Displays every page inside the manual. Useful for testing and to play around\n",
"mini_process: Creates a new process according to simpleProcess.c\n",
"test_priority: There will be 2 processes created, process 1 has priority 1 and process 2 has priority 7 (max).\n\
they will be created in that order. The 2 are in a file in Userland called priorityTest.c\n\
check what happens to adress if the priority system is working.\n",
"sh\n", "mem\n", "ps\n", "loop\n", "kill\n", "nice\n", "block\n", "cat\n", "wc\n", "filther\n", "phylo\n",

// Useful

"malloc:\n\
use: void* malloc(uint64_t size)\n\
description: the malloc function allocates [size] bytes and returns a pointer to the allocated memory.\n\
return value: returns a pointer to the allocated memory or NULL in case of error or no space available\n",
"realloc:\n\
use: void* realloc(void* pnt, uint64_t size)\n\
description: the realloc function allocates [size] bytes and returns a pointer to the allocated memory. The memory is the same from the start until the end of pnt\n\
return value: returns a pointer to the allocated memory or NULL in case of error or no space available\n",
"calloc:\n\
use: void* calloc(uint64_t size)\n\
description: the malloc function allocates [size] bytes and returns a pointer to the allocated memory. It sets every value inside of it to 0\n\
return value: returns a pointer to the allocated memory or NULL in case of error or no space available\n",
"free:\n\
use: void free(void* pnt)\n\
description: free frees the memory that was previously allocated by pnt via malloc() or calloc(). Nothing happens if pnt is NULL\n\
return value: free doesnt return anything\n",
"createProcess:\n\
use:\n\
description: \n\
return value: \n",

"getPriority:\n\
use: getPriority(pid_t pid)\n\
description: gets the priority of the sent pid\n\
return value: returns the priority of the sent pid. Returns -1 if pid is out of bounds. Runtime error if no process exists with that pid\n",
"setPriority:\n\
use: setPriority(pid_t pid, int newPriority)\n\
description: sets a new priority for the sent pid. Higher numbers mean higher priorities.\n\
"
};

// add new command or useful here
static char *functions = "\
Commands: help, registers, time, eliminator, echo, clear, change_font, nano_song, test_zero_division\n\
test_invalid_opcode, test_malloc, man, todo, functions, mini_process, test_priority\n\n\n\
Useful: malloc, realloc, calloc, free, getPriority, setPriority, sh, mem, ps, loop, kill, \n\
nice, block, cat, wc, filther, phylo\n";

// add new command or useful here
static char *todo[] = {
// help
"Check that they are on date",
// registers
"",
// time
"",
// eliminator
"",
// echo
"Make it be able to print other things (echo test_malloc for example)",
// clear
"",
// change_font
"make it able to take arguments for the font size",
// nano_song
"",
// test_zero_divition
"",
// test_invalid_opcode
"",
// test_malloc
"",
// man
"write a manual for each argument",
// todo
"write a lot of TODOs\n\
make it return a random todo",
// functions
"Not implemented",
// mini_process
"Make easier to understand",
// test_priority
"",
//sh
"",
//mem
"",
//ps
"",
//loop
"",
//kill
"",
//nice
"",
//block
"",
//cat
"",
//wc
"",
//filther
"",
//phylo
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

static uint64_t readCommand(char *buff);
static int interpret(char *command);

extern int phylo_main(int argc, char *argv[]);

void shell();

void startNanoShell(){
	char* argv[] = {""};
	pid_t pid = (pid_t)createProcess(&shell, 1, argv);
}

// add new command here
void shell()
{
    char cmdBuff[CMD_MAX_CHARS + 1] = {0};
    int exit = 0;
    while (!exit)
    {
        fdprintf(STDMARK, PROMPT);
        int command_length = readLine(cmdBuff, CMD_MAX_CHARS);

        int interpretation = interpret(cmdBuff);
		
		TimeStamp ts = {0};

        char toPrint[100];
        int i = 0;
		int j = 0;


		char* argv[] = {""};

		pid_t pid;

        switch (interpretation)
        {
        case HELP:
            printf(help_text);
            break;

        uint64_t regs[17];          /* buffer para los registros   */
        getRegisters(regs);         /* <<- ahora la llamada es correcta */
        /* acá los imprimís como quieras */
        printRegs(regs);            /* tu función de salida        */
        break;

        case TIME:
            printCurrentTime();
            break;

        case ELIMINATOR:
            eliminator();
            break;

        case ECHO:
            while (cmdBuff[i] && cmdBuff[i] != ' ' && cmdBuff != '\t')
            {
                i++;
            }
            i++;
            for (j = 0; cmdBuff[i]; i++, j++)
            {
                toPrint[j] = cmdBuff[i];
            }
            toPrint[j] = 0;
            printf(toPrint);
            break;
            
        case CLEAR:
            clearScreen();
            break;

        case CHANGE_FONT:
            changeFontSize();
            break;

        case NANO_SONG:
            syscall(33,0,0,0);
            //nanoAnthem();
            clearScreen();
            break;

        case TEST_ZERO_DIVISION:
            testZeroDivision();
            break;

        case TEST_INVALID_OPCODE:
            testInvalidOpcode();
            break;

		case TEST_MALLOC:
			printf("%d", testMalloc());
			break;

		case MAN:
			while (cmdBuff[i] && cmdBuff[i] != ' ' && cmdBuff != '\t')
            {
                i++;
            }
            i++;
            for (j = 0; cmdBuff[i]; i++, j++)
            {
                toPrint[j] = cmdBuff[i];
            }
            toPrint[j] = 0;
			interpretation = interpret(toPrint);
			if(interpretation == -1){
				printf("man expects a command passed as argument");
				break;
			}
			printf("%s\nTODO: %s", man[interpretation], todo[interpretation]);
			break;

		case TODO:
    		syscall(6, &ts, 0, 0);
			while(todo[ts.seconds%INSTRUCTION_COUNT] == ""){
				ts.seconds++;
			}
			printf("%d\n", ts.seconds);
			printf("%s\n", todo[ts.seconds%INSTRUCTION_COUNT]);
			break;

		case FUNCTIONS:
			printf("%s\n", functions);
			break;

		case MINI_PROCESS:;
			pid = (pid_t)createProcess(&startProcess, 1, argv);
			break;
		
		case TEST_PRIORITY:
			;
			pid_t pid1, pid2;
			pid1 = (pid_t)createProcess(&testFunc, 1, argv);
			printf("priorities: %d: %d, ", pid1, getPriority(pid1));
			setPriority(pid1, 1);
			printf("priorities: %d: %d, ", pid1, getPriority(pid1));
			break;
		/*
		case TEST_SEMAPHORE:;
			pid_t bloqueado, liberador;
			bloqueado = (pid_t)createProcess(&bloqueadoFunc, 1, argv);
			liberador = (pid_t)createProcess(&liberadorFunc, 1, argv);
			break;

		case TEST_PIPE:;
			pid_t habla, escucha;
			habla = (pid_t)createProcess(&hablaFunc, 1, argv);
			escucha = (pid_t)createProcess(&escuchaFunc, 1, argv);
			break;*/

		case SH:;
			
			break;

	

		case PS:;
	
			break;

		case LOOP:;
			int fd[2];
			pipe_open("loopPipe", fd);
			pid_t pid = (pid_t)createProcess(&loopFunc, 1, argv);
			char bufLoop[100];
			unsigned_num_to_str(pid, 0, bufLoop);
			pipe_write(fd[1], bufLoop, 10);
			
			while (cmdBuff[i] && cmdBuff[i] != ' ' && cmdBuff != '\t')
            {
                i++;
            }
            i++;
            for (j = 0; cmdBuff[i]; i++, j++)
            {
                toPrint[j] = cmdBuff[i];
            }
            toPrint[j] = 0;
			pipe_write(fd[1], toPrint, 10);
			break;

		case KILL:;
			while (cmdBuff[i] && cmdBuff[i] != ' ' && cmdBuff != '\t')
            {
                i++;
            }
            i++;
            for (j = 0; cmdBuff[i]; i++, j++)
            {
                toPrint[j] = cmdBuff[i];
            }
            toPrint[j] = 0;
			uint64_t itKill = 0;
			uint32_t pidKill = unsigned_str_to_num(&itKill, 100, toPrint);
			killProcess(pidKill);
			break;

		case NICE:;
			while (cmdBuff[i] && cmdBuff[i] != ' ' && cmdBuff != '\t')
            {
                i++;
            }
            i++;
            for (j = 0; cmdBuff[i]; i++, j++)
            {
                toPrint[j] = cmdBuff[i];
            }
            toPrint[j] = 0;
			char newPriority[100];
			while (cmdBuff[i] && cmdBuff[i] != ' ' && cmdBuff != '\t')
            {
                i++;
            }
            i++;
            for (j = 0; cmdBuff[i]; i++, j++)
            {
                newPriority[j] = cmdBuff[i];
            }
            newPriority[j] = 0;
			uint64_t itNice = 0;
			uint32_t pidNice = unsigned_str_to_num(&itNice, 100, toPrint);
			uint32_t priorityNice = unsigned_str_to_num(&itNice, 100, newPriority);
			setPriority(pidNice, priorityNice);	
			break;

		case BLOCK:;
			while (cmdBuff[i] && cmdBuff[i] != ' ' && cmdBuff != '\t')
            {
                i++;
            }
            i++;
            for (j = 0; cmdBuff[i]; i++, j++)
            {
                toPrint[j] = cmdBuff[i];
            }
            toPrint[j] = 0;
			uint64_t it = 0;
			uint32_t pidBlock = unsigned_str_to_num(&it, 100, toPrint);
			blockProcess(pidBlock); // doesnt work
			break;

		case CAT:;
			 int c;
			/* leemos hasta Enter (simple) */
			while ((c = getc()) != '\n')      // getc() bloquea hasta que llega algo
				putChar(c);
			putChar('\n');
			break;

		case WC:;
			int lines = 0;
			while(*cmdBuff != '\n'){
				if (*cmdBuff == '\n'){
					lines++;
				}
			}
			printf("lineas: %d\n", lines);
			break;

		case FILTER:;
			while(*cmdBuff != '\n'){
				if (!strchr("aeiouAEIOU", *cmdBuff)){
				    putChar(*cmdBuff);
				}
			}
			putChar('\n');
			break;
        
        case MEM: ;                         /* ← el punto y coma evita el error del label */
        {
            MemStatus ms;
            if (memStatus(&ms) == 0) {
                printf("Memory status\n");
                printf("  Total: %u KiB\n", ms.total / 1024);
                printf("  Used : %u KiB\n", ms.used  / 1024);
                printf("  Free : %u KiB\n", ms.free  / 1024);
            } else {
            printf("mem: syscall failed\n");
        }
}
break;

		case PHYLO: {
    int N = 5;

    char *p = cmdBuff;
    while (*p && *p != ' ' && *p != '\t') ++p; /* salta "phylo" */
    if (*p) {                                 /* hay algo más */
        N = unsigned_str_to_num((uint64_t[]){0}, 3, p+1);
    }

    /* armamos argv para el proceso hijo */
    char nbuf[4];          /* cabe "20\0" */
    unsigned_num_to_str(N, 0, nbuf);
    char *args[] = { nbuf, NULL };

    createProcess(phylo_main, 2, args);
    break;
}

        case -1:
            printf("Command not found: '%s'", cmdBuff);
            break;
        }

        if (interpretation != CHANGE_FONT && interpretation != ELIMINATOR && interpretation != CLEAR && interpretation != NANO_SONG)
        {
            printf("\n");
        }
    }
}

static int interpret(char *command)
{
	printf("instructions[0]: %s\n", instructions[0]);
    char actualCommand[CMD_MAX_CHARS] = {0};
    int i;
    for (i = 0; i < CMD_MAX_CHARS && command[i] != 0 && command[i] != ' ' && command[i] != '\t'; i++)
    {
        actualCommand[i] = command[i];
        toMinus(actualCommand);
    }
    if (i == CMD_MAX_CHARS && command[i] != 0)
        return -1;
    for (i = 0; instructions[i]!=0; i++)
    {
        if (strcmp(actualCommand, instructions[i]) == 0)
            return i;
    }
    return -1;
}