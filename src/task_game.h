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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "terminal_gui/terminal_f.h"

#include "data.h"
#include "block.h"


/* === Data Structures and Constants === */

/** The width of the playing field */
#define FIELD_X 10
/** The height of the playing field */
#define FIELD_Y 20

/** The x coordinate of any newly created block */
#define GAME_NEW_BLOCK_X 4
/** The y coordinate of any newly created block */
#define GAME_NEW_BLOCK_Y 0

/** Updates-per-second rate for the TASK_GAME task */
#define TASK_GAME_UPS 60
/** The number of keyboard inputs read in one update of the TASK_GAME task */
#define TASK_GAME_KEYS 3

typedef enum {
    GAME_RUN,
    GAME_PAUSED,
    GAME_OVER,
    GAME_INVALID
} gameState_t;

/** The data structure holding data specifically related to the game task */
typedef struct {

    /** Whether the game task is running (1) or not (0) */
    short gameRun;
    /** Next task to switch to if the Game task terminates */
    nextTask_t nextTask;

    /** The current state of the game */
    gameState_t gameState;

    /** Variable storing whether any keyboard input read in the last frame */
    short keyIn;
    /** Variable storing the keyboard input buffer read in the last frame */
    char keyBuffer [TASK_GAME_KEYS];

    /** The current score of the game */
    int score;

    /** The playing field, with one tile being 2x1 characters in size */
    char field [FIELD_X][FIELD_Y];
    /** The origin point (top left) of the game playing field */
    int fieldOriginX, fieldOriginY;

    /** The current size of the terminal */
    int termX, termY;

    /** The tiles of the currently falling block, specified as an array of XY coord pairs, with the first tile being the origin of rotation */
    block_t block;
    /** The randomizer bag to generate a constrained random next block */
    bag_t blockBag;

    /** The default/initial time it takes for a block to fall down 1 step */
    float defaultFallDelay;
    /** The current time it takes for a block to fall down 1 step */
    float fallDelay;
    /** The timer for the current 1-step fall */
    float fallTimer;
    /** Whether the block is currently falling */
    short falling;

    /** The time it takes for a non-falling block to be placed down */
    float placeDelay;
    /** The timer for the current block placement */
    float placeTimer;

} gameData_t;


/* === Function Declarations === */

/** The main task function for the Game task */
nextTask_t game_task(programData_t * data);


/* --- Update Functions --- */

/** General update function for the Game task (logic shared by all states) */
void game_update(programData_t * data, gameData_t * gameData);

/** Update function containing the GAME_RUN state logic */
void game_updateRun(programData_t * data, gameData_t * gameData);

/** Update function containing the GAME_PAUSED state logic */
void game_updatePaused(programData_t * data, gameData_t * gameData);

/** Update function containing the GAME_OVER state logic */
void game_updateOver(programData_t * data, gameData_t * gameData);

/** Update function containing the GAME_INVALID state logic, for invalid terminal size (or general error state) */
void game_updateInvalid(programData_t * data, gameData_t * gameData);


/* --- Render Functions --- */

/** Render/Draw function for the Game task */
void game_render(programData_t data, gameData_t gameData);

/** Render function containing specific separate GAME_RUN state draw calls */
void game_renderRun(programData_t data, gameData_t gameData);

/** Render function containing specific separate GAME_PAUSED state draw calls */
void game_renderPaused(programData_t data, gameData_t gameData);

/** Render function containing specific separate GAME_OVER state draw calls */
void game_renderOver(programData_t data, gameData_t gameData);

/** Render function containing GAME_INVALID state draw calls, for invalid terminal size */
void game_renderInvalid(programData_t data, gameData_t gameData);


/* --- Other Functions --- */

/** Moves the given block and checks collisions with respect to the passed game field. If the movement would collide, the block isn't moved. 
 * @param dryRun if this is 1, the movement isn't actually saved, and only the result is returned, behaving as only a collision checker
 * @return the result of the movement, whether moved successfully (1) or collided and not moved (0)
*/
short game_moveBlock(block_t * block, int x, int y, char field [FIELD_X][FIELD_Y], short dryRun);

/** Rotates the given block and checks collisions with respect to the passed game field. If the rotation would collide, the block isn't rotated. 
 * @return the result of the rotation, whether rotated successfully (1) or collided and not rotated (0)
*/
short game_rotateBlock(block_t * block, char field [FIELD_X][FIELD_Y]);

/** Internal function, collides the given tile coordinates with the given field and returns whether the tile collides (1) or not (0) */
short _game_collideTile(int tileX, int tileY, char field[FIELD_X][FIELD_Y]);

/** Internal function, generates a block in the gameData_t structure at the given starting position, of a random type. 
 * The function keeps track of the last block that was generated, forcing a single reroll upon generating the same block twice in a row, 
 * which decreases the chances of duplicate block generation, while keeping it as a rare game mechanic/feature.
*/
void _game_genBlock(gameData_t * data, int startX, int startY);

/** Internal function, places down the currently existing block at its position into the game's field and returns whether placed correctly 
 * @returns 1 if the block was placed correctly, 0 if not
*/
short _game_placeBlock(gameData_t * data);

#endif /* TASK_GAME_H */