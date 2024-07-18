#include "task_options.h"

nextTask_t options_task(programData_t * data) {

    optionsData_t optData = {0};
    options_init(&optData, *data);

    while(optData.optRun && data->run) {

        /* Starting frame time management */
        data_frameStart(data);

        /* Update and render functions */
        options_update(data, &optData);
        options_render(*data, optData);
                
        /* Resetting screen clear flag if set */
        if(optData.screenClear)
            optData.screenClear = 0;

        /* Ending frame time management, sleeping to keep desired updates per second */
        data_frameEnd(data, TASK_OPTIONS_UPS);
    }

    nextTask_t result = optData.nextTask;
    options_destroy(&optData, data);
    
    return result;
}

void options_init(optionsData_t * data, programData_t programData) {
    /* Misc data */
    data->optRun = 1;
    data->nextTask = TASK_TITLE;
    
    /* GUI dialogs */
    data->activeDialog = DIALOG_ALIGN_MENU_X;

    /* Horizontal alignment menu */
    gui_createDialog_opt(&data->dialogs[DIALOG_ALIGN_MENU_X], "GAME: HORIZONTAL ALIGNMENT", COLOR_WHITE, COLOR_RED, "", NO_CODE,
                            COLOR_BLUE, COLOR_CYAN, COLOR_RED, COLOR_MAGENTA,
                            BUTTON_LINE, GUI_SIZE_AUTO, GUI_SIZE_AUTO);
    data->alignMenuXLeft = gui_addButton(&data->dialogs[DIALOG_ALIGN_MENU_X], " [ ]   LEFT ");
    data->alignMenuXCenter = gui_addButton(&data->dialogs[DIALOG_ALIGN_MENU_X], " [ ] CENTER ");
    data->alignMenuXRight = gui_addButton(&data->dialogs[DIALOG_ALIGN_MENU_X], " [ ]  RIGHT ");
    /* Setting active button from data */
    switch(programData.alignment.alignX) {
        case ALIGN_LEFT:
            data->dialogs[DIALOG_ALIGN_MENU_X].currentButton = data->alignMenuXLeft;
        break;
        case ALIGN_CENTER:
            data->dialogs[DIALOG_ALIGN_MENU_X].currentButton = data->alignMenuXCenter;
        break;
        case ALIGN_RIGHT:
            data->dialogs[DIALOG_ALIGN_MENU_X].currentButton = data->alignMenuXRight;
        break;
    }

    /* Vertical alignment menu */
    gui_createDialog_opt(&data->dialogs[DIALOG_ALIGN_MENU_Y], "GAME: VERTICAL ALIGNMENT", COLOR_WHITE, COLOR_RED, "", NO_CODE,
                            COLOR_BLUE, COLOR_CYAN, COLOR_RED, COLOR_MAGENTA,
                            BUTTON_LINE, GUI_SIZE_AUTO, GUI_SIZE_AUTO);
    data->alignMenuYTop = gui_addButton(&data->dialogs[DIALOG_ALIGN_MENU_Y], " [ ]    TOP ");
    data->alignMenuYCenter = gui_addButton(&data->dialogs[DIALOG_ALIGN_MENU_Y], " [ ] CENTER ");
    data->alignMenuYBottom = gui_addButton(&data->dialogs[DIALOG_ALIGN_MENU_Y], " [ ] BOTTOM ");
    /* Setting active button from data */
    switch(programData.alignment.alignY) {
        case ALIGN_TOP:
            data->dialogs[DIALOG_ALIGN_MENU_Y].currentButton = data->alignMenuYTop;
        break;
        case ALIGN_CENTER:
            data->dialogs[DIALOG_ALIGN_MENU_Y].currentButton = data->alignMenuYCenter;
        break;
        case ALIGN_BOTTOM:
            data->dialogs[DIALOG_ALIGN_MENU_Y].currentButton = data->alignMenuYBottom;
        break;
    }

    /* FPS Counter menu */
    gui_createDialog_opt(&data->dialogs[DIALOG_FPS_COUNTER], "FPS COUNTER", COLOR_WHITE, COLOR_RED, "", NO_CODE,
                            COLOR_BLUE, COLOR_CYAN, COLOR_RED, COLOR_MAGENTA,
                            BUTTON_LINE, GUI_SIZE_AUTO, GUI_SIZE_AUTO);
    data->fpsMenuYes = gui_addButton(&data->dialogs[DIALOG_FPS_COUNTER], " [ ]    YES ");
    data->fpsMenuNo = gui_addButton(&data->dialogs[DIALOG_FPS_COUNTER], " [ ]     NO ");
    /* Setting active button from data */
    data->dialogs[DIALOG_FPS_COUNTER].currentButton = (programData.fpsCounter ? data->fpsMenuYes : data->fpsMenuNo);

    /* Data Reset menu */
    gui_createDialog_opt(&data->dialogs[DIALOG_DATA_RESET], "USER DATA", COLOR_WHITE, COLOR_RED, "", NO_CODE,
                            COLOR_MAGENTA, COLOR_RED, COLOR_WHITE, COLOR_CYAN,
                            BUTTON_LINE, GUI_SIZE_AUTO, GUI_SIZE_AUTO);
    data->resetDataButton = gui_addButton(&data->dialogs[DIALOG_DATA_RESET], "RESET USER DATA");

    /* Back button */
    gui_createDialog_opt(&data->dialogs[DIALOG_BUTTON_BACK], "", NO_CODE, NO_CODE, "", NO_CODE,
                            COLOR_BLUE, COLOR_CYAN, COLOR_RED, COLOR_BLACK,
                            BUTTON_LINE, GUI_SIZE_AUTO, GUI_SIZE_AUTO);
    data->backButton = gui_addButton(&data->dialogs[DIALOG_BUTTON_BACK], "=== BACK TO TITLE ===");

    getTerminalSize(&data->termX, &data->termY);
    data->screenClear = 1;
}

