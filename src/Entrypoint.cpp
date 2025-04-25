#include "CyberSeaquell.h"

void mainCRTStartup() {
	if (!drill_lib_init()) {
		ExitProcess(EXIT_FAILURE);
	}
	CyberSeaquell::run_cyber_seaquell();
	ExitProcess(EXIT_SUCCESS);
}