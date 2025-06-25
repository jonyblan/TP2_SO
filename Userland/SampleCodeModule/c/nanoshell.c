#include <nanoshell.h>
#include <standardlib.h>
#include <videolib.h>
#include <processes.h>
#include <stddef.h>
#include <pipelib.h>

#define CMD_MAX_CHARS 1000
#define CMD_NAME_MAX_CHARS 100
#define PROMPT "NanoShell $> "
#define MAX_ARGS 10

typedef struct {
    char *cmd1;
    char *args1[MAX_ARGS];
    char *cmd2;
    char *args2[MAX_ARGS];
    int hasPipe;
    int isBackground;
} ParsedCommand;

static void (*instructionFunctions[])(uint8_t, char **) = {
    helpFunc,             // "help"
    registersFunc,        // "registers"
    timeFunc,             // "time"
    echoFunc,             // "echo"
    clearFunc,            // "clear"
    testZeroDivisionFunc,     // "test_zero_division"
    testInvalidOpcodeFunc,    // "test_invalid_opcode"
    testMallocFunc,           // "test_malloc"
    todoFunc,             // "todo"
    functionsFunc,        // "functions"
    startProcess,         // "mini_process"
    testPriorityFunc,     // "test_priority"
    testSemaphoreFunc,    // "test_semaphore"
    testPipeFunc,         // "test_pipe"
    memFunc,              // "mem"
    psFunc,               // "ps"
    loopFunc,             // "loop"
    killFunc,             // "kill"
    niceFunc,             // "nice"
    blockFunc,            // "block"
    catFunc,              // "cat"
    wcFunc,               // "wc"
    filterFunc,           // "filther"
    phyloFunc,            // "phylo"
    unblockFunc,          // "unblock"
    NULL,
};

// add new command or useful here
static char *instructions[] = {"help", "registers", "time", "echo", "clear", "test_zero_division", \
"test_invalid_opcode", "test_malloc", "todo", "functions", "mini_process", "test_priority",\
"test_semaphore", "test_pipe", "mem", "ps", "loop", "kill", "nice", "block", "cat", "wc", \
"filther", "phylo", "unblock",\
/*useful*/ "malloc", "realloc", "calloc", "free", "createProcess", "getPriority", "setPriority", 0,};



static uint64_t readCommand(char *buff);
static int interpret(char *command);
static pid_t fgProccess=NULL;
static int hasToWait = 1;
static int pipeCounter=0;

void shell();

int startNanoShell(){
	char* argv[] = {""};
	pid_t pid = (pid_t)createProcess((void*)&shell, 1, argv,"shell");
    return pid;
}

int countArgs(char **argv) {
    int i = 0;
    while (argv[i])
        i++;
    return i;
}

void parseInput(char *input, ParsedCommand *out) {
    out->hasPipe = 0;
    out->isBackground = 0;
    memset(out->args1, 0, sizeof(out->args1));
    memset(out->args2, 0, sizeof(out->args2)); 

    int len = strlen(input);
    if (len > 0 && input[len - 1] == '&') {
        out->isBackground = 1;
        input[len - 1] = 0;
    }

    char *pipePos = strchr(input, '|');
    if (pipePos) {
        out->hasPipe = 1;
        *pipePos = 0;
        pipePos++;
    }

    int i = 0;
    char *tok = strtok(input, " \t");
    while (tok && i < MAX_ARGS - 1) {
        out->args1[i++] = tok;
        tok = strtok(NULL, " \t");
    }
    out->cmd1 = out->args1[0];

    if (out->hasPipe) {
        i = 0;
        tok = strtok(pipePos, " \t");
        while (tok && i < MAX_ARGS - 1) {
            out->args2[i++] = tok;
            tok = strtok(NULL, " \t");
        }
        out->cmd2 = out->args2[0];
    }
}

