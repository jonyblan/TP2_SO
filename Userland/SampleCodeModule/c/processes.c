#include <standardlib.h>

void startProcess(int argc, char* argv[]){
	while(1){
		printf("Simple process running.\nargc: %d\n", argc);
	}
}

void testFunc(int argc, char* argv){
	//while(1){
		printf("Function %d running!\n", argc);
	//}
}