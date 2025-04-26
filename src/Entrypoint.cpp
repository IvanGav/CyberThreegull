#include "CyberThreegull.h"
#include "Terminal/Terminal.h"

void mainCRTStartup() {
	if (!drill_lib_init()) {
		ExitProcess(EXIT_FAILURE);
	}

	terminalsInit();
	openTerminal(0);
	print(fileToStrA(getTerminal()));

	//CyberThreegull::run_cyber_seaquell();
	ExitProcess(EXIT_SUCCESS);
}