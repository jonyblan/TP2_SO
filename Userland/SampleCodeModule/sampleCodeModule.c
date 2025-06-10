/* sampleCodeModule.c */
//#include <standardlib.h>
#include <nanoshell.h>
#include "../../Kernel/include/processManager.h" //TO DO syscal para crear procesos desde user space
#include "../../Kernel/include/memoryManager.h" //esto era necesario para probar el createProcess
#include <stddef.h>
#include <eliminator.h>

char *v = (char *)0xB8000 + 79 * 2;

static int var1 = 0;
static int var2 = 0;

extern void call_80h();

int main()
{
	startNanoShell();
	//char* argv[]={"nanoshell",NULL};
	//createProcess((void*)startNanoShell,1,1,argv);
	return 0xDEADBEEF;
}