#include "gui_dialog.h"

void gui_createDialog(gui_dialog_t * dialog, char const * title, char const * text, int sizeX, int sizeY, int buttonLayout) {

    gui_createDialog_opt(dialog, title, COLOR_WHITE, COLOR_CYAN, text, COLOR_WHITE, 
                            COLOR_MAGENTA, COLOR_RED, COLOR_CYAN, COLOR_BLUE, 
                            buttonLayout, sizeX, sizeY);
}

void gui_createDialog_opt(gui_dialog_t * dialog, char const * title, int titleColor, int titleBackground, char const * text, int textColor, 
                            int buttonBackground, int activeButtonBackground, int buttonTextColor, int dialogBackground, 
                            int buttonLayout, int sizeX, int sizeY) {

    /* Setting options */
    gui_setOptions(dialog, dialogBackground, titleColor, titleBackground, textColor, 
                    buttonBackground, activeButtonBackground, buttonTextColor, 
                    sizeX, sizeY);

    /* Allocating memory for the button buffer */
    dialog->buttons = malloc(BUTTONS_DEFAULT_SIZE * sizeof(button_t));
    dialog->numButtons = 0;
    dialog->currentButton = -1;
    dialog->allocButtons = BUTTONS_DEFAULT_SIZE;
    dialog->buttonLayout = buttonLayout;

    /* Initializing dialog content to empty string */
    dialog->titleSpace = strlen(title) + 1;
    dialog->title = malloc(dialog->titleSpace * sizeof(char));
    dialog->title[0] = '\0';
    dialog->textSpace = strlen(text) + 1;
    dialog->text = malloc(dialog->textSpace * sizeof(char));
    dialog->text[0] = '\0';

    /* Setting content */
    gui_setContent(dialog, title, text);
}

void gui_destroyDialog(gui_dialog_t * dialog) {
    /* Freeing allocated button texts */
    for(int i = 0; i < dialog->numButtons; ++i) {
        free(dialog->buttons[i].text);
    }
    /* Deallocating and clearing button list */
    dialog->numButtons = 0;
    dialog->currentButton = -1;
    free(dialog->buttons);
    /* Freeing content */
    free(dialog->title);
    free(dialog->text);
}


void gui_setOptions(gui_dialog_t * dialog, int dialogBackground, int titleColor, int titleBackground, int textColor, 
                    int buttonBackground, int activeButtonBackground, int buttonTextColor, 
                    int sizeX, int sizeY) {
    
    /* Setting colors */
    dialog->background = dialogBackground;
    dialog->titleColor = titleColor;
    dialog->titleBackground = titleBackground;
    dialog->textColor = textColor;
    dialog->buttonBackground = buttonBackground;
    dialog->buttonActiveBackground = activeButtonBackground;
    dialog->buttonTextColor = buttonTextColor;

    /* Setting other options */
    dialog->sizeX = sizeX;
    dialog->sizeY = sizeY;
    dialog->realWidth = sizeX;
    dialog->realHeight = sizeY;
}

void gui_setContent(gui_dialog_t * dialog, char const * title, char const * text) {
    /* Expanding string if necessary */
    if(strlen(title) > (dialog->titleSpace - 1)) {
        dialog->titleSpace = strlen(title) + 1;
        dialog->title = realloc(dialog->title, dialog->titleSpace * sizeof(char));
    }
    if(strlen(text) > (dialog->textSpace - 1)) {
        dialog->textSpace = strlen(text) + 1;
        dialog->text = realloc(dialog->text, dialog->textSpace * sizeof(char));
    }
    
    /* Setting text content */
    strcpy(dialog->title, title);
    strcpy(dialog->text, text);

    /* Updating the dialog size based on the content */
    _gui_updateAutoSize(dialog);
}

