#include "Windows.h"

void mainCRTStartup() {
	WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), "Hello World\n", 12, NULL, NULL);
	ExitProcess(EXIT_SUCCESS);
}