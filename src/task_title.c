#include "task_title.h"

nextTask_t title_task(programData_t * data) {

    /* Title Task Initialization */
    titleData_t titleData = {0};
    title_init(&titleData);

    /* Title Task Loop */
    while(titleData.titleRun && data->run) {

        /* Starting frame time measurement */
        data_frameStart(data);

        title_update(data, &titleData);
        title_render(*data, titleData);
        
        /* Resetting force reload after rendered */
        if(titleData.screenClear)
            titleData.screenClear = 0;
        
        /* Ending frame time measurement, sleeping to match desired updates per second */
        data_frameEnd(data, TASK_TITLE_UPS);
    }


    /* Title Task Termination */
    nextTask_t result = titleData.nextTask;
    title_destroy(&titleData);
    return result;
}

void title_init(titleData_t * data) {
    
    /* Misc data */
    data->titleRun = 1;
    data->nextTask = TASK_EXIT;
    data->validTerminal = 1;
    /* Force screen reload when screen started */
    data->screenClear = 1;

    /* Initializing terminal information */
    getTerminalSize(&data->termX, &data->termY);
    data->terminalResized = 0;
    
    /* GUI creation */
    gui_createDialog_opt(&data->menu, "", NO_CODE, NO_CODE, 
                            "Thank you for playing Tile Game!", COLOR_WHITE,
                            COLOR_BLUE, COLOR_CYAN, COLOR_RED, COLOR_BLACK,
                            BUTTON_BREAK, GUI_SIZE_AUTO, GUI_SIZE_AUTO);
    data->bPlay = gui_addButton(&data->menu, "          === PLAY ===          ");
    data->bOptions = gui_addButton(&data->menu, "         === OPTIONS ===        ");
    data->bQuit = gui_addButton(&data->menu, "          === QUIT ===          ");
}

void title_destroy(titleData_t * data) {
    gui_destroyDialog(&data->menu);
}

void title_update(programData_t * data, titleData_t * titleData) {
    
    /* Validating terminal size */
    titleData->validTerminal = data_validTerm();

    /* Skipping the rest of update if the terminal is invalid - logic not needed */
    if(!titleData->validTerminal) {
        titleData->screenClear = 1;
        return;
    }

    /* Reading and processing keyboard input */
    char readBuffer [TASK_TITLE_KEYS] = {0};
    short keyIn = nbRead(readBuffer, TASK_TITLE_KEYS);

    /* Processing keyboard input */
    if(keyIn) {
        keys_t keys;
        keys_processBuffer(readBuffer, TASK_TITLE_KEYS, &keys);

        if(keys.KEY_Q) {
            titleData->nextTask = TASK_EXIT;
            titleData->titleRun = 0;
        }

        /* Updating dialog */
        int pressed = gui_update(&titleData->menu, keys);
        if(pressed == titleData->bPlay) {
            /* Play game upon request */
            titleData->nextTask = TASK_GAME;
            titleData->titleRun = 0;

        } else if(pressed == titleData->bOptions) {
            titleData->nextTask = TASK_OPTIONS;
            titleData->titleRun = 0;

        } else if(pressed == titleData->bQuit) {
            /* Exit game upon request */
            titleData->nextTask = TASK_EXIT;
            titleData->titleRun = 0;
        }
    }

    /* Process terminal size */
    int newX = 0, newY = 0;
    getTerminalSize(&newX, &newY);
    if(newX != titleData->termX || newY != titleData->termY) {
        titleData->terminalResized = 1;
        titleData->screenClear = 1;
        titleData->termX = newX;
        titleData->termY = newY;
    } else if(titleData->terminalResized) {
        titleData->terminalResized = 0;
    }
}