int gui_addButton(gui_dialog_t * dialog, char const * buttonText) {
    
    /* Allocating more space if necessary */
    if(dialog->numButtons >= dialog->allocButtons) {
        dialog->allocButtons += BUTTONS_ALLOC_CONST;
        dialog->buttons = realloc(dialog->buttons, (dialog->allocButtons) * sizeof(button_t));
    }

    /* Adding button and incrementing total number */
    int currentIdx = (dialog->numButtons)++;
    /* Getting pointer to current button and filling with data */
    button_t * button = (dialog->buttons + currentIdx);
    /* Allocating space for button text and saving text */
    button->text = malloc((strlen(buttonText) + 1) * sizeof(char));
    strcpy(button->text, buttonText);

    /* Setting button position */
    if(currentIdx == 0 || dialog->buttonLayout == BUTTON_BREAK) {
        button->posX = 0;
        button->posY = dialog->realHeight;
    } else if(dialog->buttonLayout == BUTTON_LINE) {
        button_t lastButton = dialog->buttons[currentIdx-1];
        button->posX = lastButton.posX + lastButton.sizeX + 1;
        button->posY = lastButton.posY;
    } else {
        button->posX = 1;
        button->posY = 1;
    }
    /* Setting button size */
    button->sizeX = _gui_maxLineLength(button->text);
    button->sizeY = _gui_numLines(button->text);

    /* Setting active button */
    if(dialog->currentButton < 0)
        dialog->currentButton = currentIdx;

    /* Updating dialog real width and height with new button */
    _gui_updateAutoSize(dialog);

    /* Returning index of newly added button */
    return currentIdx;
}


int gui_update(gui_dialog_t * dialog, keys_t keys) {

    /* Checking if there are valid buttons and ensuring correct current button index if so */
    if(dialog->currentButton < 0 && dialog->numButtons > 0) {
        dialog->currentButton = 0;
    }
    if(dialog->numButtons <= 0 && dialog->currentButton >= 0) {
        dialog->currentButton = -1;
    }

    /* Getting whether next or previous button should be accessed based on the currently active keys and keyboard layout */
    short prev = (keys.KEY_ARROW_UP && dialog->buttonLayout == BUTTON_BREAK) || (keys.KEY_ARROW_LEFT && dialog->buttonLayout == BUTTON_LINE);
    short next = (keys.KEY_ARROW_DOWN && dialog->buttonLayout == BUTTON_BREAK) || (keys.KEY_ARROW_RIGHT && dialog->buttonLayout == BUTTON_LINE);
    /* Switching between buttons */
    if(prev && dialog->currentButton > 0)
        dialog->currentButton--;
    else if(next && dialog->currentButton < (dialog->numButtons-1))
        dialog->currentButton++;

    /* Checking whether Enter key pressed */
    if(keys.KEY_ENTER)
        return dialog->currentButton;

    /* Returning -1 if nothing pressed */
    return -1;
}

