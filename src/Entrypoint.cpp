#include "CyberThreegull.h"

void mainCRTStartup() {
	if (!drill_lib_init()) {
		ExitProcess(EXIT_FAILURE);
	}
	CyberThreegull::run_cyber_seaquell();
	ExitProcess(EXIT_SUCCESS);
}