#include "task_game.h"

nextTask_t game_task(programData_t * data) {

    /* Game Task Initialization */
    gameData_t gameData = {0};
    gameData.gameRun = 1;
    gameData.fieldOriginX = 16;
    gameData.fieldOriginY = 0;

    /* Generating and moving demonstration block for this version of the game */
    block_gen(&gameData.block, BLOCK_T, 5, 5);

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
        /* Quitting if desired */
        if(strchr(buffer, 'q')) {
            gameData->nextTask = TASK_TITLE;
            gameData->gameRun = 0;
        }

        /* Moving block on arrow keys */
        if(buffer[0] == (char)(27) && buffer[1] == (char)(91)) {
            if(buffer[2] == (char)(65))
                block_rotate(&gameData->block);
            else if(buffer[2] == (char)(67))
                block_move(1, 0, &gameData->block);
            else if(buffer[2] == (char)(68))
                block_move(-1, 0, &gameData->block);
        }

        if(strchr(buffer, '1'))
            block_gen(&gameData->block, BLOCK_T, 5, 5);
        if(strchr(buffer, '2'))
            block_gen(&gameData->block, BLOCK_CUBE, 5, 5);
        if(strchr(buffer, '3'))
            block_gen(&gameData->block, BLOCK_LINE, 5, 5);
        if(strchr(buffer, '4'))
            block_gen(&gameData->block, BLOCK_L, 5, 5);
        if(strchr(buffer, '5'))
            block_gen(&gameData->block, BLOCK_L_R, 5, 5);
        if(strchr(buffer, '6'))
            block_gen(&gameData->block, BLOCK_Z, 5, 5);
        if(strchr(buffer, '7'))
            block_gen(&gameData->block, BLOCK_Z_R, 5, 5);
        
    }

}

void game_render(programData_t data, gameData_t gameData) {

    /* Resetting and erasing screen before drawing new frame */
    modeReset();
    erase();
    cursorHome();

    /* Drawing general debug information (for now) */
    puts("DEMO GAME");
    printf("dt: %lf\n", data.deltaTime);
    printf("Block o: %d,%d\n", gameData.block.tiles[0][0], gameData.block.tiles[0][1]);

    /* Drawing the game playing area field */
    cursorHome();
    cursorMoveBy(RIGHT, gameData.fieldOriginX);
    if(gameData.fieldOriginY > 0)
        cursorMoveBy(DOWN, gameData.fieldOriginY);
    modeSet(NO_CODE, COLOR_WHITE, COLOR_BLACK);
    puts("||||||||||||||||||||||");
    for(short i = 0; i < 20; i++) {
        cursorMoveBy(RIGHT, gameData.fieldOriginX);
        putchar('|');
        for(short j = 0; j < 20; j++) {
            putchar(' ');
        }
        putchar('|');
        putchar('\n');
    }
    cursorMoveBy(RIGHT, gameData.fieldOriginX);
    puts("||||||||||||||||||||||");

    /* Drawing the block */
    block_render(gameData.block, gameData.fieldOriginX, gameData.fieldOriginY);

    /* Flushing STDOUT at the end of render part of loop to make sure everything renders */
    fflush(stdout);
}