/** 
 * @file task_game.h
 * @author Martin (Linux-Tech-Tips)
 * @brief Header file declaring data structures and functions related to the game task (data.h/TASK_GAME)
 * 
 * This file is responsible for the task handling the actual TETRGS game itself, including data structures and functions 
 * for initialization, proper termination and the loop.
*/

#ifndef TASK_GAME_H
#define TASK_GAME_H

#include <string.h>
#include "terminal_gui/terminal_f.h"

#include "data.h"
#include "block.h"


/* === Data Structures and Constants === */

/** The width of the playing field */
#define FIELD_X 10
/** The height of the playing field */
#define FIELD_Y 20

/** The data structure holding data specifically related to the game task */
typedef struct {

    /** Whether the game task is running (1) or not (0) */
    short gameRun;
    /** Next task to switch to if the Game task terminates */
    nextTask_t nextTask;

    /** The playing field, with one tile being 2x1 characters in size */
    char field [FIELD_X][FIELD_Y];

    /** The origin point (top left) of the game playing field */
    int fieldOriginX, fieldOriginY;

    /** The tiles of the currently falling block, specified as an array of XY coord pairs, with the first tile being the origin of rotation */
    block_t block;

} gameData_t;


/* === Function Declarations === */

/** The main task function for the Game task */
nextTask_t game_task(programData_t * data);

/** Update function for the Game task */
void game_update(programData_t * data, gameData_t * gameData);

/** Render/Draw function for the Game task */
void game_render(programData_t data, gameData_t gameData);

#endif /* TASK_GAME_H */