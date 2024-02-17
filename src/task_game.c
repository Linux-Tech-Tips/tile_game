#include "task_game.h"

nextTask_t game_task(programData_t * data) {

    /* Game Task Initialization */
    gameData_t gameData = {0};
    gameData.gameRun = 1;

    /* Game Task Loop */
    while(gameData.gameRun && data->run) {

        /* Starting frame time management */
        data_frameStart(data);

        game_update(data, &gameData);

        game_render(*data, gameData);

        /* Ending frame time management, sleeping to keep desired updates per second */
        data_frameEnd(data, 60);
    }



    /* Game Task Termination */
    return gameData.nextTask;
}

void game_update(programData_t * data, gameData_t * gameData) {

    /* Reading keyboard input - up to 3 characters per frame */
    char buffer [3];
    short keyIn = nbRead(buffer, 3);

    if(keyIn) {
        if(strchr(buffer, 'q')) {
            gameData->nextTask = TASK_TITLE;
            gameData->gameRun = 0;
        }
    }

}

void game_render(programData_t data, gameData_t gameData) {

    /* TODO STUB Implement TETRGS Game */

    erase();
    cursorHome();
    puts("GAME SCREEN lol");
    printf("Delta time is %lf\n", data.deltaTime);

}