void title_render(programData_t data, titleData_t titleData) {

    /* Reset cursor to default position and style */
    modeReset();
    cursorHome();

    /* Clearing terminal, but only if reload requested to prevent flickering */
    if(titleData.screenClear)
        erase();
    
    /* Printing invalid information and skipping the rest if invalid */
    if(!titleData.validTerminal) {
        modeSet(STYLE_BOLD, COLOR_WHITE, COLOR_RED);
        cursorMoveTo(2, 2);
        printf("UNSUPPORTED TERMINAL SIZE (MINIMUM %dx%d)\nPLEASE RESIZE TO CONTINUE", TERM_MIN_X, TERM_MIN_Y);
        return;
    }

    /* Print the FPS counter (if desired) */
    if(data.userData.fpsCounter) {
        cursorMoveTo(2, 2);
        printf("FPS: %.2f  ", (float)(1.0f/data.deltaTime));
    }

    /* Print the Title text */
    int posY = 4;
    cursorMoveTo(0, posY);
    modeSet(STYLE_BOLD, COLOR_CYAN, NO_CODE);
    /* Choosing title style based on available terminal width */
    if(titleData.termX < 80) {
        /* Thinner title text */
        int posX = util_center(38, titleData.termX);
        cursorMoveTo(posX, posY++);
        puts("  _|_|_|_|_|  _|  _|      _|_|_|      ");
        cursorMoveTo(posX, posY++);
        puts("      _|      _|  _|      _|          ");
        cursorMoveTo(posX, posY++);
        puts("      _|      _|  _|      _|_|        ");
        cursorMoveTo(posX, posY++);
        puts("      _|      _|  _|      _|          ");
        cursorMoveTo(posX, posY++);
        puts("      _|      _|  _|_|_|  _|_|_|      ");
        cursorMoveTo(posX, posY++);
        puts("    ");
        cursorMoveTo(posX, posY++);
        puts("  _|_|_|    _|_|    _|      _|  _|_|_|");
        cursorMoveTo(posX, posY++);
        puts("_|        _|    _|  _|_|  _|_|  _|    ");
        cursorMoveTo(posX, posY++);
        puts("_|  _|_|  _|_|_|_|  _|  _|  _|  _|_|  ");
        cursorMoveTo(posX, posY++);
        puts("_|    _|  _|    _|  _|      _|  _|    ");
        cursorMoveTo(posX, posY++);
        puts("  _|_|_|  _|    _|  _|      _|  _|_|_|");

    } else {
        /* Wider title text */
        int posX = util_center(74, titleData.termX);
        cursorMoveTo(posX, posY++);
        puts(" _|_|_|_|_|  _|_|_|  _|_|    _|      _|  _|  _|      _|    _|_|    _|    ");
        cursorMoveTo(posX, posY++);
        puts("     _|      _|      _|  _|  _|_|  _|_|  _|  _|_|    _|  _|    _|  _|    ");
        cursorMoveTo(posX, posY++);
        puts("     _|      _|_|    _|_|    _|  _|  _|  _|  _|  _|  _|  _|_|_|_|  _|    ");
        cursorMoveTo(posX, posY++);
        puts("     _|      _|      _|  _|  _|      _|  _|  _|    _|_|  _|    _|  _|    ");
        cursorMoveTo(posX, posY++);
        puts("     _|      _|_|_|  _|  _|  _|      _|  _|  _|      _|  _|    _|  _|_|_|");
        cursorMoveTo(posX, posY++);
        puts("    \n");
        cursorMoveTo(posX, posY++);
        puts("_|_|_|_|_|  _|  _|      _|_|_|        _|_|_|    _|_|    _|      _|  _|_|_|");
        cursorMoveTo(posX, posY++);
        puts("    _|      _|  _|      _|          _|        _|    _|  _|_|  _|_|  _|    ");
        cursorMoveTo(posX, posY++);
        puts("    _|      _|  _|      _|_|        _|  _|_|  _|_|_|_|  _|  _|  _|  _|_|  ");
        cursorMoveTo(posX, posY++);
        puts("    _|      _|  _|      _|          _|    _|  _|    _|  _|      _|  _|    ");
        cursorMoveTo(posX, posY++);
        puts("    _|      _|  _|_|_|  _|_|_|        _|_|_|  _|    _|  _|      _|  _|_|_|");

    }

    /* Print GUI under */
    int guiX = util_center(titleData.menu.realWidth, titleData.termX);
    gui_render(titleData.menu, guiX, posY, 1);


    /* Print top of the border */
    modeSet(NO_CODE, COLOR_WHITE, COLOR_CYAN);
    cursorMoveTo(1, 1);
    for(int x = 0; x < titleData.termX; ++x) {
        putchar('#');
    }

    /* Printing sides of the border */
    for(int y = 0; y < titleData.termY; ++y) {
        cursorMoveTo(1, y);
        putchar('#');
        cursorMoveTo(titleData.termX, y);
        putchar('#');
    }

    /* Print bottom of the border */
    cursorMoveTo(1, titleData.termY);
    for(int x = 0; x < titleData.termX; ++x) {
        putchar('#');
    }

}