void options_destroy(optionsData_t * data, programData_t * programData) {
    /* Apply selected dialog button options */
    /* DIALOG_ALIGN_MENU_X */
    int currentButton = data->dialogs[DIALOG_ALIGN_MENU_X].currentButton;
    if(currentButton == data->alignMenuXLeft)
        programData->alignment.alignX = ALIGN_LEFT;
    else if(currentButton == data->alignMenuXCenter)
        programData->alignment.alignX = ALIGN_CENTER;
    else if(currentButton == data->alignMenuXRight)
        programData->alignment.alignX = ALIGN_RIGHT;
    /* DIALOG_ALIGN_MENU_Y */
    currentButton = data->dialogs[DIALOG_ALIGN_MENU_Y].currentButton;
    if(currentButton == data->alignMenuYTop)
        programData->alignment.alignY = ALIGN_TOP;
    else if(currentButton == data->alignMenuYCenter)
        programData->alignment.alignY = ALIGN_CENTER;
    else if(currentButton == data->alignMenuYBottom)
        programData->alignment.alignY = ALIGN_BOTTOM;
    /* DIALOG_FPS_COUNTER */
    currentButton = data->dialogs[DIALOG_FPS_COUNTER].currentButton;
    if(currentButton == data->fpsMenuYes)
        programData->fpsCounter = 1;
    else if(currentButton == data->fpsMenuNo)
        programData->fpsCounter = 0;
    
    /* Disposing of dialogs */
    for(int i = 0; i < OPTIONS_NUM_DIALOGS; ++i)
        gui_destroyDialog(&data->dialogs[i]);
}

