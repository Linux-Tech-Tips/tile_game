/** 
 * @file data.h
 * @author Martin (Linux-Tech-Tips)
 * @brief Header file containing declarations related to program data and functions to manipulate said data
 * 
 * This file contains the data structure for program data, as well as functions created to manipulate this program data.
 * The data in question is program data independent of any task, instantiated typically in the main function for the whole program.
*/

#ifndef PROGRAM_DATA_H
#define PROGRAM_DATA_H

/** Minimum terminal width for the game to work */
#define TERM_MIN_X 48
/** Minimum terminal height for the game to work */
#define TERM_MIN_Y 28

/** The name of the file to save data to and load data from, used in main */
#define SAVE_FILE_NAME ".game_data"

#include <time.h>

#include "terminal_gui/terminal_f.h"


/* === Data Structures and Constants === */

/** The various types of alignment on either axis */
typedef enum {
    ALIGN_LEFT,
    ALIGN_RIGHT,
    ALIGN_CENTER,
    ALIGN_TOP,
    ALIGN_BOTTOM
} _alignment_t;

/** The alignment of the game playing field in the terminal */
typedef struct {
    /** Typically expected to be ALIGN_LEFT, ALIGN_RIGHT or ALIGN_CENTER */
    _alignment_t alignX;
    /** Typically expected to be ALIGN_TOP, ALIGN_BOTTOM or ALIGN_CENTER */
    _alignment_t alignY;
} fieldAlign_t;

/** Saved user data */
typedef struct {

    /** The alignment of the game playing field */
    fieldAlign_t alignment;

    /** Whether the FPS (Frames Per Second) counter should be printed */
    short fpsCounter;

    /** The highest achieved score */
    int highScore;

} userData_t;

/** The data structure containing task-independent general program data */
typedef struct {

    /** Whether the program is running (1) or should terminate (0) */
    short run;

    /** The saved user data */
    userData_t userData;

    /** The time at any previous point of measurement */
    struct timespec prevTime;
    /** The time between the last frame and the current one */
    double deltaTime;

    /** The current terminal dimensions */
    int termX, termY;
    /** Whether the terminal was resized in the last frame */
    short termResized;

} programData_t;

/** Enumeration defining the type of task to switch to */
typedef enum {
    TASK_TITLE,
    TASK_GAME,
    TASK_OPTIONS,
    TASK_EXIT
} nextTask_t;


/* === Functions === */

/** Loads a programData_t structure from a file with the given name, or sets default values if file not found */
void data_load(programData_t * data, char const * fileName);

/** Saves a programData_t structure into a file with the given name */
void data_save(programData_t data, char const * fileName);

/** Resets a programData_t structure to default values, including saved user data */
void data_reset(programData_t * data);

/** Initializes the unsaved data of a programData_t structure to initial values */
void data_init(programData_t * data);


/** Starts frame time tracking, to be called at the start of frame */
void data_frameStart(programData_t * data);

/** Ends frame time tracking, updates data->deltaTime and sleeps to match the desired ups 
 * @param ups the desired updates-per-second value of the program
*/
void data_frameEnd(programData_t * data, int ups);

/** Utility function to convert the C library timespec structure into precise seconds, saved as a double */
double data_timeToSec(struct timespec time);

/** Utility function to validate the current terminal size against the required MIN size 
 * @returns whether the current terminal is valid (1) or not (0)
*/
short data_validTerm(void);

/** Updates the currently saved terminal size, returns whether the terminal was resized in the last frame */
short data_termSize(programData_t * data);

#endif /* PROGRAM_DATA_H */