/** 
 * @file task_title.h
 * @author Martin (Linux-Tech-Tips)
 * @brief Header file declaring data structures and functions related to the main/title screen task (data.h/TASK_MAIN)
*/

#ifndef TASK_TITLE_H
#define TASK_TITLE_H

#include <string.h>
#include "terminal_gui/terminal_f.h"

#include "data.h"
#include "util.h"
#include "gui_dialog.h"

/** Updates-per-second rate for the TASK_TITLE task */
#define TASK_TITLE_UPS 20
/** The number of keyboard inputs read in one update of the TASK_TITLE task */
#define TASK_TITLE_KEYS 4

/** The data structure holding data related to the title task */
typedef struct {

    /** Whether the title task is running */
    short titleRun;
    /** The next task to switch to */
    nextTask_t nextTask;

    /** Whether the current terminal is valid */
    short validTerminal;

    /** The last frame terminal size */
    int termX, termY;
    /** Whether the terminal was resized in the last frame */
    short terminalResized;

    /** Whether the screen should be erased the next frame */
    short screenClear;

    /** The GUI buttons menu */
    gui_dialog_t menu;
    /** The index of the 'play' button within the GUI */
    int bPlay;
    /** The index of the 'options' button within the GUI */
    int bOptions;
    /** The index of the 'quit' button within the GUI */
    int bQuit;

} titleData_t;

/** The main task function for the Title task, consisting of task init, task loop and task termination */
nextTask_t title_task(programData_t * data);

/** Initializes or resets a titleData_t struct */
void title_init(titleData_t * data);

/** Destroys the given titleData_t struct */
void title_destroy(titleData_t * data);

/** Frame update logic for the title task */
void title_update(programData_t * data, titleData_t * titleData);

/** Draw calls for the title task */
void title_render(programData_t data, titleData_t titleData);

#endif /* TASK_TITLE_H */