void options_update(programData_t * data, optionsData_t * optData) {

    /* Validating terminal size */
    optData->validTerminal = data_validTerm();

    /* Skipping the rest of update if the terminal is invalid - logic not needed */
    if(!optData->validTerminal) {
        optData->screenClear = 1;
        return;
    }

    /* Getting the current frame terminal size */
    int newTermX, newTermY;
    getTerminalSize(&newTermX, &newTermY);

    /* Clearing screen and updating sizes on resize */
    if(newTermX != optData->termX || newTermY != optData->termY) {
        optData->screenClear = 1;
        optData->termX = newTermX;
        optData->termY = newTermY;
    }

    /* Process keyboard input */
    char keyBuffer [TASK_OPTIONS_KEYS] = {0};
    optData->keyIn = nbRead(keyBuffer, TASK_OPTIONS_KEYS);
    keys_processBuffer(keyBuffer, TASK_OPTIONS_KEYS, &optData->keys);

    /* Process active dialog switching (including cycle to the other end of the dialogs) */
    if(optData->keys.KEY_ARROW_UP) {
        if(optData->activeDialog == DIALOG_ALIGN_MENU_X)
            optData->activeDialog = DIALOG_BUTTON_BACK;
        else
            --(optData->activeDialog);
    } else if((optData->keys.KEY_ARROW_DOWN || optData->keys.KEY_TAB)) {
        if(optData->activeDialog == DIALOG_BUTTON_BACK)
            optData->activeDialog = DIALOG_ALIGN_MENU_X;
        else
            ++(optData->activeDialog);
    }

    /* Process ESC to exit task */
    if(optData->keys.KEY_ESC) {
        optData->nextTask = TASK_TITLE;
        optData->optRun = 0;
    }

    /* Update currently active dialog */
    int button = gui_update((optData->dialogs + optData->activeDialog), optData->keys);

    /* Process dialog button presses */
    if(button >= 0) {

        switch(optData->activeDialog) {
            /* Horizontal align */
            case DIALOG_ALIGN_MENU_X:
                if(button == optData->alignMenuXLeft)
                    data->alignment.alignX = ALIGN_LEFT;
                else if(button == optData->alignMenuXCenter)
                    data->alignment.alignX = ALIGN_CENTER;
                else if(button == optData->alignMenuXRight)
                    data->alignment.alignX = ALIGN_RIGHT;
            break;
            
            /* Vertical align */
            case DIALOG_ALIGN_MENU_Y:
                if(button == optData->alignMenuYTop)
                    data->alignment.alignY = ALIGN_TOP;
                else if(button == optData->alignMenuYCenter)
                    data->alignment.alignY = ALIGN_CENTER;
                else if(button == optData->alignMenuYBottom)
                    data->alignment.alignY = ALIGN_BOTTOM;
            break;
            
            /* FPS counter */
            case DIALOG_FPS_COUNTER:
                if(button == optData->fpsMenuYes)
                    data->fpsCounter = 1;
                else if(button == optData->fpsMenuNo)
                    data->fpsCounter = 0;
            break;

            /* Data reset */
            case DIALOG_DATA_RESET:
                if(button == optData->resetDataButton)
                    data_reset(data);
            break;

            /* Back button */
            case DIALOG_BUTTON_BACK:
                if(button == optData->backButton) {
                    optData->nextTask = TASK_TITLE;
                    optData->optRun = 0;
                }
            break;
        }
    }
}

void options_render(programData_t data, optionsData_t optData) {

    /* Reset style and cursor */
    modeReset();
    cursorHome();
    /* Clear screen if requested */
    if(optData.screenClear)
        erase();

    /* Printing invalid information and skipping the rest if invalid */
    if(!optData.validTerminal) {
        modeSet(STYLE_BOLD, COLOR_WHITE, COLOR_RED);
        cursorMoveTo(2, 2);
        printf("UNSUPPORTED TERMINAL SIZE (MINIMUM %dx%d)\nPLEASE RESIZE TO CONTINUE", TERM_MIN_X, TERM_MIN_Y);
        return;
    }

    /* Render dialogs */
    int posY = 3;
    for(int i = 0; i < OPTIONS_NUM_DIALOGS; ++i) {
        /* Render dialog to the center of screen */
        gui_dialog_t dialog = optData.dialogs[i];
        int posX = util_center(dialog.realWidth, optData.termX);
        gui_render(dialog, posX, posY, (optData.activeDialog == i));

        /* Drawing selected marker to desired buttons */
        if(i < DIALOG_DATA_RESET) {
            cursorMoveTo(posX+3 + (13 * dialog.currentButton), posY+2);
            modeSet(STYLE_BOLD, COLOR_WHITE, (optData.activeDialog == i ? COLOR_BLUE : COLOR_CYAN));
            putchar('*');
        }

        /* Adjusting vertical position for the next dialog */
        posY += dialog.realHeight + 1;
    }

    /* Print top of the border */
    modeSet(NO_CODE, COLOR_WHITE, COLOR_CYAN);
    cursorMoveTo(1, 1);
    for(int x = 0; x < optData.termX; ++x) {
        putchar('#');
    }

    /* Printing sides of the border */
    for(int y = 0; y < optData.termY; ++y) {
        cursorMoveTo(1, y);
        putchar('#');
        cursorMoveTo(optData.termX, y);
        putchar('#');
    }

    /* Print bottom of the border */
    cursorMoveTo(1, optData.termY);
    for(int x = 0; x < optData.termX; ++x) {
        putchar('#');
    }

    /* Flushing STDOUT to make sure everything renders */
    fflush(stdout);
}
