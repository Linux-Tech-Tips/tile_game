/** 
 * @file block.h
 * @author Martin (Linux-Tech-Tips)
 * @brief Header file declaring data structures and functions related to blocks
*/

#ifndef BLOCK_H
#define BLOCK_H

#include <stdlib.h>

#include "terminal_gui/terminal_f.h"

/* === Data Structures and Types === */

/** Enumeration defining the type of block */
typedef enum {
    /** T Block - classic 3x2 inverse T shape */
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

/** The data structure containing information about a given block 
 * NOTE: The format the tiles of the block are saved as is a fixed-size array of tiles[4][2], as 
 *       each block is made of 4 tiles, with XY coords of length 2
*/
typedef struct {
    /** The tiles the block consists of */
    int tiles [4][2];
    /** The type of the block (mostly responsible for visuals, color) */
    blockType_t type;
} block_t;

/** The data structure containing a block randomizer bag, including a shuffled list and the index of the next element
 * NOTE: The bag uses a shuffle of 14 elements - 2x of each blockType
*/
typedef struct {
    blockType_t shuffle [14];
    int nextIdx;
} bag_t;

/* === Functions === */

/** Moves the given tile by the specified offset */
void block_moveTile(int tile [2], int x, int y);

/** Rotates the given tile around the specified origin */
void block_rotateTile(int tile [2], int originX, int originY);

/** Populates the given block_t data structure with data of the given block type */
void block_gen(block_t * block, blockType_t type, int startX, int startY);

/** Internal function, assigns the given coordinates to the correct fields in the block structure */
void _block_genTiles(block_t * block, int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);

/** Draws the desired block to the terminal, with a specifiable custom (top-left) origin point */
void block_render(block_t block, int originX, int originY);

/** Returns the next blockType_t drawn from the given bag, reshuffles bag if end reached 
 * NOTE: On any error, the function defaults to the T block
*/
blockType_t block_getNext(bag_t * bag);

/** Shuffles the given bag randomly (Knuth-based shuffle) and resets the next element index to 0 */
void block_shuffleBag(bag_t * bag);

/** Initializes the bag structure with unshuffled blocks and an index of 0, and proceeds to shuffle the blocks
 * NOTE: The format the unshuffled bag is generated in is all the blocks in order, twice
*/
void block_initBag(bag_t * bag);

#endif /* BLOCK_H */