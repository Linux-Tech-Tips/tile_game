/** 
 * @file main.c 
 * @author Martin (Linux-Tech-Tips)
 * @brief the main file containing the main function 
*/

#include <signal.h>
#include "terminal_gui/terminal_f.h"

#include "data.h"


int main() {

    /* Task-independent data declaration and initialization */
    programData_t data = {0};
    data_load(&data, ".game_data");

    /* Terminal GUI initialization */
    screenSave();
    cursorHide();
    erase();
    startKeys();

    /* Main program runtime component - switching tasks, with own data and time keeping, based on what the program is supposed to be doing */
    nextTask_t nextTask = TASK_MAIN;
    while(data.run) {
        switch(nextTask) {
            case TASK_MAIN:
            /* TODO STUB Implement */
            break;

            case TASK_GAME:
            /* TODO STUB Implement */
            break;

            case TASK_EXIT:
                data.run = 0;
            break;

            default:
            /* TODO STUB Implement */
        }
    }

    /* Terminal GUI termination */
    endKeys();
    screenRestore();
    cursorShow();

    /* Data termination - saving */
    data_save(data, ".game_data");

    return 0;

}