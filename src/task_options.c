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

    /* Vertical alignment menu */
    gui_createDialog_opt(&data->dialogs[DIALOG_ALIGN_MENU_Y], "GAME: VERTICAL ALIGNMENT", COLOR_WHITE, COLOR_RED, "", NO_CODE,
                            COLOR_BLUE, COLOR_CYAN, COLOR_RED, COLOR_MAGENTA,
                            BUTTON_LINE, GUI_SIZE_AUTO, GUI_SIZE_AUTO);
    data->alignMenuYTop = gui_addButton(&data->dialogs[DIALOG_ALIGN_MENU_Y], " [ ]    TOP ");
    data->alignMenuYCenter = gui_addButton(&data->dialogs[DIALOG_ALIGN_MENU_Y], " [ ] CENTER ");
    data->alignMenuYBottom = gui_addButton(&data->dialogs[DIALOG_ALIGN_MENU_Y], " [ ] BOTTOM ");

    /* FPS Counter menu */
    gui_createDialog_opt(&data->dialogs[DIALOG_FPS_COUNTER], "FPS COUNTER", COLOR_WHITE, COLOR_RED, "", NO_CODE,
                            COLOR_BLUE, COLOR_CYAN, COLOR_RED, COLOR_MAGENTA,
                            BUTTON_LINE, GUI_SIZE_AUTO, GUI_SIZE_AUTO);
    data->fpsMenuYes = gui_addButton(&data->dialogs[DIALOG_FPS_COUNTER], " [ ]    YES ");
    data->fpsMenuNo = gui_addButton(&data->dialogs[DIALOG_FPS_COUNTER], " [ ]     NO ");

    /* Data Reset menu */
    gui_createDialog_opt(&data->dialogs[DIALOG_DATA_RESET], "USER DATA", COLOR_WHITE, COLOR_RED, "", NO_CODE,
                            COLOR_MAGENTA, COLOR_RED, COLOR_WHITE, COLOR_CYAN,
                            BUTTON_LINE, GUI_SIZE_AUTO, GUI_SIZE_AUTO);
    data->resetDataButton = gui_addButton(&data->dialogs[DIALOG_DATA_RESET], "RESET USER DATA");

    /* Data reset confirm dialog */
    gui_createDialog_opt(&data->resetConfirm, "CONFIRM DATA RESET", COLOR_WHITE, COLOR_RED, 
                            "Are you sure you want to reset all game data?", COLOR_WHITE, COLOR_BLUE, COLOR_CYAN, 
                            COLOR_RED, COLOR_MAGENTA, BUTTON_LINE, GUI_SIZE_AUTO, GUI_SIZE_AUTO);
    data->resetConfirmYes = gui_addButton(&data->resetConfirm, " = YES = ");
    data->resetConfirmNo = gui_addButton(&data->resetConfirm, " = NO = ");

    /* Back button */
    gui_createDialog_opt(&data->dialogs[DIALOG_BUTTON_BACK], "", NO_CODE, NO_CODE, "", NO_CODE,
                            COLOR_BLUE, COLOR_CYAN, COLOR_RED, COLOR_BLACK,
                            BUTTON_LINE, GUI_SIZE_AUTO, GUI_SIZE_AUTO);
    data->backButton = gui_addButton(&data->dialogs[DIALOG_BUTTON_BACK], " === BACK TO TITLE === ");

    /* Setting buttons to correct states based on program data */
    options_refreshButtons(&programData, data, 0);

    getTerminalSize(&data->termX, &data->termY);
    data->screenClear = 1;
}

void options_destroy(optionsData_t * data, programData_t * programData) {
    /* Apply selected dialog button options */
    options_refreshButtons(programData, data, 1);
    
    /* Disposing of dialogs */
    for(int i = 0; i < OPTIONS_NUM_DIALOGS; ++i)
        gui_destroyDialog(&data->dialogs[i]);
    
    /* Disposing of floating confirm dialog */
    gui_destroyDialog(&data->resetConfirm);
}

