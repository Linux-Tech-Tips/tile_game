/** 
 * @file game.h
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


/* === Data Structures === */

/** Enumeration defining the type of block */
typedef enum {
    /** T Block - classic 3x2 inverse T shape */
    BLOCK_T,
    /** Cube Block - 2x2 cube shape */
    BLOCK_CUBE,
    /** Line Block - 1x4 line shape */
    BLOCK_LINE,
    /** L Block - 2x3 L shape */
    BLOCK_L,
    /** L_R Block - 2x3 mirrored L shape */
    BLOCK_L_R,
    /** Z Block - 2x4 Z shape */
    BLOCK_Z,
    /** Z_R Block - 2x4 mirrored Z shape */
    BLOCK_Z_R
} blockType_t;

/** The data structure holding data specifically related to the game task */
typedef struct {

    /** Whether the game task is running (1) or not (0) */
    short gameRun;
    /** Next task to switch to if the Game task terminates */
    nextTask_t nextTask;

    /** The playing field - 10x20 dimensions, with one tile being 2x1 characters in size */
    char field [20][20];

    /** The origin point (top left) of the game playing field */
    int fieldOriginX, fieldOriginY;

    /** The tiles of the currently falling block, specified as an array of XY coord pairs, with the first tile being the origin of rotation */
    int blockTiles [4][2];

} gameData_t;


/* === Function Declarations === */

/** The main task function for the Game task */
nextTask_t game_task(programData_t * data);

/** Update function for the Game task */
void game_update(programData_t * data, gameData_t * gameData);

/** Render/Draw function for the Game task */
void game_render(programData_t data, gameData_t gameData);

/** Moves the provided block (specified as an array of tiles) by the specified x and y amount */
void game_moveBlock(int x, int y, int tiles [4][2]);

/** Rotates the provided block (specified as an array of tiles) by 90° 
 * NOTE: The function assumes the first tile is the origin of rotation for the Block
*/
void game_rotateBlock(int tiles [4][2]);

/** Generates the coordinates of the tiles for the block of the given type */
void game_genBlock(int tiles [4][2], blockType_t type);

#endif /* TASK_GAME_H */