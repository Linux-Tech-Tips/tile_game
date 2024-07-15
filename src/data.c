#include "data.h"

void data_load(programData_t * data, char const * fileName) {
    data->run = 1;
    clock_gettime(CLOCK_MONOTONIC, &(data->prevTime));
    
    /* TODO STUB, Implement actual function later */
    data_reset(data);
}

void data_save(programData_t data, char const * fileName) {
    /* TODO STUB, Implement function later */
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