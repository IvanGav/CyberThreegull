#include "CyberThreegull.h"
#include "Terminal/Terminal.h"

void mainCRTStartup() {
	if (!drill_lib_init()) {
		ExitProcess(EXIT_FAILURE);
	}

	terminalsInit();
	openTerminal(0);
	typeChar(Win32::Key::KEY_K, 'k');
	typeChar(Win32::Key::KEY_Y, 'y');
	typeChar(Win32::Key::KEY_S, 's');
	//typeChar(Win32::Key::KEY_LEFT, '1');
	typeChar(Win32::Key::KEY_RETURN, '1');
	typeChar(Win32::Key::KEY_UP, '1');
	typeChar(Win32::Key::KEY_S, 's');
	typeChar(Win32::Key::KEY_LEFT, '1');
	typeChar(Win32::Key::KEY_LEFT, '1');
	typeChar(Win32::Key::KEY_LEFT, '1');
	typeChar(Win32::Key::KEY_DELETE, '1');
	print(fileToStrA(getTerminal()));

	//CyberThreegull::run_cyber_seaquell();
	ExitProcess(EXIT_SUCCESS);
}