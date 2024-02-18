#include "task_game.h"

nextTask_t game_task(programData_t * data) {

    /* Game Task Initialization */
    gameData_t gameData = {0};
    gameData.gameRun = 1;
    gameData.fieldOriginX = 16;
    gameData.fieldOriginY = 0;

    gameData.field[0][19] = BLOCK_CUBE;
    gameData.field[0][18] = BLOCK_CUBE;
    gameData.field[1][19] = BLOCK_CUBE;
    gameData.field[1][18] = BLOCK_CUBE;

    gameData.field[5][5] = BLOCK_T;

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
    cursorMoveBy(DOWN, gameData.fieldOriginY);
    modeSet(NO_CODE, COLOR_WHITE, COLOR_BLACK);
    puts("|====================|");
    for(short y = 0; y < FIELD_Y; y++) {
        cursorMoveBy(RIGHT, gameData.fieldOriginX);
        putchar('|');
        for(short x = 0; x < FIELD_X; x++) {
            if(gameData.field[x][y] > 0) {
                modeSet(NO_CODE, COLOR_BLACK, gameData.field[x][y]);
                fputs("[]", stdout);
            } else {
                modeSet(NO_CODE, COLOR_WHITE, COLOR_BLACK);
                fputs("  ", stdout);
            }
        }
        putchar('|');
        putchar('\n');
    }
    cursorMoveBy(RIGHT, gameData.fieldOriginX);
    puts("|====================|");

    /* Drawing the block (with the origin incremented by 1 tile, which is the origin of the internal part of the field) */
    block_render(gameData.block, gameData.fieldOriginX+2, gameData.fieldOriginY+1);

    /* Flushing STDOUT at the end of render part of loop to make sure everything renders */
    fflush(stdout);
}