void gui_render(gui_dialog_t dialog, int posX, int posY, short focused) {

    /* Checking if too thin to render anything */
    if(dialog.realWidth < 1 || dialog.realHeight < 1)
        return;

    modeReset();

    /* Setting up cursor tracking variables */
    int cursorRelX = 0;
    int cursorRelY = 0;

    /* Printing title if exists */
    if(dialog.title[0] != '\0') {
        cursorMoveTo(posX, posY);
        int background = (focused ? dialog.titleBackground : dialog.background);
        _gui_printLines(dialog.title, dialog.titleColor, background, background, &cursorRelX, &cursorRelY, posX, posY, dialog.realWidth, dialog.realHeight, 1);
    }

    /* Printing empty line separator */
    if(cursorRelY < dialog.realHeight) {
        _gui_printLines("", NO_CODE, dialog.background, dialog.background, &cursorRelX, &cursorRelY, posX, posY, dialog.realWidth, dialog.realHeight, 0);
    }

    /* Printing text content */
    if(dialog.text[0] != '\0' && cursorRelY < dialog.realHeight) {
        cursorMoveTo(posX+cursorRelX, posY+cursorRelY);
        _gui_printLines(dialog.text, dialog.textColor, dialog.background, dialog.background, &cursorRelX, &cursorRelY, posX, posY, dialog.realWidth, dialog.realHeight, 1);

        /* Printing empty line separator */
        if(cursorRelY < dialog.realHeight) {
            _gui_printLines("", NO_CODE, dialog.background, dialog.background, &cursorRelX, &cursorRelY, posX, posY, dialog.realWidth, dialog.realHeight, 0);
        }
    }


    /* The lowest point below all buttons, to print final line for BUTTON_LINE layout */
    int buttonMaxY = 0;

    /* Print all buttons */
    for(int idx = 0; idx < dialog.numButtons && (cursorRelX < dialog.realWidth && cursorRelY < dialog.realHeight); ++idx) {
        
        /* Getting position from button information */
        button_t button = dialog.buttons[idx];
        cursorRelX = button.posX;
        cursorRelY = button.posY;
        cursorMoveTo(posX+cursorRelX, posY+cursorRelY);

        /* Updating largest found button bottom coord */
        if((cursorRelY + button.sizeY) > buttonMaxY)
            buttonMaxY = (cursorRelY + button.sizeY);

        /* Print button content */
        int background = ((dialog.currentButton == idx && focused) ? dialog.buttonBackground : dialog.buttonActiveBackground);
        _gui_printLines(dialog.buttons[idx].text, dialog.buttonTextColor, background, dialog.background,
                        &cursorRelX, &cursorRelY, posX, posY, dialog.realWidth, dialog.realHeight, 1);
        
        /* Printing empty next line if desired */
        if(dialog.buttonLayout == BUTTON_BREAK && cursorRelY < dialog.realHeight) {
            _gui_printLines("", NO_CODE, dialog.background, dialog.background, &cursorRelX, &cursorRelY, posX, posY, dialog.realWidth, dialog.realHeight, 0);
        }
    }

    /* Print bottom margin line if not printed by for loop */
    if(dialog.buttonLayout == BUTTON_LINE && buttonMaxY < dialog.realHeight) {
        cursorRelX = 0;
        cursorRelY = buttonMaxY;
        _gui_printLines("", NO_CODE, dialog.background, dialog.background, &cursorRelX, &cursorRelY, posX, posY, dialog.realWidth, dialog.realHeight, 0);
    }
}


size_t _gui_maxLineLength(char const * content) {

    /* Returning line length if no line breaks */
    if(strchr(content, '\n') == NULL)
        return strlen(content);
    
    /* Going through content, checking for longest line */
    size_t lineLength = 0;
    size_t currentLength = 0;
    for(size_t idx = 0; idx < strlen(content); ++idx) {
        if(content[idx] == '\n') {
            if(currentLength > lineLength)
                lineLength = currentLength;
            currentLength = 0;
        } else {
            ++currentLength;
        }
    }
    
    /* Checking current line length after for loop finished, in case last line the longest */
    if(currentLength > lineLength)
        lineLength = currentLength;

    return lineLength;
}

size_t _gui_numLines(char const * content) {

    /* Returning 0 on an empty string */
    if(strlen(content) < 1 || content[0] == '\0')
        return 0;

    /* Counting lines starting with 1 for the first line */
    size_t count = 1;

    /* Going through string until no more newlines found */
    char * newline = strchr(content, '\n');
    while(newline != NULL) {
        newline = strchr((newline+1), '\n');
        ++count;
    }

    return count;
}

