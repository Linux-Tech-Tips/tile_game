/** 
 * @file task_game.h
 * @author Martin (Linux-Tech-Tips)
 * @brief Header file declaring data structures and functions related to the game task (data.h/TASK_GAME)
 * 
 * This file is responsible for the task handling the actual game itself, including data structures and functions 
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
#include "util.h"
#include "block.h"
#include "gui_dialog.h"


/* === Data Structures and Constants === */

/** The width of the playing field */
#define FIELD_X 10
/** The height of the playing field */
#define FIELD_Y 21

/** The minimum left margin for the playing field */
#define FIELD_MIN_LEFT 16

/** The x coordinate of any newly created block */
#define GAME_NEW_BLOCK_X 4
/** The y coordinate of any newly created block */
#define GAME_NEW_BLOCK_Y 0

/** The default time it takes the block to fall down 1 step */
#define GAME_DEFAULT_FALL_DELAY 0.5f
/** The default time it takes the block to be placed if not falling */
#define GAME_DEFAULT_PLACE_DELAY 0.8f
/** The default minimum delay between two block downward drops */
#define GAME_DEFAULT_DROP_TIME 0.05f

/** Updates-per-second rate for the TASK_GAME task */
#define TASK_GAME_UPS 60
/** The number of keyboard inputs read in one update of the TASK_GAME task */
#define TASK_GAME_KEYS 3

/** Enumeration describing the current state of the game */
typedef enum {
    /** The game is running */
    GAME_RUN,
    /** The game is paused */
    GAME_PAUSED,
    /** The game has ended */
    GAME_OVER,
    /** The game needs to be restarted */
    GAME_RESTART,
    /** The terminal size is invalid and the game can't continue */
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

    /** If set, the screen will be cleared on the next frame (use sparingly to prevent flickering) */
    short screenClear;


    /** The game pause menu dialog */
    gui_dialog_t pauseMenu;
    /** The pause menu 'continue' button */
    int pauseContinue;
    /** The pause menu 'restart' button */
    int pauseRestart;
    /** The pause menu 'quit' button */
    int pauseQuit;

    /** The game over menu dialog */
    gui_dialog_t overMenu;
    /** The game over menu 'restart' button */
    int overRestart;
    /** The game over menu 'quit' button */
    int overQuit;


    /** Variable storing whether any keyboard input read in the last frame */
    short keyIn;
    /** Variable storing the keyboard inputs processed in the last frame */
    keys_t keys;


    /** The current score of the game */
    int score;

    /** The playing field, with one tile being 2x1 characters in size */
    char field [FIELD_X][FIELD_Y];
    /** The origin point (top left) of the game playing field */
    int fieldOriginX, fieldOriginY;

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

    /** The minimum time delay between two drops of a block using the down arrow key */
    float dropTime;
    /** The timer counting down the drop time delay */
    float dropTimer;

} gameData_t;


/* === Function Declarations === */

/** The main task function for the Game task */
nextTask_t game_task(programData_t * data);


/* --- Create/Destroy Functions --- */

/** Initializes an empty gameData_t structure with the given parameters */
void game_init(gameData_t * data, fieldAlign_t alignment, int termX, int termY);

/** Resets the gameplay data of an existing gameData_t structure with the given parameters */
void game_reset(gameData_t * data, fieldAlign_t alignment, int termX, int termY);

/** Destroys a gameData_t structure once it's no longer used */
void game_destroy(gameData_t * data);


/* --- Update Functions --- */

/** General update function for the Game task (logic shared by all states) */
void game_update(programData_t * data, gameData_t * gameData);

/** Update function containing the GAME_RUN state logic */
void game_updateRun(programData_t * data, gameData_t * gameData);

/** Update function containing the GAME_PAUSED state logic */
void game_updatePaused(gameData_t * gameData);

/** Update function containing the GAME_OVER state logic */
void game_updateOver(gameData_t * gameData);

/** Update function containing the GAME_INVALID state logic, for invalid terminal size (or general error state) */
void game_updateInvalid(programData_t * data, gameData_t * gameData);


/* --- Render Functions --- */

/** Render/Draw function for the Game task */
void game_render(programData_t data, gameData_t gameData);

/** Render function containing specific separate GAME_RUN state draw calls */
void game_renderRun(gameData_t gameData);

/** Render function containing specific separate GAME_PAUSED state draw calls */
void game_renderPaused(programData_t data, gameData_t gameData);

/** Render function containing specific separate GAME_OVER state draw calls */
void game_renderOver(programData_t data, gameData_t gameData);

/** Render function containing GAME_INVALID state draw calls, for invalid terminal size */
void game_renderInvalid(programData_t data);


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

/** Saves the X and Y origin of a game playing field to posX and posY */
void _game_getAlignPos(fieldAlign_t alignment, int * posX, int * posY, int terminalWidth, int terminalHeight);

#endif /* TASK_GAME_H */