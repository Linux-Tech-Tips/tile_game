#include "task_title.h"

nextTask_t title_task(programData_t * data) {

    /* Title Task Initialization */
    nextTask_t result;
    short titleRun = 1;
    short validTerm = 1;

    gui_dialog_t menu;
    gui_createDialog(&menu, "MAIN MENU\nTEST DIALOG", "THIS IS A GUI DIALOG TEST\nTESTING THE DIALOG MODULE", GUI_SIZE_AUTO, GUI_SIZE_AUTO, BUTTON_LINE);

    int b_opt1 = gui_addButton(&menu, "    OPTION 1    \nPRESS TO TRIGGER");
    int b_opt2 = gui_addButton(&menu, "    OPTION 2    ");
    int b_opt3 = gui_addButton(&menu, "    OPTION 3    ");

    short printB1 = 0;

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
            keys_processBuffer(readBuffer, TASK_TITLE_KEYS, &keys);

            if(keys.KEY_Q) {
                result = TASK_EXIT;
                titleRun = 0;
            }

            if(keys.KEY_P && validTerm) {
                result = TASK_GAME;
                titleRun = 0;
            }

            /* Updating dialog */
            int pressed = gui_update(&menu, keys);
            if(pressed == b_opt1) {
                printB1 = !printB1;
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
            gui_render(menu, 10, 10);

            modeReset();
            putchar('\n');
            if(printB1) {
                puts("BUTTON 1 TRIGGER");
            }
            
        } else {
            printf("Your current Terminal size (%dx%d) is unsupported (min %dx%d).\n", x, y, TERM_MIN_X, TERM_MIN_Y);
            puts("Please resize your terminal to continue");
        }

        
        /* Ending frame time measurement, sleeping to match desired updates per second */
        data_frameEnd(data, TASK_TITLE_UPS);
    }


    /* Title Task Termination */
    gui_destroyDialog(&menu);
    return result;
}