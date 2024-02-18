#include "block.h"

void block_moveTile(int tile [2], int x, int y) {
    tile[0] += x;
    tile[1] += y;
}

void block_rotateTile(int tile [2], int originX, int originY) {
    /* Saving the old x and y coordinates into local variables */
    int x = tile[0], y = tile[1];
    /* Rotating using solutions to the matrix equation '(x, y) = ((0, 1), (-1, 0))(x-oX, y-oY) + (oX, oY)' 
        NOTE: The matrix is represented as a list of its columns, the vectors as singular columns */
    tile[0] = -y + originY + originX;
    tile[1] = x - originX + originY;
}

void block_gen(block_t * block, blockType_t type, int startX, int startY) {
    /* Setting the correct type */
    block->type = type;

    switch(type) {
        case BLOCK_T:
            _block_genTiles(block, 0, 0, -1, 0, 1, 0, 0, 1);
            break;
        
        case BLOCK_CUBE:
            _block_genTiles(block, 0, 0, 1, 0, 0, 1, 1, 1);
            break;

        case BLOCK_LINE:
            _block_genTiles(block, 0, 0, -1, 0, 1, 0, 2, 0);
            break;

        case BLOCK_L:
            _block_genTiles(block, 0, 0, -1, 0, 1, 0, 1, 1);
            break;

        case BLOCK_L_R:
            _block_genTiles(block, 0, 0, 1, 0, -1, 0, -1, 1);
            break;

        case BLOCK_Z:
            _block_genTiles(block, 0, 0, 1, 0, 0, 1, -1, 1);
            break;

        case BLOCK_Z_R:
            _block_genTiles(block, 0, 0, -1, 0, 0, 1, 1, 1);
            break;
    }

    for(short i = 0; i < 4; i++)
        block_moveTile(block->tiles[i], startX, startY);
}

void _block_genTiles(block_t * block, int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4) {
    /* ORIGIN TILE */
    block->tiles[0][0] = x1;
    block->tiles[0][1] = y1;
    /* TILE 2 */
    block->tiles[1][0] = x2;
    block->tiles[1][1] = y2;
    /* TILE 3 */
    block->tiles[2][0] = x3;
    block->tiles[2][1] = y3;
    /* TILE 4 */
    block->tiles[3][0] = x4;
    block->tiles[3][1] = y4;
}

void block_render(block_t block, int originX, int originY) {
    /* Going through all 4 tiles in the block */
    for(short i = 0; i < 4; i++) {
        /* Start by moving to the specified origin point (and converting 0-indexed origin into 1-indexed terminal coords) */
        cursorMoveTo(originX+1, originY+1);
        
        /* Moving cursor to the coordinates of the current tile */
        cursorMoveBy(RIGHT, block.tiles[i][0] * 2); /* Multiplying the coordinate movement by 2, as each block has a width of 2 */
        cursorMoveBy(DOWN, block.tiles[i][1]);
        /* As the block type corresponds 1-1 with the background color specification, specifying the type as the color draws the correct color of the block */
        modeSet(NO_CODE, COLOR_BLACK, block.type);
        puts("[]");
    }
}