void options_update(programData_t * data, optionsData_t * optData) {

    /* Updating terminal size info */
    data_termSize(data);

    /* Validating terminal size */
    optData->validTerminal = data_validTerm(*data);

    /* Skipping the rest of update if the terminal is invalid - logic not needed */
    if(!optData->validTerminal) {
        optData->screenClear = 1;
        return;
    }

    /* Asking to clear the screen if terminal resized */
    if(data->termResized)
        optData->screenClear = 1;

    /* Process keyboard input */
    char keyBuffer [TASK_OPTIONS_KEYS] = {0};
    optData->keyIn = nbRead(keyBuffer, TASK_OPTIONS_KEYS);
    keys_processBuffer(keyBuffer, TASK_OPTIONS_KEYS, &optData->keys);

    if(optData->resetConfirmActive) {
        /* Updating floating dialog if active */
        
        /* Exiting from dialog if requested */
        if(optData->keys.KEY_ESC) {
            optData->resetConfirmActive = 0;
            optData->screenClear = 1;
        }

        /* Updating dialog */
        int button = gui_update(&optData->resetConfirm, optData->keys);

        /* Reset data on request, exit otherwise */
        if(button >= 0) {
            if(button == optData->resetConfirmYes) {
                data_reset(data);
                options_refreshButtons(data, optData, 0);
                optData->resetConfirmActive = 0;
                optData->screenClear = 1;
            } else if(button == optData->resetConfirmNo) {
                optData->resetConfirmActive = 0;
                optData->screenClear = 1;
            }
        }

    } else {
        /* Updating rest of options dialog if floating dialog not active */

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
                        (&data->userData)->alignment.alignX = ALIGN_LEFT;
                    else if(button == optData->alignMenuXCenter)
                        (&data->userData)->alignment.alignX = ALIGN_CENTER;
                    else if(button == optData->alignMenuXRight)
                        (&data->userData)->alignment.alignX = ALIGN_RIGHT;
                break;
                
                /* Vertical align */
                case DIALOG_ALIGN_MENU_Y:
                    if(button == optData->alignMenuYTop)
                        (&data->userData)->alignment.alignY = ALIGN_TOP;
                    else if(button == optData->alignMenuYCenter)
                        (&data->userData)->alignment.alignY = ALIGN_CENTER;
                    else if(button == optData->alignMenuYBottom)
                        (&data->userData)->alignment.alignY = ALIGN_BOTTOM;
                break;
                
                /* FPS counter */
                case DIALOG_FPS_COUNTER:
                    if(button == optData->fpsMenuYes)
                        (&data->userData)->fpsCounter = 1;
                    else if(button == optData->fpsMenuNo)
                        (&data->userData)->fpsCounter = 0;
                break;

                /* Data reset */
                case DIALOG_DATA_RESET:
                    if(button == optData->resetDataButton)
                        optData->resetConfirmActive = 1;
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

    /* Render reset confirm dialog and skipping the rest if desired */
    if(optData.resetConfirmActive) {
        /* Render reset confirm dialog */
        int posX = util_center(optData.resetConfirm.realWidth, optData.termX);
        gui_render(optData.resetConfirm, posX, 6, 1);

    } else {
        /* Render normal dialogs */
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

void options_refreshButtons(programData_t * data, optionsData_t * optData, short apply) {

    if(apply) {
        /* Applying buttons to settings */

        /* DIALOG_ALIGN_MENU_X */
        int currentButton = optData->dialogs[DIALOG_ALIGN_MENU_X].currentButton;
        if(currentButton == optData->alignMenuXLeft)
            (&data->userData)->alignment.alignX = ALIGN_LEFT;
        else if(currentButton == optData->alignMenuXCenter)
            (&data->userData)->alignment.alignX = ALIGN_CENTER;
        else if(currentButton == optData->alignMenuXRight)
            (&data->userData)->alignment.alignX = ALIGN_RIGHT;

        /* DIALOG_ALIGN_MENU_Y */
        currentButton = optData->dialogs[DIALOG_ALIGN_MENU_Y].currentButton;
        if(currentButton == optData->alignMenuYTop)
            (&data->userData)->alignment.alignY = ALIGN_TOP;
        else if(currentButton == optData->alignMenuYCenter)
            (&data->userData)->alignment.alignY = ALIGN_CENTER;
        else if(currentButton == optData->alignMenuYBottom)
            (&data->userData)->alignment.alignY = ALIGN_BOTTOM;

        /* DIALOG_FPS_COUNTER */
        currentButton = optData->dialogs[DIALOG_FPS_COUNTER].currentButton;
        if(currentButton == optData->fpsMenuYes)
            (&data->userData)->fpsCounter = 1;
        else if(currentButton == optData->fpsMenuNo)
            (&data->userData)->fpsCounter = 0;

    } else {
        /* Applying settings to buttons */

        /* DIALOG_ALIGN_MENU_X */
        switch(data->userData.alignment.alignX) {
            case ALIGN_LEFT:
                optData->dialogs[DIALOG_ALIGN_MENU_X].currentButton = optData->alignMenuXLeft;
            break;
            case ALIGN_CENTER:
                optData->dialogs[DIALOG_ALIGN_MENU_X].currentButton = optData->alignMenuXCenter;
            break;
            case ALIGN_RIGHT:
                optData->dialogs[DIALOG_ALIGN_MENU_X].currentButton = optData->alignMenuXRight;
            break;
        }

        /* DIALOG_ALIGN_MENU_Y */
        switch(data->userData.alignment.alignY) {
            case ALIGN_TOP:
                optData->dialogs[DIALOG_ALIGN_MENU_Y].currentButton = optData->alignMenuYTop;
            break;
            case ALIGN_CENTER:
                optData->dialogs[DIALOG_ALIGN_MENU_Y].currentButton = optData->alignMenuYCenter;
            break;
            case ALIGN_BOTTOM:
                optData->dialogs[DIALOG_ALIGN_MENU_Y].currentButton = optData->alignMenuYBottom;
            break;
        }

        /* DIALOG_FPS_COUNTER */
        optData->dialogs[DIALOG_FPS_COUNTER].currentButton = (data->userData.fpsCounter ? optData->fpsMenuYes : optData->fpsMenuNo);
    }
}