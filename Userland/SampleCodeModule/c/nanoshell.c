#include <nanoshell.h>
#include <standardlib.h>
#include <videolib.h>
#include <processes.h>
#include <stddef.h>
#include <memStatus.h>
#include <philosopher.h>

#define CMD_MAX_CHARS 100
#define CMD_NAME_MAX_CHARS 100
#define PROMPT "NanoShell $> "

void loopFunc(int argc, char* argv[]);
	
// add new command or useful here
static char *instructions[] = {"help", "registers", "time", "echo", "clear", "change_font", "test_zero_division", \
"test_invalid_opcode", "test_malloc", "functions", "mini_process", "test_priority", "sh", "mem", "ps", "loop", "kill", "nice", "block", "cat", \
"wc", "filther", "phylo", \
/*useful*/ "malloc", "realloc", "calloc", "free", "createProcess", "getPriority", "setPriority", 0,};

// add new command here
static char *help_text =  " \n\- help --> Help display with all commands\n\
- registers --> Displays the lastest backup of registers\n\
- time --> Displays time and date\n\
- echo [string] --> Prints the [string] argument in the display\n\
- clear --> clears the display\n\
- change_font --> Changes the current font\n\
- test_zero_division --> Test for the Zero Division exception\n\
- test_invalid_opcode --> Test for the Invalid Opcode exception\n\
- test_malloc --> starts the malloc test\n\
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
- phylo --> " ;


static uint64_t readCommand(char *buff);
static int interpret(char *command);

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

        //uint64_t regs[17];          /* buffer para los registros   */
        //getRegisters(regs);         /* <<- ahora la llamada es correcta */
        /* acá los imprimís como quieras */
        //printRegs(regs);            /* tu función de salida        */
        //break;

        case TIME:
            printCurrentTime();
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

        case TEST_ZERO_DIVISION:
            testZeroDivision();
            break;

        case TEST_INVALID_OPCODE:
            testInvalidOpcode();
            break;

		case TEST_MALLOC:
			printf("%d", testMalloc());
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
            MemStatus ms;
            if (memStatus(&ms) == 0) {
                printf("Memory status\n");
                printf("  Total: %u KiB\n", ms.total / 1024);
                printf("  Used : %u KiB\n", ms.used  / 1024);
                printf("  Free : %u KiB\n", ms.free  / 1024);
            } else {
            printf("mem: syscall failed\n");
            }   
            break;
        case PHYLO: ;
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

            createProcess(phylo, 2, args);
            break;
        

        case -1:
            printf("Command not found: '%s'", cmdBuff);
            break;
        }

        if (interpretation != CHANGE_FONT && interpretation != ELIMINATOR && interpretation != CLEAR)
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