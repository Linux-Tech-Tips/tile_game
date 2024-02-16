/** 
 * @file title.h
 * @author Martin (Linux-Tech-Tips)
 * @brief Header file declaring data structures and functions related to the main/title screen task (data.h/TASK_MAIN)
*/

#ifndef TASK_TITLE_H
#define TASK_TITLE_H

#include <string.h>
#include "terminal_gui/terminal_f.h"

#include "data.h"

/** The main task function for the Title task, consisting of task init, task loop and task termination */
nextTask_t title_task(programData_t * data);

#endif /* TASK_TITLE_H */