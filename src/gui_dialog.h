/** 
 * @file gui_dialog.h
 * @author Martin (Linux-Tech-Tips)
 * @brief Header file declaring data structures and functions related to GUI
*/

#ifndef GUI_DIALOG_H
#define GUI_DIALOG_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "terminal_gui/terminal_f.h"

#include "util.h"

/* === STRUCTURES AND CONSTANTS === */

/** The number of available button slots allocated by default */
#define BUTTONS_DEFAULT_SIZE 5
/** The number of new button slots allocated when current size reached */
#define BUTTONS_ALLOC_CONST 5

/** GUI Dialog automatic size, set to a dimension to resize to fit text */
#define GUI_SIZE_AUTO -1

/** Button layout - all buttons are in a single line (which is cut off if space runs out) */
#define BUTTON_LINE 0
/** Button layout - buttons are separated by line breaks, in a vertical column */
#define BUTTON_BREAK 1

/** Data structure holding the data of a button */
typedef struct {
    /* The text content of the button */
    char * text;
    /* The position of the button within the dialog */
    int posX, posY;
    /* The size of the button */
    int sizeX, sizeY;
} button_t;

/** Data structure holding content related to GUI */
typedef struct {

    /** The desired size of the dialog. If a size is set, text is cut off to fit size,
     * if any dimension is set to -1, it is automatically sized to fit the text content.
    */
    int sizeX, sizeY;

    /** The actual size of the dialog, evaluated if sizeX/sizeY is set to automatic */
    int realWidth, realHeight;

    /** Specifies the desired behavior of the buttons, set using constants */
    int buttonLayout;

    /** The title of the GUI dialog instance, keep empty to not show title */
    char * title;
    /** The color of the title text */
    int titleColor;
    /** Currently allocated space for the title text */
    size_t titleSpace;

    /** The text content shown in the dialog */
    char * text;
    /** The color of the text content */
    int textColor;
    /** Currently allocated space for the text content */
    size_t textSpace;

    /** Background color for the dialog */
    int background;

    /** The number of buttons existing in the dialog (and the length of the buttons string array) */
    int numButtons;
    /** Array containing all buttons added to the GUI dialog */
    button_t * buttons;
    /** Internal, the total number of bytes allocated for the buttons string array */
    int allocButtons;
    /** The index of the currently selected button */
    int currentButton;

    /** The background color of the buttons */
    int buttonBackground;
    /** The background color of the actively selected button */
    int buttonActiveBackground;
    /** The color of the text content within buttons */
    int buttonTextColor;

} gui_dialog_t;


/* === FUNCTIONS === */

/** Used to initialize a gui_dialog_t structure, adding a title and text, with default color options. The size and button layout are set here.
 * Allocates space for button text strings.
*/
void gui_createDialog(gui_dialog_t * dialog, char const * title, char const * text, int sizeX, int sizeY, int buttonLayout);

/** Used to initialize a gui_dialog_t structure, adding a title and text of the desired colors, as well as the desired dialog options. 
 * Allocates space for button text strings.
*/
void gui_createDialog_opt(gui_dialog_t * dialog, char const * title, int titleColor, char const * text, int textColor, 
                            int buttonBackground, int activeButtonBackground, int buttonTextColor, int dialogBackground, 
                            int buttonLayout, int sizeX, int sizeY);

/** Deallocates button array, to be called on any dialog not being used anymore */
void gui_destroyDialog(gui_dialog_t * dialog);

/** Sets the text content of the GUI dialog */
void gui_setContent(gui_dialog_t * dialog, char const * title, char const * text);

/** Sets the GUI dialog options, including colors */
void gui_setOptions(gui_dialog_t * dialog, int dialogBackground, int titleColor, int textColor, 
                    int buttonBackground, int activeButtonBackground, int buttonTextColor, 
                    int sizeX, int sizeY);

/** Adds a button to the GUI dialog (manages button text string array memory)
 * @returns the index of the newly added button
*/
int gui_addButton(gui_dialog_t * dialog, char const * buttonText);


/** Updates the given GUI dialog logic (buttons) using the given keyboard input buffer, call this if the dialog is in focus 
 * @returns the index of a button that was pressed, otherwise -1
*/
int gui_update(gui_dialog_t * dialog, keys_t keys);

/** Renders the given GUI dialog to the terminal */
void gui_render(gui_dialog_t dialog, int posX, int posY);


/** Gets the number of non-null chars in the longest line (separated by '\n') in the given content */
size_t _gui_maxLineLength(char const * content);

/** Gets the number of lines (separated by '\n') in the given content */
size_t _gui_numLines(char const * content);

/** Updates the real width and height of the dialog if size set to AUTO */
void _gui_updateAutoSize(gui_dialog_t * dialog);

/** Prints the given lines with the given colors to the given coords, in relation to the given origin,
 * ensures to not breach the given width and height, modifies the given x and y coords to keep track outside.
 * Fills the remainder of each line with fillColor, unless set to NO_CODE;
 * Indents all printed lines with the given indent.
*/
void _gui_printLines(char const * text, int textColor, int background, int fillColor, int * x, int * y, int originX, int originY, int width, int height, int indent);


#endif /* GUI_DIALOG_H */
