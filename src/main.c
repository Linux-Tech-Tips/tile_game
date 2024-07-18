/** 
 * @file main.c 
 * @author Martin (Linux-Tech-Tips)
 * @brief the main file containing the main function 
*/

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include "terminal_gui/terminal_f.h"

#include "data.h"
#include "task_title.h"
#include "task_game.h"
#include "task_options.h"

/** The global static variable used to transfer data from a signal handler to the program. 
 * Initialized to a NULL pointer, optimally to be set to the address of a variable used in the program.
*/
static short * signalRun = NULL;

/** Function to handle received SIGINT signal to allow for graceful program termination */
void signalHandler(int sigID);

/* Main function */
int main(int argc, char * argv []) {

    /* Minimum terminal size check */
    int x = -1, y = -1;
    getTerminalSize(&x, &y);
    if(x < TERM_MIN_X || y < TERM_MIN_Y) {
        fprintf(stderr, "Unsupported Terminal Size: %dx%d (minimum 48x24 required)\n", x, y);
        return 1;
    }

    /* Attaching a signal handler for a graceful exit on receiving SIGINT */
    struct sigaction signalControl = {0};
    signalControl.sa_handler = signalHandler;
    sigaction(SIGINT, &signalControl, NULL);

    /* Seeding pseudo-random number generator */
    srand(time(NULL));

    /* Task-independent data declaration and initialization */
    programData_t data = {0};
    data_load(&data, SAVE_FILE_NAME);
    /* Attaching the data.run program data variable to the static global signal processing variable */
    signalRun = &(data.run);

    /* Terminal GUI initialization */
    screenSave();
    cursorHide();
    erase();
    startKeys();

    /* Main program runtime component - switching tasks, with own data and time keeping, based on what the program is supposed to be doing */
    nextTask_t nextTask = TASK_TITLE;
    while(data.run) {
        switch(nextTask) {
            /* Title screen task */
            case TASK_TITLE:
                nextTask = title_task(&data);
            break;

            /* Game task */
            case TASK_GAME:
                nextTask = game_task(&data);
            break;

            /* Options task */
            case TASK_OPTIONS:
                nextTask = options_task(&data);
            break;

            /* Graceful exit program procedure */
            case TASK_EXIT:
                data.run = 0;
            break;

            /* Default behavior (if no correct nextTask set) - exit program gracefully */
            default:
                nextTask = TASK_EXIT;
        }
    }

    /* Terminal GUI termination */
    endKeys();
    screenRestore();
    cursorShow();

    /* Data termination - saving */
    data_save(data, SAVE_FILE_NAME);

    /* Returning successful exit to shell */
    return 0;
}


/* --- Other function implementations --- */

void signalHandler(int sigID) {
    if(signalRun != NULL) {
        /* Setting the attached run variable to 0 to terminate program gracefully */
        *signalRun = 0;
    } else {
        /* Hard exit if no signal handling run variable attached (pointer is NULL) */
        exit(1);
    }
}