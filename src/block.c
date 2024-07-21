#include "block.h"

void block_moveTile(int tile [TILE_COORDS], int x, int y) {
    tile[TILE_X] += x;
    tile[TILE_Y] += y;
}

void block_rotateTile(int tile [TILE_COORDS], int originX, int originY) {
    /* Saving the old x and y coordinates into local variables */
    int x = tile[TILE_X], y = tile[TILE_Y];
    /* Rotating using solutions to the matrix equation '(x, y) = ((0, 1), (-1, 0))(x-oX, y-oY) + (oX, oY)' 
        NOTE: The matrix is represented as a list of its columns, the vectors as singular columns */
    tile[TILE_X] = -y + originY + originX;
    tile[TILE_Y] = x - originX + originY;
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

    for(short i = 0; i < BLOCK_SIZE; ++i)
        block_moveTile(block->tiles[i], startX, startY);
}

void _block_genTiles(block_t * block, int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4) {
    /* ORIGIN TILE */
    block->tiles[0][TILE_X] = x1;
    block->tiles[0][TILE_Y] = y1;
    /* TILE 2 */
    block->tiles[1][TILE_X] = x2;
    block->tiles[1][TILE_Y] = y2;
    /* TILE 3 */
    block->tiles[2][TILE_X] = x3;
    block->tiles[2][TILE_Y] = y3;
    /* TILE 4 */
    block->tiles[3][TILE_X] = x4;
    block->tiles[3][TILE_Y] = y4;
}

void block_render(block_t block, int originX, int originY) {
    /* Going through all tiles in the block */
    for(short i = 0; i < BLOCK_SIZE; ++i) {
        /* Start by moving to the specified origin point (and converting 0-indexed origin into 1-indexed terminal coords) */
        cursorMoveTo(originX+1, originY+1);
        
        /* Moving cursor to the coordinates of the current tile */
        cursorMoveBy(RIGHT, block.tiles[i][TILE_X] * 2); /* Multiplying the coordinate movement by 2, as each block has a width of 2 */
        cursorMoveBy(DOWN, block.tiles[i][TILE_Y]);
        /* As the block type corresponds 1-1 with the background color specification, specifying the type as the color draws the correct color of the block */
        modeSet(NO_CODE, COLOR_BLACK, block.type);
        puts("[]");
    }
}

short block_testPos(block_t block, int posX, int posY) {
    for(short i = 0; i < BLOCK_SIZE; ++i) {
        if(posX == block.tiles[i][0] && posY == block.tiles[i][1])
            return 1;
    }
    return 0;
}


blockType_t block_getNext(bag_t * bag) {
    /* Reshuffling if index reached out of bounds */
    if(bag->nextIdx >= (BLOCK_TYPE_NUM * BLOCK_BAG_SIZE)) {
        block_shuffleBag(bag);
    }

    /* Returning the next block type if bag valid, otherwise T block */
    if(bag->nextIdx < (BLOCK_TYPE_NUM * BLOCK_BAG_SIZE) && bag->nextIdx >= 0) {
        return bag->shuffle[bag->nextIdx++];
    } else {
        return BLOCK_T;
    }
}

void block_shuffleBag(bag_t * bag) {
    /* Shuffling the bag from largest array index to the second index */
    for(int i = ((BLOCK_TYPE_NUM * BLOCK_BAG_SIZE) - 1); i > 0; --i) {
        /* Generating random element from the left-hand side of the array to swap the current element with */
        int j = rand() % (i+1);
        /* Swapping elements */
        if(j != i) {
            int tmp = bag->shuffle[j];
            bag->shuffle[j] = bag->shuffle[i];
            bag->shuffle[i] = tmp;
        }
    }
    /* Resetting next block index */
    bag->nextIdx = 0;
}

void block_initBag(bag_t * bag) {
    /* Initializing the block array which will be shuffled */
    for(int i = 0; i < BLOCK_TYPE_NUM; ++i) {
        /* Creating the next tile based on the iteration number */
        blockType_t b = BLOCK_TYPE_MIN + i;
        /* Writing the block type into all the expected array positions */
        for(int j = 0; j < BLOCK_BAG_SIZE; ++j)
            bag->shuffle[i+(j*BLOCK_TYPE_NUM)] = b;
    }
    /* Initializing next index to 0 */
    bag->nextIdx = 0;
    /* Shuffling the new initialized bag */
    block_shuffleBag(bag);
}