#include "task_game.h"

nextTask_t game_task(programData_t * data) {

    /* Game Task Initialization */
    gameData_t gameData = {0};
    game_init(&gameData, data->alignment);

    srand(time(NULL));

    /* Game Task Loop */
    while(gameData.gameRun && data->run) {

        /* Starting frame time management */
        data_frameStart(data);

        /* Main update and render functions */
        game_update(data, &gameData);
        game_render(*data, gameData);

        /* Resetting screen clear flag if set */
        if(gameData.screenClear)
            gameData.screenClear = 0;

        /* Ending frame time management, sleeping to keep desired updates per second */
        data_frameEnd(data, TASK_GAME_UPS);
    }

    /* Disposing of the created game task */
    nextTask_t result = gameData.nextTask;
    game_destroy(&gameData);

    /* Game Task Termination */
    return result;
}


void game_init(gameData_t * data, fieldAlign_t alignment) {
    /* Misc data */
    data->gameRun = 1;
    data->nextTask = TASK_TITLE;
    data->gameState = GAME_RUN;
    data->keyIn = 0;

    /* Pause menu*/
    gui_createDialog_opt(&data->pauseMenu, "GAME PAUSED", COLOR_WHITE, COLOR_RED, "", NO_CODE, 
                            COLOR_BLUE, COLOR_CYAN, COLOR_RED, COLOR_MAGENTA, 
                            BUTTON_LINE, GUI_SIZE_AUTO, GUI_SIZE_AUTO);
    data->pauseContinue = gui_addButton(&data->pauseMenu, "CONTINUE");
    data->pauseRestart = gui_addButton(&data->pauseMenu, "RESTART");
    data->pauseQuit = gui_addButton(&data->pauseMenu, "QUIT");
    /* Game over menu */
    gui_createDialog_opt(&data->overMenu, "GAME OVER", COLOR_WHITE, COLOR_RED, 
                            "Thanks For Playing!\n - Final Score:         ", COLOR_WHITE, 
                            COLOR_BLUE, COLOR_CYAN, COLOR_RED, COLOR_MAGENTA,
                            BUTTON_BREAK, GUI_SIZE_AUTO, GUI_SIZE_AUTO);
    data->overRestart = gui_addButton(&data->overMenu, "PLAY AGAIN");
    data->overQuit = gui_addButton(&data->overMenu, "MAIN MENU");

    /* Unmutable gameplay options */
    data->defaultFallDelay = GAME_DEFAULT_FALL_DELAY;
    data->placeDelay = GAME_DEFAULT_PLACE_DELAY;
    
    /* Block bag initialization */
    block_initBag(&data->blockBag);

    /* Reset gameplay data */
    game_reset(data, alignment);
}

void game_reset(gameData_t * data, fieldAlign_t alignment) {
    /* Reset game state and timers */
    data->gameState = GAME_RUN;
    data->score = 0;
    data->fallDelay = data->defaultFallDelay;
    data->fallTimer = data->fallDelay;
    data->placeTimer = data->placeDelay;
    data->falling = 1;
    
    /* Reset game playing field */
    for(int x = 0; x < FIELD_X; ++x) {
        for(int y = 0; y < FIELD_Y; ++y) {
            data->field[x][y] = 0;
        }
    }

    /* Set the game to clear on the next frame */
    data->screenClear = 1;

    /* Reset dialog buttons */
    (&data->pauseMenu)->currentButton = 0;
    (&data->overMenu)->currentButton = 0;

    /* Update field origin position based on the set alignment */
    getTerminalSize(&(data->termX), &(data->termY));
    _game_getAlignPos(alignment, &(data->fieldOriginX), &(data->fieldOriginY), data->termX, data->termY);

    /* Generate newly falling block */
    block_shuffleBag(&data->blockBag);
    _game_genBlock(data, GAME_NEW_BLOCK_X, GAME_NEW_BLOCK_Y);
}

void game_destroy(gameData_t * data) {
    gui_destroyDialog(&data->pauseMenu);
    gui_destroyDialog(&data->overMenu);
}


void game_update(programData_t * data, gameData_t * gameData) {

    /* Getting the current frame terminal size */
    int newTermX, newTermY;
    getTerminalSize(&newTermX, &newTermY);

    /* Clearing screen and updating sizes on resize */
    if(newTermX != gameData->termX || newTermY != gameData->termY) {
        gameData->screenClear = 1;
        gameData->termX = newTermX;
        gameData->termY = newTermY;
        /* Updating field alignment based on new terminal size */
        _game_getAlignPos(data->alignment, &(gameData->fieldOriginX), &(gameData->fieldOriginY), gameData->termX, gameData->termY);
    }

    /* Checking to make sure terminal dimensions are valid */
    if(!data_validTerm()) {
        gameData->gameState = GAME_INVALID;
        gameData->screenClear = 1;
    }

    /* Reading keyboard input - up to TASK_GAME_KEYS characters per frame */
    char keyBuffer [TASK_GAME_KEYS] = {0};
    gameData->keyIn = nbRead(keyBuffer, TASK_GAME_KEYS);
    keys_processBuffer(keyBuffer, TASK_GAME_KEYS, &gameData->keys);

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

        case GAME_INVALID:
            game_updateInvalid(data, gameData);
        break;

        case GAME_RESTART:
            game_reset(gameData, data->alignment);
        break;
    }

}

