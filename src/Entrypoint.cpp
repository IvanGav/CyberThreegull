#include "CyberThreegull.h"
#include "Terminal/Terminal.h"



void mainCRTStartup() {
    if (!drill_lib_init()) {
        ExitProcess(EXIT_FAILURE);
    }

    //runVirtualTerminal(); // Run the virtual terminal
    CyberThreegull::run_cyber_threequell();

    ExitProcess(EXIT_SUCCESS);
}
