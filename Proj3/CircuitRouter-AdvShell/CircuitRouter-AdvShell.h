#ifndef CIRCUITROUTER_SHELL_H
#define CIRCUITROUTER_SHELL_H

#include "lib/vector.h"
#include "lib/timer.h"
#include <sys/types.h>

#define PIPE_NAME "/tmp/CircuitRouter-AdvShell.pipe"

typedef struct {
    pid_t pid;
    int status;
    TIMER_T startTime;
    TIMER_T endTime;
} child_t;

void waitForChild(vector_t *children);
void printChildren(vector_t *children);

#endif /* CIRCUITROUTER_SHELL_H */
