#include "task_game.h"

nextTask_t game_task(programData_t * data) {

    /* Game Task Initialization */
    gameData_t gameData = {0};
    gameData.gameRun = 1;

    gameData.gameState = GAME_RUN;

    /* TODO Field origin will be defined based on the terminal size, once dynamic UI implemented */
    gameData.fieldOriginX = 16;
    gameData.fieldOriginY = 0;

    gameData.defaultFallDelay = 0.5f;
    gameData.fallDelay = gameData.defaultFallDelay;
    gameData.fallTimer = gameData.fallDelay;

    gameData.placeDelay = 0.8f;
    gameData.placeTimer = gameData.placeDelay;

    srand(time(NULL));

    /* Generating the initial block */
    gameData.falling = 1;
    block_initBag(&gameData.blockBag);
    _game_genBlock(&gameData, GAME_NEW_BLOCK_X, GAME_NEW_BLOCK_Y);

    /* Game Task Loop */
    while(gameData.gameRun && data->run) {

        /* Starting frame time management */
        data_frameStart(data);

        /* Main update and render functions */
        game_update(data, &gameData);
        game_render(*data, gameData);

        /* Ending frame time management, sleeping to keep desired updates per second */
        data_frameEnd(data, TASK_GAME_UPS);
    }

    /* Game Task Termination */
    return gameData.nextTask;
}


void game_update(programData_t * data, gameData_t * gameData) {

    /* Clearing the previous frame's keyboard input */
    gameData->keyBuffer[0] = 0;
    gameData->keyBuffer[1] = 0;
    gameData->keyBuffer[2] = 0;
    /* Reading keyboard input - up to TASK_GAME_KEYS characters per frame */
    gameData->keyIn = nbRead(gameData->keyBuffer, TASK_GAME_KEYS);

    /* Updating state-specific logic based on current state */
    switch(gameData->gameState) {
        case GAME_RUN:
            game_updateRun(data, gameData);
        break;

        case GAME_PAUSED:
            game_updatePaused(data, gameData);
        break;

        case GAME_OVER:
            game_updateOver(data, gameData);
        break;
    }

}

void game_updateRun(programData_t * data, gameData_t * gameData) {

    /* --- Keyboard Processing --- */

    /* NOTE: Keyboard processing expects only one key to be pressed in a frame, and processes said key from buffer[0] */
    if(gameData->keyIn) {

        /* Moving block on arrow keys */
        if(gameData->keyBuffer[0] == (char)(27) && gameData->keyBuffer[1] == (char)(91)) {
            short moved = 0;
            /* Moving block in desired direction */
            if(gameData->keyBuffer[2] == (char)(65)) {
                game_rotateBlock(&gameData->block, gameData->field);
                moved = 1;
            } else if(gameData->keyBuffer[2] == (char)(67)) {
                game_moveBlock(&gameData->block, 1, 0, gameData->field, 0);
                moved = 1;
            } else if(gameData->keyBuffer[2] == (char)(68)) {
                game_moveBlock(&gameData->block, -1, 0, gameData->field, 0);
                moved = 1;
            } else if(gameData->keyBuffer[2] == (char)(66)) {
                short dropped = game_moveBlock(&gameData->block, 0, 1, gameData->field, 0);
                moved = 1;
                if(dropped)
                    gameData->score += 1 + 5 * ((gameData->defaultFallDelay / gameData->fallDelay) - 1);
            }

            /* Slow down place timer if block moved in any direction */
            if(moved) {
                if(!gameData->falling && gameData->placeTimer > 0) {
                    gameData->placeTimer += 0.5f * data->deltaTime;
                }
            }
        
        /* Processing paused state for next frame */
        } else if(gameData->keyBuffer[0] == 'p' || (gameData->keyBuffer[0] == (char)(27) && gameData->keyBuffer[1] == 0)) {
            gameData->gameState = GAME_PAUSED;
        }
    }


    /* --- Gameplay Logic --- */

    /* Check whether the block is in falling state or being placed state */
    if(gameData->falling) {
        /* Decrement fall timer and proceed with fall */
        if(gameData->fallTimer > 0) {
            gameData->fallTimer -= data->deltaTime;
        } else {
            gameData->fallTimer = gameData->fallDelay;
            /* Update falling state based on the result of fall */
            gameData->falling = game_moveBlock(&gameData->block, 0, 1, gameData->field, 0);
        }
    } else if(!(gameData->falling = game_moveBlock(&gameData->block, 0, 1, gameData->field, 1))) {
        /* Decrement place timer */
        if(gameData->placeTimer > 0) {
            gameData->placeTimer -= data->deltaTime;
        } else {
            /* Place down block, reset falling block */
            if(!_game_placeBlock(gameData)) {
                gameData->gameState = GAME_OVER;
            }
            _game_genBlock(gameData, GAME_NEW_BLOCK_X, GAME_NEW_BLOCK_Y);
            gameData->placeTimer = gameData->placeDelay;
            gameData->falling = 0;
        }
    }

    /* Clearing lines and adding score */
    int lineClears = 0;
    for(int y = (FIELD_Y-1); y >= 0; y--) {

        /* Checking whether row cleared */
        int rowCleared = 1;
        for(int x = 0; x < FIELD_X; x++) {
            if(gameData->field[x][y] <= 0) {
                rowCleared = 0;
                break;
            }
        }

        /* Moving processed line downwards by the number of lines cleared previously */
        if(lineClears > 0) {
            for(int x = 0; x < FIELD_X; x++) {
                if(y+lineClears < FIELD_Y) {
                    gameData->field[x][y+lineClears] = gameData->field[x][y];
                }
            }
        }

        /* Incrementing the number of line clears if the current row was cleared */
        lineClears += rowCleared;
    }

    /* Adding score if lines cleared in the frame */
    if(lineClears > 0) {
        gameData->score += 100 * (2*lineClears - 1);
        gameData->fallDelay *= 0.95f - 0.025f * (lineClears - 1);
    }
}

