/** 
 * @file util.h
 * @author Martin (Linux-Tech-Tips)
 * @brief A collection of utility functions and structures
*/

#ifndef TG_UTIL_H
#define TG_UTIL_H

#include <string.h>


/* === KEYBOARD IO === */

/** Structure containing boolean switches for all keys detected by the application, 
 * so that entire keyboard buffer can be processed into a map of currently pressed keys,
 * including keys consisting of an escape code of multiple characters 
 * (for example so that pressing any escape-based keys, such as arrow keys, isn't interpreted as the ESC key)
*/
typedef struct {
    /* Chars 1 */
    short KEY_Q;
    short KEY_W;
    short KEY_E;
    short KEY_R;
    short KEY_T;
    short KEY_Y;
    short KEY_U;
    short KEY_I;
    short KEY_O;
    short KEY_P;
    /* Chars 2 */
    short KEY_A;
    short KEY_S;
    short KEY_D;
    short KEY_F;
    short KEY_G;
    short KEY_H;
    short KEY_J;
    short KEY_K;
    short KEY_L;
    /* Chars 3 */
    short KEY_Z;
    short KEY_X;
    short KEY_C;
    short KEY_V;
    short KEY_B;
    short KEY_N;
    short KEY_M;

    /* Escape-based keys */
    short KEY_ESC;
    short KEY_ARROW_LEFT;
    short KEY_ARROW_RIGHT;
    short KEY_ARROW_UP;
    short KEY_ARROW_DOWN;
    short KEY_ENTER;
    short KEY_TAB;
} keys_t;

/** Processes all keystrokes recorded as characters in keyBuffer into the passed keys structure */
void keys_processBuffer(char * keyBuffer, size_t numKeys, keys_t * keys);


/** returns the maximum of two given integers */
int util_maxInt(int a, int b);

#endif /* TG_UTIL_H */