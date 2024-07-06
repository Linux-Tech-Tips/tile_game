#include "task_title.h"

nextTask_t title_task(programData_t * data) {

    /* Title Task Initialization */
    nextTask_t result;
    short titleRun = 1;
    short validTerm = 1;

    /* Title Task Loop */
    while(titleRun && data->run) {

        /* Starting frame time measurement */
        data_frameStart(data);


        /* --- Update Component --- */

        /* Reading keyboard input - maximum of 4 characters a frame (no more precision required for the Title task) */
        char readBuffer [TASK_TITLE_KEYS] = {0};
        short keyIn = nbRead(readBuffer, TASK_TITLE_KEYS);

        /* Validating terminal size */
        int x = -1, y = -1;
        getTerminalSize(&x, &y);
        if(x < TERM_MIN_X || y < TERM_MIN_Y) {
            if(validTerm)
                validTerm = 0;
        } else if(!validTerm) {
            validTerm = 1;
        }

        /* Processing keyboard input */
        if(keyIn) {
            keys_t keys;
            keys_processBuffer(readBuffer, &keys);

            if(keys.KEY_Q) {
                result = TASK_EXIT;
                titleRun = 0;
            }

            if(keys.KEY_P && validTerm) {
                result = TASK_GAME;
                titleRun = 0;
            }

        }


        /* --- Render Component --- */

        modeReset();
        erase();
        cursorHome();
        printf("Current delta time is: %lf\n", data->deltaTime);
        printf("Current FPS is: %.2f\n", (float)(1.0/data->deltaTime));

        if(validTerm) {
            cursorMoveBy(RIGHT, 20);
            puts("TETRGS Title Screen");
            cursorMoveBy(RIGHT, 20);
            puts(" - Press 'p' to play game");
            cursorMoveBy(RIGHT, 20);
            puts(" - Press 'q' to exit");
            //printf(" - Read characters from stdin: %s\n", readBuffer);
            printf("\nCharacters read from stdin (as numbers): ");
            for(int i = 0; i < TASK_TITLE_KEYS; i++) {
                printf("(%d) ", (int)readBuffer[i]);
            }
            puts(";");
        } else {
            printf("Your current Terminal size (%dx%d) is unsupported (min %dx%d).\n", x, y, TERM_MIN_X, TERM_MIN_Y);
            puts("Please resize your terminal to continue");
        }
        
        /* Ending frame time measurement, sleeping to match desired updates per second */
        data_frameEnd(data, TASK_TITLE_UPS);
    }


    /* Title Task Termination */
    return result;
}