void game_updateRun(programData_t * data, gameData_t * gameData) {

    /* --- Keyboard Processing --- */

    /* NOTE: Keyboard processing expects only one key to be pressed in a frame, and processes said key from buffer[0] */
    if(gameData->keyIn) {

        short moved = 0;
        if(gameData->keys.KEY_ARROW_UP) {
            game_rotateBlock(&gameData->block, gameData->field);
            moved = 1;
        } else if(gameData->keys.KEY_ARROW_RIGHT) {
            game_moveBlock(&gameData->block, 1, 0, gameData->field, 0);
            moved = 1;
        } else if(gameData->keys.KEY_ARROW_LEFT) {
            game_moveBlock(&gameData->block, -1, 0, gameData->field, 0);
            moved = 1;
        } else if(gameData->keys.KEY_ARROW_DOWN) {
            /* TODO BUGFIX: MAKE BLOCK DROPDOWN FOLLOW DELTA TIME */
            short dropped = game_moveBlock(&gameData->block, 0, 1, gameData->field, 0);
            moved = 1;
            if(dropped)
                gameData->score += 1 + 5 * ((gameData->defaultFallDelay / gameData->fallDelay) - 1);
        }

        if(moved && !gameData->falling && gameData->placeTimer > 0)
            gameData->placeTimer += 0.5f * data->deltaTime;
        
        /* Processing paused state for the next frame */
        if(gameData->keys.KEY_P || gameData->keys.KEY_ESC)
            gameData->gameState = GAME_PAUSED;

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

    /* Checking and updating high score */
    if(gameData->score > data->highScore)
        data->highScore = gameData->score;
}

void game_updatePaused(programData_t * data, gameData_t * gameData) {
    
    if(gameData->keyIn) {
        
        int button = gui_update(&gameData->pauseMenu, gameData->keys);

        /* Process GUI dialog buttons */
        if(button >= 0) {
            if(button == gameData->pauseContinue) {
                /* Continue */
                gameData->gameState = GAME_RUN;
                gameData->screenClear = 1;
            } else if(button == gameData->pauseRestart) {
                /* Restart */
                gameData->gameState = GAME_RESTART;
            } else if(button == gameData->pauseQuit) {
                /* Exit to title */
                gameData->nextTask = TASK_TITLE;
                gameData->gameRun = 0;
            }
        }
    }

}

void game_updateOver(programData_t * data, gameData_t * gameData) {
    if(gameData->keyIn) {

        int button = gui_update(&gameData->overMenu, gameData->keys);

        /* Process GUI buttons */
        if(button >= 0) {
            if(button == gameData->overRestart) {
                /* Play again */
                gameData->gameState = GAME_RESTART;
            } else if(button == gameData->overQuit) {
                /* Exit to title */
                gameData->nextTask = TASK_TITLE;
                gameData->gameRun = 0;
            }
        }
    }
}

void game_updateInvalid(programData_t * data, gameData_t * gameData) {

    if(gameData->keyIn) {
        if(gameData->keys.KEY_Q) {
            gameData->nextTask = TASK_EXIT;
            gameData->gameRun = 0;
        }
    }

    if(data_validTerm()) {
        gameData->gameState = GAME_PAUSED;
        gameData->screenClear = 1;
    }

}


void game_render(programData_t data, gameData_t gameData) {

    /* Resetting mode before drawing new frame */
    modeReset();
    cursorHome();
    
    /* Clearing screen if requested */
    if(gameData.screenClear)
        erase();

    /* Drawing score and high score */
    cursorMoveTo(2, 2);
    printf("SCORE %6d", gameData.score);
    cursorMoveTo(2, 3);
    printf("BEST  %6d", data.highScore);

    /* Drawing FPS if requested */
    if(data.fpsCounter) {
        cursorMoveTo(2, 4);
        printf("FPS: %.2f  ", (float)(1.0f/data.deltaTime));
    }

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

        case GAME_INVALID:
            game_renderInvalid(data, gameData);
        break;
    }

    /* Print top of the border */
    modeSet(NO_CODE, COLOR_WHITE, COLOR_CYAN);
    cursorMoveTo(1, 1);
    for(int x = 0; x < gameData.termX; ++x) {
        putchar('#');
    }

    /* Printing sides of the border */
    for(int y = 0; y < gameData.termY; ++y) {
        cursorMoveTo(1, y);
        putchar('#');
        cursorMoveTo(gameData.termX, y);
        putchar('#');
    }

    /* Print bottom of the border */
    cursorMoveTo(1, gameData.termY);
    for(int x = 0; x < gameData.termX; ++x) {
        putchar('#');
    }

    modeReset();

    /* Flushing STDOUT at the end of render part of loop to make sure everything renders */
    fflush(stdout);
}

