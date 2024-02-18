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

    gameData.field[3][5] = BLOCK_T;

    /* Generating and moving demonstration block for this version of the game */
    block_gen(&gameData.block, BLOCK_T, 5, 5);

    /* Game Task Loop */
    while(gameData.gameRun && data->run) {

        /* Starting frame time management */
        data_frameStart(data);

        game_render(*data, gameData);
        game_update(data, &gameData);


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
                game_rotateBlock(&gameData->block, gameData->field);
            else if(buffer[2] == (char)(67))
                game_moveBlock(&gameData->block, 1, 0, gameData->field);
            else if(buffer[2] == (char)(68))
                game_moveBlock(&gameData->block, -1, 0, gameData->field);
            else if(buffer[2] == (char)(66))
                game_moveBlock(&gameData->block, 0, 1, gameData->field);
        } else if(strchr(buffer, ' ')) {
            game_moveBlock(&gameData->block, 0, -1, gameData->field);
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

    /* Drawing the block (with the origin incremented by 1, which is the origin of the internal part of the field) 
        NOTE: Here, fieldOriginX is offset by 1 because although the traditional tile has a width of 2, the vertical borders only have a width of 1 */
    block_render(gameData.block, gameData.fieldOriginX+1, gameData.fieldOriginY+1);

    /* Flushing STDOUT at the end of render part of loop to make sure everything renders */
    fflush(stdout);
}

short game_moveBlock(block_t * block, int x, int y, char field [FIELD_X][FIELD_Y]) {

    int newTiles [4][2];

    /* Populating the array of new tiles and checking for collisions */
    for(short i = 0; i < 4; i++) {
        /* Moving i-th new tile */
        newTiles[i][0] = block->tiles[i][0]; 
        newTiles[i][1] = block->tiles[i][1];
        block_moveTile(newTiles[i], x, y);

        /* Collision checking */
        if(_game_collideTile(newTiles[i][0], newTiles[i][1], field))
            return 0;
    }

    /* If this point in code reached, no collisions detected, which means the block doesn't collide */
    /* Overwriting the old tiles array with the newly created newTiles array */
    for(short i = 0; i < 4; i++) {
        block->tiles[i][0] = newTiles[i][0];
        block->tiles[i][1] = newTiles[i][1];
    }
    /* Returning successful move */
    return 1;
}

short game_rotateBlock(block_t * block, char field [FIELD_X][FIELD_Y]) {

    int newTiles[4][2];
    /* Copying the origin point */
    newTiles[0][0] = block->tiles[0][0];
    newTiles[0][1] = block->tiles[0][1];

    for(short i = 1; i < 4; i++) {
        /* Rotating i-th non-origin tile */
        newTiles[i][0] = block->tiles[i][0];
        newTiles[i][1] = block->tiles[i][1];
        block_rotateTile(newTiles[i], newTiles[0][0], newTiles[0][1]);

        /* Checking tile collisions */
        if(_game_collideTile(newTiles[i][0], newTiles[i][1], field))
            return 0;
    }

    /* If this point reached, no collisions detected, so the old tiles array is overwritten by the new array */
    for(short i = 1; i < 4; i++) {
        block->tiles[i][0] = newTiles[i][0];
        block->tiles[i][1] = newTiles[i][1];
    }

    /* Returning successful rotate */
    return 1;
}

short _game_collideTile(int tileX, int tileY, char field[FIELD_X][FIELD_Y]) {
    /* Checking out-of-bounds first, then if within bounds, the position in the field is checked 
        NOTE: the top bound is not checked, as blocks can fall from however high they like */
    return (tileX < 0 || tileX >= FIELD_X || tileY >= FIELD_Y) || (tileY >= 0 && field[tileX][tileY] > 0);
}