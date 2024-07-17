#include "util.h"

/* === KEYBOARD IO === */

void keys_processBuffer(char * keyBuffer, size_t numKeys, keys_t * keys) {

    /* Clearing all keys to false before processing the frame's keyBuffer */
    *keys = (const keys_t){0};

    for(size_t idx = 0; idx < numKeys; ++idx) {
        /* Switching the correct key property according to the key buffer */
        switch(keyBuffer[idx]) {
            /* Chars 1 */
            case 'q':
            case 'Q':
                keys->KEY_Q = 1;
                break;
            case 'w':
            case 'W':
                keys->KEY_W = 1;
                break;
            case 'e':
            case 'E':
                keys->KEY_E = 1;
                break;
            case 'r':
            case 'R':
                keys->KEY_R = 1;
                break;
            case 't':
            case 'T':
                keys->KEY_T = 1;
                break;
            case 'y':
            case 'Y':
                keys->KEY_Y = 1;
                break;
            case 'u':
            case 'U':
                keys->KEY_U = 1;
                break;
            case 'i':
            case 'I':
                keys->KEY_I = 1;
                break;
            case 'o':
            case 'O':
                keys->KEY_O = 1;
                break;
            case 'p':
            case 'P':
                keys->KEY_P = 1;
                break;

            /* Chars 2 */
            case 'a':
            case 'A':
                keys->KEY_A = 1;
                break;
            case 's':
            case 'S':
                keys->KEY_S = 1;
                break;
            case 'd':
            case 'D':
                keys->KEY_D = 1;
                break;
            case 'f':
            case 'F':
                keys->KEY_F = 1;
                break;
            case 'g':
            case 'G':
                keys->KEY_G = 1;
                break;
            case 'h':
            case 'H':
                keys->KEY_H = 1;
                break;
            case 'j':
            case 'J':
                keys->KEY_J = 1;
                break;
            case 'k':
            case 'K':
                keys->KEY_K = 1;
                break;
            case 'l':
            case 'L':
                keys->KEY_L = 1;
                break;

            /* Chars 3 */
            case 'z':
            case 'Z':
                keys->KEY_L = 1;
                break;
            case 'x':
            case 'X':
                keys->KEY_L = 1;
                break;
            case 'c':
            case 'C':
                keys->KEY_L = 1;
                break;
            case 'v':
            case 'V':
                keys->KEY_L = 1;
                break;
            case 'b':
            case 'B':
                keys->KEY_L = 1;
                break;
            case 'n':
            case 'N':
                keys->KEY_L = 1;
                break;
            case 'm':
            case 'M':
                keys->KEY_L = 1;
                break;

            /* Escape-based keys */
            case (char)(27):
                /* 91 '[' is the character following ESC for general-purpose escape codes, like the arrow keys, del, home, pgup/pgdn etc. */
                if((idx+1) < numKeys && keyBuffer[idx+1] == (char)(91)) {
                    ++idx;
                    /* Ensuring no out-of-bounds read */
                    if((idx+1) < numKeys) {
                        switch((int)keyBuffer[idx+1]) {
                            case 65:
                                keys->KEY_ARROW_UP = 1;
                                ++idx;
                                break;
                            case 66:
                                keys->KEY_ARROW_DOWN = 1;
                                ++idx;
                                break;
                            case 67:
                                keys->KEY_ARROW_RIGHT = 1;
                                ++idx;
                                break;
                            case 68:
                                keys->KEY_ARROW_LEFT = 1;
                                ++idx;
                                break;
                            default:
                                break;
                        }
                    }
                /* 79 'O' is the character following ESC for some F keys (locally F1-F4), capturing to not trigger ESC */
                } else if((idx+1) < numKeys && keyBuffer[idx+1] == (char)(79)) {
                    ++idx;
                } else {
                    /* NOTE: This can still lead to a false trigger of ESC, if an escape-based key is pressed 
                        late into the frame, and ends up only fitting halfway into the buffer.
                        This is unlikely, but could theoretically happen, which may be good to keep in mind.
                        ---
                        Making a more reliable system with just a basic terminal wouldn't be very simple,
                        so this is sufficient for this project. However, for a better UX, it is recommended
                        to use a fast tick rate, and a small keyBuffer size, optimally a multiple of 3
                        (as arrow keys are 3 bytes wide, and will be likely the most commonly spammed key).
                    */
                    keys->KEY_ESC = 1;
                }
                break;
            case (char)(10):
                keys->KEY_ENTER = 1;
                break;
            case (char)(9):
                keys->KEY_TAB = 1;
                break;
            
            default:
                break;
        }
    }
}


int util_maxInt(int a, int b) {
    return (a > b ? a : b);
}

int util_center(int objSize, int totalSize) {
    /* Return 1 if object doesn't fit */
    if(objSize > totalSize)
        return 1;
    /* Return the approximate center position, the 1 is an offset to make the result 1-indexed */
    return (totalSize / 2) - (objSize / 2) + 1;
}