void game_renderRun(programData_t data, gameData_t gameData) {
    /* NOTE: Playing field draw calls are specific to the RUN state to not interfere with the GUI */

    /* Getting field origin offset instead of actual position, since cursorMoveBy is used */
    int originXOffset = gameData.fieldOriginX - 1;
    int originYOffset = gameData.fieldOriginY - 1;
    /* Resetting cursor to field origin */
    cursorHome();
    cursorMoveBy(RIGHT, originXOffset);
    cursorMoveBy(DOWN, originYOffset);
    
    /* Drawing top border */
    modeSet(NO_CODE, COLOR_WHITE, COLOR_BLACK);
    putchar('|');
    for(int x = 0; x < FIELD_X; ++x) {
       fputs("==", stdout);
    }
    putchar('|');
    putchar('\n');
    
    /* Drawing field */
    for(short y = 0; y < FIELD_Y; y++) {
        cursorMoveBy(RIGHT, originXOffset);
        modeSet(NO_CODE, COLOR_WHITE, COLOR_BLACK);
        putchar('|');
        for(short x = 0; x < FIELD_X; x++) {
            if(gameData.field[x][y] > 0) {
                modeSet(NO_CODE, COLOR_BLACK, gameData.field[x][y]);
                fputs("[]", stdout);
            } else if(block_testPos(gameData.block, x, y)) {
                cursorMoveBy(RIGHT, 2);
            } else {
                modeSet(NO_CODE, COLOR_WHITE, COLOR_BLACK);
                fputs("  ", stdout);
            }
        }
        modeSet(NO_CODE, COLOR_WHITE, COLOR_BLACK);
        putchar('|');
        putchar('\n');
    }

    /* Drawing bottom border */
    cursorMoveBy(RIGHT, originXOffset);
    putchar('|');
    for(int x = 0; x < FIELD_X; ++x) {
        fputs("==", stdout);
    }
    putchar('|');

    /* Drawing the block */
    block_render(gameData.block, gameData.fieldOriginX, gameData.fieldOriginY);
}

void game_renderPaused(programData_t data, gameData_t gameData) {

    /* Rendering pause menu somewhere approx in the field */    
    int posX = util_center(gameData.pauseMenu.realWidth, gameData.termX);
    gui_render(gameData.pauseMenu, posX, 6, 1);
}

void game_renderOver(programData_t data, gameData_t gameData) {
    
    /* Rendering game over dialog */
    int posX = util_center(gameData.overMenu.realWidth, gameData.termX);
    gui_render(gameData.overMenu, posX, 6, 1);
    cursorMoveTo(posX+17, 9);
    modeSet(STYLE_BOLD, COLOR_WHITE, gameData.overMenu.background);
    printf("%6d", gameData.score);
}

void game_renderInvalid(programData_t data, gameData_t gameData) {

    /* Rendering error dialog */
    modeSet(STYLE_BOLD, COLOR_WHITE, COLOR_RED);
    cursorMoveTo(2, 2);
    printf("The current Terminal size (%dx%d) is invalid (minimum %dx%d).\n", gameData.termX, gameData.termY, TERM_MIN_X, TERM_MIN_Y);
    puts("Resize the terminal to continue the game, or exit using 'q'");
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

void _game_getAlignPos(fieldAlign_t alignment, int * posX, int * posY, int terminalWidth, int terminalHeight) {

    /* Get horizontal alignment-based x position */
    switch(alignment.alignX) {
        case ALIGN_LEFT:
            *posX = 16;
        break;

        case ALIGN_CENTER:
            *posX = util_maxInt(util_center((FIELD_X*2)+2, terminalWidth), FIELD_MIN_LEFT);
        break;

        case ALIGN_RIGHT:
            *posX = util_maxInt(terminalWidth - ((FIELD_X*2)+2) - 16, FIELD_MIN_LEFT);
        break;

        default:
            *posX = 16;
    }

    /* Get vertical alignment-based y position */
    switch(alignment.alignY) {
        case ALIGN_TOP:
            *posY = 3;
        break;

        case ALIGN_CENTER:
            *posY = util_center(FIELD_Y+2, terminalHeight);
        break;

        case ALIGN_BOTTOM:
            *posY = terminalHeight - (FIELD_Y+2) - 3;
            break;
        
        default:
            *posY = 3;
    }
}