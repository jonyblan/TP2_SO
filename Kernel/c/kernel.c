#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <keyboardDriver.h>
#include <idtLoader.h>
#include <nano.h>


extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

void testProcessA(int a,int b/*, int c, int d*/) {
	while (1) {
		vdPrintDec(a);vdPrintDec(b);//vdPrintDec(c);vdPrintDec(d);
		vdPrintChar('\n');
		sleep(10);
	}
}

void testProcessB() {
	while (1) {
		vdPrint("Process B running");
		vdPrintChar('\n');
		sleep(10);
	}
}
void testProcessC() {
	while (1) {
		vdPrint("Process C running");
		vdPrintChar('\n');
		sleep(10);
	}
}

static void *const sampleCodeModuleAddress = (void *)0x400000;
static void *const sampleDataModuleAddress = (void *)0x500000;

typedef int (*EntryPoint)();

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
	
	initScheduler(getStackBase());

	char *argv[] = {"1","2"/*, "3", "4"*/};
	//createFirstProcess((void*)sampleCodeModuleAddress, 0, argv);
	createProcess((void*)testProcessA, 1, 2, argv);
	setTickFrequency(120);


	((EntryPoint)sampleCodeModuleAddress)();

	return 0;
}