void _gui_updateAutoSize(gui_dialog_t * dialog) {

    /* Stopping if neither size is auto */
    if(dialog->sizeX != GUI_SIZE_AUTO && dialog->sizeY != GUI_SIZE_AUTO)
        return;

    /* Setting the real width */
    if(dialog->sizeX == GUI_SIZE_AUTO) {
        dialog->realWidth = util_maxInt((int)_gui_maxLineLength(dialog->text), (int)_gui_maxLineLength(dialog->title)) + 2;
    }

    /* Setting the real height */
    if(dialog->sizeY == GUI_SIZE_AUTO) {
        /* Padding around the text content */
        dialog->realHeight = (strlen(dialog->text) > 0 ? 2 : 1);
        /* Title height */
        dialog->realHeight += _gui_numLines(dialog->title);
        /* Text content height */
        dialog->realHeight += _gui_numLines(dialog->text);
    }

    /* Updating real width and height according to buttons */
    if(dialog->buttonLayout == BUTTON_LINE) {
        /* BUTTON_LINE case */
        int buttonLine = 1;
        int maxHeight = 0;
        for(int idx = 0; idx < dialog->numButtons; ++idx) {
            button_t button = dialog->buttons[idx];
            buttonLine += button.sizeX + 1;
            if(button.sizeY > maxHeight)
                maxHeight = button.sizeY;
        }

        if(dialog->sizeX == GUI_SIZE_AUTO)
            dialog->realWidth = util_maxInt(dialog->realWidth, buttonLine);
        if(dialog->sizeY == GUI_SIZE_AUTO)
            dialog->realHeight += maxHeight + (maxHeight > 0 ? 1 : 0);
    
    } else if(dialog->buttonLayout == BUTTON_BREAK) {
        /* BUTTON_BREAK case */
        int maxWidth = 0;
        for(int idx = 0; idx < dialog->numButtons; ++idx) {
            button_t button = dialog->buttons[idx];
            if(button.sizeX > maxWidth)
                maxWidth = button.sizeX;
            if(dialog->sizeY == GUI_SIZE_AUTO)
                dialog->realHeight += button.sizeY + 1;
        }

        if(dialog->sizeX == GUI_SIZE_AUTO) {
            dialog->realWidth = util_maxInt(dialog->realWidth, (maxWidth+2));
        }
    }
}

void _gui_printLines(char const * text, int textColor, int background, int fillColor, int * x, int * y, int originX, int originY, int width, int height, int indent) {

    /* Move to line print start point */
    int startX = *x;
    cursorMoveTo(*x + originX, *y + originY);

    /* Printing indent to the start of the line */
    if(indent  > 0 && fillColor != NO_CODE) {
        modeSet(NO_CODE, NO_CODE, fillColor);
        for(int i = 0; i < indent; ++i) {
            putchar(' ');
            ++(*x);
        }
    }

    /* Set style */
    modeSet(NO_CODE, textColor, background);

    /* Going through each character, printing */
    for(size_t i = 0; i < strlen(text) && (*x < width && *y < height); ++i) {
        if(text[i] == '\n') {
            /* Newline found - finish the rest of the line */
            if(fillColor != NO_CODE) {
                modeSet(NO_CODE, NO_CODE, fillColor);
                while(*x < width) {
                    putchar(' ');
                    ++(*x);
                }
            }
            /* Start new line coords */
            *x = startX;
            ++(*y);
            /* Move cursor to new line coords */
            cursorMoveTo(*x + originX, *y + originY);

            /* Indent new line if desired */
            if(indent > 0 && fillColor != NO_CODE) {
                modeSet(NO_CODE, NO_CODE, fillColor);
                for(int i = 0; i < indent; ++i) {
                    putchar(' ');
                    ++(*x);
                }
            }

            /* Resetting style to expected button style */
            modeSet(NO_CODE, textColor, background);

        } else {
            /* Print character and increment counters*/
            putchar(text[i]);
            ++(*x);
        }
    }

    /* Completing line with background color */
    if(fillColor != NO_CODE) {
        modeSet(NO_CODE, NO_CODE, fillColor);
        while(*x < width) {
            putchar(' ');
            ++(*x);
        }
    }
    /* Starting on next line */
    *x = startX;
    ++(*y);
    /* Moving cursor to next line to sync with coords */
    cursorMoveTo(*x + originX, *y + originY);
}
