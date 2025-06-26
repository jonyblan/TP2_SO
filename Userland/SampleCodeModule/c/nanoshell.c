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
                getInFg(p2);
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
        if (!parsed.isBackground){
            fgProccess = pid;
            getInFg(pid);
        }

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
