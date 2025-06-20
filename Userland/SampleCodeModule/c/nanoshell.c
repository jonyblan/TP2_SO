#include <nanoshell.h>
#include <standardlib.h>
#include <videolib.h>
#include <simpleProcess.h>
#include <stddef.h>
#include <memStatus.h>

#define CMD_MAX_CHARS 1000
#define CMD_NAME_MAX_CHARS 100
#define PROMPT "NanoShell $> "

void startProcess(int argc, char* argv[]);
	
// add new command or useful here
static char *instructions[] = {"help", "registers", "time", "eliminator", "echo", "clear", "change_font", "nano_song", "test_zero_division", \
"test_invalid_opcode", "test_malloc", "man", "todo", "functions", "mini_process",\
/*useful*/ "malloc", "realloc", "calloc", "free", "createProcess", 0,};

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
- mini_process --> creates a new process according to simpleProcess.c\n";

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
return value: free doesnt return anything",
"createProcess:\n"
};

// add new command or useful here
static char *functions = "\
Commands: help, registers, time, eliminator, echo, clear, change_font, nano_song, test_zero_division\n\
test_invalid_opcode, test_malloc, man, todo, functions, mini_process\n\n\n\
Useful: malloc, realloc, calloc, free\n";

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
};

static uint64_t readCommand(char *buff);
static int interpret(char *command);

// add new command here
void startNanoShell()
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

		char* argv[] = {"startProcess\0", NULL};
		int argc = 2;

		pid_t pid;

        switch (interpretation)
        {
        case HELP:
            printf(help_text);
            break;

        case REGISTERS:
            getRegisters();
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
            nanoAnthem();
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

		case MINI_PROCESS:
			printf("string = %s\n", argv[0]);
			printf("process = %s\n", startProcess);
			printf("a\n");
			pid = (uint64_t)createProcess(startProcess);
			printf("after createProcess: %d\n", pid);
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