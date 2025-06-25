/* sampleCodeModule.c */
#include <standardlib.h>
#include <nanoshell.h>

char *v = (char *)0xB8000 + 79 * 2;

static int var1 = 0;
static int var2 = 0;

extern void call_80h();

void idleProcess(){
	setPriority(getMyPID(),0);
	while (1)	
	{
		;
	}
	
}

int main()
{	
	char* argv[]= {0};
	createProcess((void*)idleProcess,0,argv,"idle");
	int shellPID;
	shellPID=startNanoShell();
	
	wait(shellPID);
	return 0xDEADBEEF;
}