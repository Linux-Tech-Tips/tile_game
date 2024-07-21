/** 
 * @file block.h
 * @author Martin (Linux-Tech-Tips)
 * @brief Header file declaring data structures and functions related to blocks
*/

#ifndef BLOCK_H
#define BLOCK_H

#include <stdlib.h>

#include "terminal_gui/terminal_f.h"

/* === Data Structures, Types and Constants === */

/** The size (number of tiles) of a standard block */
#define BLOCK_SIZE 4
/** The number of coordinate values (here: XY -> 2) of a tile, for clarity and to avoid magic numbers elsewhere in code */
#define TILE_COORDS 2
/** The index of a tile's x coordinate */
#define TILE_X 0
/** The index of a tile's y coordinate */
#define TILE_Y 1

/** The number of unique defined block types */
#define BLOCK_TYPE_NUM 7
/** The number of times the complete block set fits into the bag */
#define BLOCK_BAG_SIZE 2

/** Enumeration defining the type of block */
typedef enum {
    /** T Block - 3x2 inverse T shape */
    BLOCK_T = COLOR_CYAN,
    /** Cube Block - 2x2 cube shape */
    BLOCK_CUBE = COLOR_MAGENTA,
    /** Line Block - 1x4 line shape */
    BLOCK_LINE = COLOR_WHITE,
    /** L Block - 2x3 L shape */
    BLOCK_L = COLOR_GREEN,
    /** L_R Block - 2x3 mirrored L shape */
    BLOCK_L_R = COLOR_YELLOW,
    /** Z Block - 2x4 Z shape */
    BLOCK_Z = COLOR_BLUE,
    /** Z_R Block - 2x4 mirrored Z shape */
    BLOCK_Z_R = COLOR_RED
} blockType_t;

/** The smallest value the blockType_t enumeration can evaluate to */
#define BLOCK_TYPE_MIN 41
/** The largest value the blockType_t enumeration can evaluate to */
#define BLOCK_TYPE_MAX 47

/** The data structure containing information about a given block 
 * NOTE: The format the tiles of the block are saved as is a fixed-size array of tiles[4][2], as 
 *       each block is made of 4 tiles, with XY coords of length 2
*/
typedef struct {
    /** The tiles the block consists of */
    int tiles [BLOCK_SIZE][TILE_COORDS];
    /** The type of the block (mostly responsible for visuals, color) */
    blockType_t type;
} block_t;

/** The data structure containing a block randomizer bag, including a shuffled list and the index of the next element */
typedef struct {
    blockType_t shuffle [BLOCK_BAG_SIZE * BLOCK_TYPE_NUM];
    int nextIdx;
} bag_t;

/* === Functions === */

/** Moves the given tile by the specified offset */
void block_moveTile(int tile [TILE_COORDS], int x, int y);

/** Rotates the given tile around the specified origin */
void block_rotateTile(int tile [TILE_COORDS], int originX, int originY);

/** Populates the given block_t data structure with data of the given block type */
void block_gen(block_t * block, blockType_t type, int startX, int startY);

/** Internal function, assigns the given coordinates to the correct fields in the block structure (number of arguments should correspond with defined BLOCK_SIZE) */
void _block_genTiles(block_t * block, int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);

/** Draws the desired block to the terminal, with a specifiable custom (top-left) origin point */
void block_render(block_t block, int originX, int originY);

/** Tests whether any part of the block covers the given position or not 
 * (relative to the block origin)
*/
short block_testPos(block_t block, int posX, int posY);

/** Returns the next blockType_t drawn from the given bag, reshuffles bag if end reached 
 * NOTE: On any error, the function defaults to the T block
*/
blockType_t block_getNext(bag_t * bag);

/** Shuffles the given bag randomly (Knuth-based shuffle) and resets the next element index to 0 */
void block_shuffleBag(bag_t * bag);

/** Initializes the bag structure with unshuffled blocks and an index of 0, and proceeds to shuffle the blocks
 * NOTE: The format the unshuffled bag is generated in is each block 'BLOCK_BAG_SIZE'-times, in order
*/
void block_initBag(bag_t * bag);

#endif /* BLOCK_H */