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


/* === Data Structures and Constants === */

/** The data structure containing task-independent general program data */
typedef struct {

    /** Whether the program is running (1) or should terminate (0) */
    short run;

    /** The highest achieved score */
    int highScore;

    /* TODO Next up, things like settings should be here as well, possibly key bindings if added later on */

} programData_t;

typedef enum {
    TASK_MAIN,
    TASK_GAME,
    TASK_EXIT
} nextTask_t;


/* === Functions === */

/** Loads a programData_t structure from a file with the given name */
void data_load(programData_t * data, char const * fileName);

/** Saves a programData_t structure into a file with the given name */
void data_save(programData_t data, char const * fileName);


#endif /* PROGRAM_DATA_H */