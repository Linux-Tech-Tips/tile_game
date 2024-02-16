#include "title.h"

nextTask_t title_task(programData_t * data) {

    /* Title Task Initialization */
    nextTask_t result;
    short titleRun = 1;


    /* Title Task Loop */
    while(titleRun && data->run) {

        /* Starting frame time measurement */
        data_frameStart(data);


        /* --- Update Component --- */

        /* Reading keyboard input - maximum of 4 characters a frame (no more precision required for the Title task) */
        char readBuffer [4] = {0};
        short keyIn = nbRead(readBuffer, 4);

        /* Processing keyboard input */
        if(keyIn) {

            if(strchr(readBuffer, 'q') != NULL) {
                result = TASK_EXIT;
                titleRun = 0;
            }

        }


        /* --- Render Component --- */

        erase();
        cursorHome();
        printf("Current delta time is: %lf\n", data->deltaTime);
        printf("Current FPS is: %.2f\n", (float)(1.0/data->deltaTime));

        cursorMoveBy(RIGHT, 20);
        puts("TETRGS Title Screen");
        cursorMoveBy(RIGHT, 20);
        puts(" - Press 'q' to exit");
        //printf(" - Read characters from stdin: %s\n", readBuffer);
        printf("\nCharacters read from stdin (as numbers): ");
        for(int i = 0; i < 4; i++) {
            printf("(%d) ", (int)readBuffer[i]);
        }
        puts(";");

        
        /* Ending frame time measurement, sleeping to match desired updates per second */
        data_frameEnd(data, 10);
    }


    /* Title Task Termination */
    return result;
}