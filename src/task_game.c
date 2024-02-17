#include "task_game.h"

nextTask_t game_task(programData_t * data) {

    /* Game Task Initialization */
    gameData_t gameData = {0};
    gameData.gameRun = 1;
    gameData.fieldOriginX = 16;
    gameData.fieldOriginY = 0;

    /* Generating and moving demonstration block for this version of the game */
    game_genBlock(gameData.blockTiles, BLOCK_T);
    game_moveBlock(5, 5, gameData.blockTiles);

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
                game_rotateBlock(gameData->blockTiles);
            else if(buffer[2] == (char)(67))
                game_moveBlock(1, 0, gameData->blockTiles);
            else if(buffer[2] == (char)(68))
                game_moveBlock(-1, 0, gameData->blockTiles);
        }
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
    printf("Block o: %d,%d\n", gameData.blockTiles[0][0], gameData.blockTiles[0][1]);

    /* Drawing the game playing area field */
    cursorHome();
    cursorMoveBy(RIGHT, gameData.fieldOriginX);
    if(gameData.fieldOriginY > 0)
        cursorMoveBy(DOWN, gameData.fieldOriginY);
    modeSet(NO_CODE, FG_WHITE, BG_BLACK);
    puts("|====================|");
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
    puts("|====================|");

    /* Drawing the block */
    for(short i = 0; i < 4; i++) {
        cursorMoveTo(gameData.fieldOriginX, gameData.fieldOriginY);
        if(gameData.blockTiles[i][0] > 0)
            cursorMoveBy(RIGHT, gameData.blockTiles[i][0] * 2);
        if(gameData.blockTiles[i][1] > 0)
            cursorMoveBy(DOWN, gameData.blockTiles[i][1]);
        modeSet(NO_CODE, FG_WHITE, BG_CYAN);
        puts("  ");
    }

    /* Flushing STDOUT at the end of render part of loop to make sure everything renders */
    fflush(stdout);
}

void game_moveBlock(int x, int y, int tiles [4][2]) {
    /* Moving the coordinates of each tile by the specified amount */
    for(short i = 0; i < 4; i++) {
        tiles[i][0] += x;
        tiles[i][1] += y;
    }
}

void game_rotateBlock(int tiles [4][2]) {
    /* Saving the coordinates of the origin into convenient local variables */
    int originX = tiles[0][0], originY = tiles[0][1];
    /* Rotating the remaining coordinates by 90° in respect to the origin */
    for(short i = 1; i < 4; i++) {
        /* Saving the old x and y coordinates into local variables */
        int x = tiles[i][0], y = tiles[i][1];
        /* Rotating using solutions to the matrix equation '(x, y) = ((0, 1), (-1, 0))(x-oX, y-oY) + (oX, oY)' 
           NOTE: The matrix is represented as a list of its columns, the vectors as singular columns */
        tiles[i][0] = -y + originY + originX;
        tiles[i][1] = x - originX + originY;
    }
}

void game_genBlock(int tiles [4][2], blockType_t type) {
    /* TODO STUB TO IMPLEMENT */
    /* For now, the T block is generated */
    /* ORIGIN TILE */
    tiles[0][0] = 0;
    tiles[0][1] = 0;
    /* TILE 2 */
    tiles[1][0] = -1;
    tiles[1][1] = 0;
    /* TILE 3 */
    tiles[2][0] = 1;
    tiles[2][1] = 0;
    /* TILE 4 */
    tiles[3][0] = 0;
    tiles[3][1] = 1;
}