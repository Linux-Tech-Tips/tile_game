#include "data.h"

void data_load(programData_t * data, char const * fileName) {

    /* Open file for binary read */
    FILE * file = fopen(fileName, "rb");

    if(file) {
        /* Read bytes from file into programData */
        int num = fread((void *)(data), sizeof(programData_t), 1, file);
        /* End function here if successfully read */
        if(num > 0) {
            /* Hard-setting certain values */
            data->run = 1;
            clock_gettime(CLOCK_MONOTONIC, &(data->prevTime));
            /* Closing file and ending function */
            fclose(file);
            return;
        }
        /* Print error message and close file if opened but couldn't read */
        fputs("Error Reading Game Data!", stderr);
        fclose(file);
    }
    
    /* Set data defaults if not loaded correctly */
    data_reset(data);
}

void data_save(programData_t data, char const * fileName) {

    /* Open file for binary write */
    FILE * file = fopen(fileName, "wb");

    int num = 0;
    if(file) {
        /* Write data into file */
        num = fwrite((void *)(&data), sizeof(programData_t), 1, file);
    }

    /* Print error message if data not saved */
    if(num < 1) {
        fputs("Error Writing Game Data!", stderr);
    }
}

void data_reset(programData_t * data) {
    data->run = 1;
    data->highScore = 0;
    data->fpsCounter = 0;
    data->alignment = (fieldAlign_t){ALIGN_CENTER, ALIGN_TOP};
}


void data_frameStart(programData_t * data) {
    /* Time now (at the start of the frame) */
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    /* Getting delta time since last frame */
    data->deltaTime = data_timeToSec(now) - data_timeToSec(data->prevTime);
    data->prevTime = now;
}

void data_frameEnd(programData_t * data, int ups) {
    /* Time now (at the end of the frame) */
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    /* Getting frameTime (how long was taken for the current frame) */
    double frameTime = data_timeToSec(now) - data_timeToSec(data->prevTime);
    /* Sleeping additional time to match the desired updates per second value */
    if(frameTime < (1.0f/ups)) {
        long sleepTimeNanosec = ((1.0f/ups) - frameTime) * 1000000000;
        struct timespec sleepTime = (struct timespec){ .tv_sec = (time_t)(sleepTimeNanosec / 1000000000), .tv_nsec = (sleepTimeNanosec % 1000000000) };
        nanosleep(&sleepTime, NULL);
    }
}

double data_timeToSec(struct timespec time) {
    return (double)(time.tv_sec) + (time.tv_nsec/1000000000.0L);
}

short data_validTerm(void) {
    int x = 0, y = 0;
    getTerminalSize(&x, &y);
    return (x >= TERM_MIN_X && y >= TERM_MIN_Y);
}