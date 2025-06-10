#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <keyboardDriver.h>
#include <idtLoader.h>
#include <processManager.h>
#include <videoDriver.h>
#include <nano.h>


extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;
extern void firstContextSwitch(void *newStack);

static const uint64_t PageSize = 0x1000;

static void *const sampleCodeModuleAddress = (void *)0x400000;
static void *const sampleDataModuleAddress = (void *)0x500000;

typedef int (*EntryPoint)();

void testProcessA(int argc, char **argv) {
	 while (1) {
        vdPrint("Process A running");
		vdPrintChar('\n');
        sleep(5);
    }
}

void testProcessB(int argc, char **argv) {
    while (1) {
        vdPrint("Process B running");
		vdPrintChar('\n');
        sleep(5);
    }
}

void nanoFace(){
    for (int i = 0; i < _384_WIDTH * _384_HEIGHT; i++)
		drawRectangle(_384[i], (i % _384_WIDTH) * 4, 4 * (i / _384_WIDTH), (i % _384_WIDTH + 1) * 4, 4 * (i / _384_WIDTH + 1));
}

void clearBSS(void *bssAddress, uint64_t bssSize)
{
	memset(bssAddress, 0, bssSize);
}

void *getStackBase()
{
	return (void *)((uint64_t)&endOfKernel + PageSize * 8 // The size of the stack itself, 32KiB
					- sizeof(uint64_t)					  // Begin at the top of the stack
	);
}

void *initializeKernelBinary()
{
	char buffer[10];

	void *moduleAddresses[] = {
		sampleCodeModuleAddress,
		sampleDataModuleAddress};

	loadModules(&endOfKernelBinary, moduleAddresses);

	clearBSS(&bss, &endOfKernel - &bss);

	return getStackBase();
}

int main()
{
	load_idt();
	
	setTickFrequency(120);

	if(initializeProcesses() == -1){
		vdPrint("Processes failed to initialize");
		vdPrintChar('\n');
		return 1;
	}


	PCB* firstProcess = getNextProcess();
	firstProcess->state = RUNNING;
	currentProcess = firstProcess;

	char *argv[] = { "A", NULL };
	//createProcess(testProcessA, 1, 1, argv);	

	//firstContextSwitch(firstProcess->stackPointer);

	while (1)
	{
		/* code */
	}
	
	//((EntryPoint)sampleCodeModuleAddress)();
	//char *argv[] = { "init", NULL };
	//createProcess(sampleCodeModuleAddress, 0, 1, argv);	
	
	return 0;
}
