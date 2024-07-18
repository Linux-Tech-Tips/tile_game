/** 
 * @file task_options.h
 * @author Martin (Linux-Tech-Tips)
 * @brief Header file declaring data structures and functions related to the options task (data.h/TASK_OPTIONS)
 * 
 * This file is responsible for the task responsible for allowing the user to set options, including data structures and functions 
 * for initialization, proper termination, and the update and draw parts of the loop.
*/

#ifndef TASK_OPTIONS_H
#define TASK_OPTIONS_H

#include "data.h"
#include "gui_dialog.h"

/** Updates-per-second rate for the TASK_OPTIONS task */
#define TASK_OPTIONS_UPS 20

/** The number of unique dialogs in the TASK_OPTIONS task */
#define OPTIONS_NUM_DIALOGS 5

/** The number of keyboard inputs read in one update of the TASK_OPTIONS task */
#define TASK_OPTIONS_KEYS 6

/** Enumeration type holding all the Options Task subdialog types */
typedef enum {
    /** Horizontal alignment menu */
    DIALOG_ALIGN_MENU_X,
    /** Vertical alignment menu */
    DIALOG_ALIGN_MENU_Y,
    /** FPS counter menu */
    DIALOG_FPS_COUNTER,
    /** Data reset menu */
    DIALOG_DATA_RESET,
    /** 'Back' button to exit the options task */
    DIALOG_BUTTON_BACK
} options_dialogType_t;

/** Struct holding dialog-related and general data for the Options Task */
typedef struct {

    /** While this is 1 the option task runs */
    short optRun;
    /** The next task to switch to */
    nextTask_t nextTask;

    /** The currently active dialog in the options task */
    options_dialogType_t activeDialog;

    /** All the dialogs in the options task */
    gui_dialog_t dialogs [OPTIONS_NUM_DIALOGS];

    /** The horizontal align 'left' button index */
    int alignMenuXLeft;
    /** The horizontal align 'center' button index */
    int alignMenuXCenter;
    /** The horizontal align 'right' button index */
    int alignMenuXRight;

    /** The vertical align 'top' button index */
    int alignMenuYTop;
    /** The vertical align 'center' button index */
    int alignMenuYCenter;
    /** The vertical align 'bottom' button index */
    int alignMenuYBottom;

    /** The FPS counter menu 'yes' button index */
    int fpsMenuYes;
    /** The FPS counter menu 'no' button index */
    int fpsMenuNo;

    /** The 'RESET DATA' button index */
    int resetDataButton;

    /** The 'back' button index */
    int backButton;

    /** Floating dialog to ask the user to confirm data reset */
    gui_dialog_t resetConfirm;
    /** Floating confirm dialog 'yes' button */
    int resetConfirmYes;
    /** Floating confirm dialog 'no' button */
    int resetConfirmNo;
    /** Whether the floating confirm dialog is actively displayed or not */
    short resetConfirmActive;

    /** The current terminal width */
    int termX;
    /** The current terminal height */
    int termY;
    /** Whether the current terminal is valid */
    short validTerminal;
    /** When this is set, clear the screen in the next frame */
    short screenClear;

    /** Variable storing whether any keyboard input read in the last frame */
    short keyIn;
    /** Variable storing the keyboard inputs processed in the last frame */
    keys_t keys;

} optionsData_t;

/** The main task function for the options task */
nextTask_t options_task(programData_t * data);

/** Initializes a new empty optionsData_t structure, using the options saved in programData */
void options_init(optionsData_t * data, programData_t programData);

/** Disposes of an optionsData_t structure and saves selected settings to programData */
void options_destroy(optionsData_t * data, programData_t * programData);

/** Updates the logic of the options task */
void options_update(programData_t * data, optionsData_t * optData);

/** Draws the content of the options task */
void options_render(programData_t data, optionsData_t optData);

/** Refreshes the currently active buttons to their actual desired state based on the program data, 
 * if 'apply' is set, the button state is instead applied to the settings
*/
void options_refreshButtons(programData_t * data, optionsData_t * optData, short apply);

#endif /* TASK_OPTIONS_H */