void game_updatePaused(programData_t * data, gameData_t * gameData) {
    
    if(gameData->keyIn) {
        /* Unpause logic */
        if(gameData->keyBuffer[0] == 'p' || (gameData->keyBuffer[0] == (char)(27) && gameData->keyBuffer[1] == 0)) {
            gameData->gameState = GAME_RUN;
        }
        
        /* Quit logic */
        if(gameData->keyBuffer[0] == 'q') {
            gameData->nextTask = TASK_TITLE;
            gameData->gameRun = 0;
        }
    }

}

void game_updateOver(programData_t * data, gameData_t * gameData) {
    if(gameData->keyIn) {
        /* Quit logic */
        if(gameData->keyBuffer[0] == 'q') {
            gameData->nextTask = TASK_TITLE;
            gameData->gameRun = 0;
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
    printf("SCORE: %d\n", gameData.score);
    printf("dt: %lf\n", data.deltaTime);

    /* Drawing the game playing area field */
    cursorHome();
    cursorMoveBy(RIGHT, gameData.fieldOriginX);
    cursorMoveBy(DOWN, gameData.fieldOriginY);
    modeSet(NO_CODE, COLOR_WHITE, COLOR_BLACK);
    puts("|====================|");
    for(short y = 0; y < FIELD_Y; y++) {
        cursorMoveBy(RIGHT, gameData.fieldOriginX);
        modeSet(NO_CODE, COLOR_WHITE, COLOR_BLACK);
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
        modeSet(NO_CODE, COLOR_WHITE, COLOR_BLACK);
        putchar('|');
        putchar('\n');
    }
    cursorMoveBy(RIGHT, gameData.fieldOriginX);
    puts("|====================|");

    /* Drawing the block (with the origin incremented by 1, which is the origin of the internal part of the field) 
        NOTE: Here, fieldOriginX is offset by 1 because although the traditional tile has a width of 2, the vertical borders only have a width of 1 */
    block_render(gameData.block, gameData.fieldOriginX+1, gameData.fieldOriginY+1);

    /* Drawing stuff specific to current state */
    switch(gameData.gameState) {
        case GAME_RUN:
            game_renderRun(data, gameData);
        break;

        case GAME_PAUSED:
            game_renderPaused(data, gameData);
        break;

        case GAME_OVER:
            game_renderOver(data, gameData);
        break;
    }

    modeReset();

    /* Flushing STDOUT at the end of render part of loop to make sure everything renders */
    fflush(stdout);
}

void game_renderRun(programData_t data, gameData_t gameData) {
    /* TODO Put something here if any specific draw calls found */
}

void game_renderPaused(programData_t data, gameData_t gameData) {

    /* Rendering pause menu somewhere approx in the field */    
    modeSet(NO_CODE, COLOR_BLACK, COLOR_WHITE);
    cursorHome();
    cursorMoveBy(RIGHT, 24);
    cursorMoveBy(DOWN, 8);
    puts("GAME PAUSED");
    cursorMoveBy(RIGHT, 24);
    puts(" - Press ESC or 'p' to continue");
    cursorMoveBy(RIGHT, 24);
    puts(" - Press 'q' to quit");
}

void game_renderOver(programData_t data, gameData_t gameData) {
    
    /* Rendering game over dialog */
    modeSet(NO_CODE, COLOR_BLACK, COLOR_WHITE);
    cursorHome();
    cursorMoveBy(RIGHT, 24);
    cursorMoveBy(DOWN, 8);
    puts("GAME OVER");
    cursorMoveBy(RIGHT, 24);
    puts(" - Press q to quit");

}


short game_moveBlock(block_t * block, int x, int y, char field [FIELD_X][FIELD_Y], short dryRun) {

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
    /* Overwriting the old tiles array with the newly created newTiles array (if desired) */
    if(!dryRun) {
        for(short i = 0; i < 4; i++) {
            block->tiles[i][0] = newTiles[i][0];
            block->tiles[i][1] = newTiles[i][1];
        }
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

void _game_genBlock(gameData_t * data, int startX, int startY) {
    /* Returning a newly generated block of type determined by the bag */
    block_gen(&data->block, block_getNext(&data->blockBag), startX, startY);
}

short _game_placeBlock(gameData_t * data) {
    short result = 1;
    for(short i = 0; i < 4; i++) {
        int blockX = data->block.tiles[i][0];
        int blockY = data->block.tiles[i][1];
        if((blockX >= 0 && blockX < FIELD_X) && (blockY >= 0 && blockY < FIELD_Y) && (data->field[blockX][blockY] < 1))
            data->field[blockX][blockY] = data->block.type;
        else
            result = 0;
    }
    return result;
}