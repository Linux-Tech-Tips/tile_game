/** 
 * @file block.h
 * @author Martin (Linux-Tech-Tips)
 * @brief Header file declaring data structures and functions related to blocks
*/

#ifndef BLOCK_H
#define BLOCK_H

#include "terminal_gui/terminal_f.h"

/* === Data Structures and Types === */

/** Enumeration defining the type of block */
typedef enum {
    /** T Block - classic 3x2 inverse T shape */
    BLOCK_T = BG_CYAN,
    /** Cube Block - 2x2 cube shape */
    BLOCK_CUBE = BG_MAGENTA,
    /** Line Block - 1x4 line shape */
    BLOCK_LINE = BG_WHITE,
    /** L Block - 2x3 L shape */
    BLOCK_L = BG_GREEN,
    /** L_R Block - 2x3 mirrored L shape */
    BLOCK_L_R = BG_YELLOW,
    /** Z Block - 2x4 Z shape */
    BLOCK_Z = BG_BLUE,
    /** Z_R Block - 2x4 mirrored Z shape */
    BLOCK_Z_R = BG_RED
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


/* === Functions === */

/** Moves the provided block by the specified x and y amount */
void block_move(int x, int y, block_t * block);

/** Rotates the provided block by 90° */
void block_rotate(block_t * block);

/** Populates the given block_t data structure with data of the given block type */
void block_gen(block_t * block, blockType_t type, int startX, int startY);

/** Draws the desired block to the terminal, with a specifiable custom (top-left) origin point */
void block_render(block_t block, int originX, int originY);

#endif /* BLOCK_H */