void (*getFn(const char *cmd))(uint8_t, char **) {
    for (int i = 0; instructions[i] != NULL; i++) {
        if (strcmp(instructions[i], cmd) == 0) {
            return instructionFunctions[i];
        }
    }
    return NULL;
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

        ParsedCommand parsed = {0};
        parseInput(cmdBuff,&parsed);

        if (parsed.cmd1 == NULL)
        {
            continue;
        }
        
        hasToWait= !parsed.isBackground;

        
		
        if (parsed.hasPipe) {
            char* name;
            unsigned_num_to_str(pipeCounter,0,name);
            uint8_t anonPipe=pipe_open(name);
            pipeCounter++;
            
            
            void (*fn1)(uint8_t, char **) = getFn(parsed.cmd1);
            void (*fn2)(uint8_t, char **) = getFn(parsed.cmd2);

            if (!fn1 || !fn2) {
                printf("Unknown command in pipe: %s | %s\n", parsed.cmd1, parsed.cmd2);
                continue;
            }

            pid_t p1 = createProcess(fn1, countArgs(parsed.args1), parsed.args1,parsed.cmd1);
            changeProcessFd(p1,anonPipe,1);

            pid_t p2 = createProcess(fn2, countArgs(parsed.args2), parsed.args2, parsed.cmd2);
            changeProcessFd(p2,anonPipe,0);

            if (!parsed.isBackground) {
                fgProccess = p2;
            } else {
                fgProccess = NULL;
            }

            continue;
        }

        void (*fn)(uint8_t, char **) = getFn(parsed.cmd1);

        if ( fn == NULL) {
            printf("Command not found: '%s'\n", parsed.cmd1);
            fgProccess = NULL;
            continue;
        }

        int interpretation = interpret(parsed.cmd1);
        pid_t pid = createProcess(fn, countArgs(parsed.args1), parsed.args1,parsed.cmd1);
        if (!parsed.isBackground)
            fgProccess = pid;

        if (interpretation != CLEAR)
        {
            printf("\n");
        }
        if (hasToWait && fgProccess != NULL)
        {
            wait(fgProccess);
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


        /* switch (interpretation)
        {
        /* case HELP:
            printf(help_text);
			printf(help_text2);
            break; */

        /* case REGISTERS:
            getRegisters();
            break; */

        /* case TIME:
            printCurrentTime();
            break; */

        /* case ECHO:
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
            break; */
            
        /* case CLEAR:
            clearScreen();
            break; */

        /* case TEST_ZERO_DIVISION:
            testZeroDivision();
            break;

        case TEST_INVALID_OPCODE:
            testInvalidOpcode();
            break; */

		/* case TEST_MALLOC:
			printf("%d", testMalloc());
			break; */

		/* case TODO:
    		syscall(6, &ts, 0, 0);
			while(todo[ts.seconds%INSTRUCTION_COUNT] == ""){
				ts.seconds++;
			}
			printf("%d\n", ts.seconds);
			printf("%s\n", todo[ts.seconds%INSTRUCTION_COUNT]);
			break;
 */
		/* case FUNCTIONS:
			printf("%s\n", functions);
			break; */

		/* case MINI_PROCESS:
			pid = (pid_t)createProcess(&startProcess, 1, argv);
			break; */
		
		/* case TEST_PRIORITY:
			;
			pid_t pid1, pid2, pid3;
			pid1 = (pid_t)createProcess(&testFunc, 1, argv);
			pid2 = (pid_t)createProcess(&testFunc, 2, argv);
			pid3 = (pid_t)createProcess(&testFunc, 3, argv);
			printf("priorities: %d: %d, %d: %d, %d: %d\n\n", pid1, getPriority(pid1), pid2, getPriority(pid2), pid3, getPriority(pid3));
			setPriority(pid1, 1);
			setPriority(pid2, 7);
			setPriority(pid3, 7);
			printf("priorities: %d: %d, %d: %d, %d: %d\n\n", pid1, getPriority(pid1), pid2, getPriority(pid2), pid3, getPriority(pid3));
			break; */

		/* case TEST_SEMAPHORE:;
			pid_t bloqueado, liberador;
			bloqueado = (pid_t)createProcess(&bloqueadoFunc, 1, argv);
			liberador = (pid_t)createProcess(&liberadorFunc, 1, argv);
			break;
 */
		/* case TEST_PIPE:;
			pid_t habla, escucha;
			escucha = (pid_t)createProcess(&escuchaFunc, 1, argv);
			habla = (pid_t)createProcess(&hablaFunc, 1, argv);
			setPriority(habla, 5);
			break; */

		/* case MEM:;
	
			break;

		case PS:;
	
			break;

		case LOOP:;
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
            createProcess(&loopFunc, 1, toPrint);
			break; */

		/* case KILL:;
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
			break; */

		/* case NICE:;
			while (cmdBuff[i] && cmdBuff[i] != ' ' && cmdBuff != '\t')
            {
                i++;
            }
            i++;
            for (j = 0; cmdBuff[i] != ' '; i++, j++)
            {
                toPrint[j] = cmdBuff[i];
            }
            j++;
            toPrint[j] = 0;
            char newPriority[100];
            i++;
            for (j = 0; cmdBuff[i] && j < 10; i++, j++)
            {
                newPriority[j] = cmdBuff[i];
            }
            newPriority[j] = 0;
            uint64_t itNice = 0;
            uint64_t itNice2 = 0;
            uint32_t pidNice = unsigned_str_to_num(&itNice, 1, toPrint);
            uint32_t priorityNice = unsigned_str_to_num(&itNice2, 1, newPriority);
            printf("PID nice: %d, new priority: %d\n", pidNice, priorityNice);
            setPriority(pidNice, priorityNice);
			break; */

		/* case BLOCK:;
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
            blockProcess(pidBlock); 
			break;
 */
		/* case CAT:;
			char actualCommand[CMD_MAX_CHARS] = {0};
            for (i = 0; i < CMD_MAX_CHARS && cmdBuff[i] != 0 && cmdBuff[i] != '\t'; i++)
            {
                actualCommand[i] = cmdBuff[i];
                toMinus(actualCommand);
            }
            actualCommand[i] = 0;
            int c = 4;
            while (actualCommand[c] != 0){
                putChar(actualCommand[c]);
                c++;
            }
			break; */

		/* case WC:; // no tiene sentido pero no tuvimos tiempo de integrarlo con procesos y pipes

            int lines = 1;
            for (i = 0; i < CMD_MAX_CHARS && cmdBuff[i] != 0 && cmdBuff[i] != '\t'; i++)
            {
                if (cmdBuff[i] == '\n'){
                    
                }
            }

            printf("lineas: %d\n", lines);
			break; */

		/* case FILTER:;
			char noVowels[CMD_MAX_CHARS] = {0};
            for (i = 0; i < CMD_MAX_CHARS && cmdBuff[i] != 0 && cmdBuff[i] != '\t'; i++)
            {
                if (!strchr("aeiouAEIOU", cmdBuff[i])){
                    noVowels[j] = cmdBuff[i];
                    toMinus(noVowels);
                    j++;
                }   
            }
            noVowels[j] = 0;
            int k = 6;
            while (noVowels[k] != 0){
                putChar(noVowels[k]);
                k++;
            }
			break; */
/* 
		case PHYLO:;
	
			break; */
			

        /* case -1:
            printf("Command not found: '%s'", cmdBuff);
            fgProccess = NULL;
            break;
        } */
        