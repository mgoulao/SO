/* =============================================================================
 *
 * CircuitRouter-SimpleShell.c
 *
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include "lib/types.h"
#include "lib/commandlinereader.h"

#define BUFFER_SIZE 100
#define NUM_INPUTS 3

typedef struct
{
    int pid, status;
} ChildProcess;

/* =============================================================================
 * printChildExit
 * =============================================================================
 */
void printChildExit(int pid, int status)
{
    printf("CHILD EXITED (PID=%d; return %s)\n", pid, (WSTOPSIG(status) == 0) ? "OK" : "NOK");
}

/* =============================================================================
 * runCommand
 * =============================================================================
 */
void runCommand(char *inputFile, int *numChilds, int MAX_CHILDS)
{
    int pid;

    pid = fork();

    if (pid < 0)
    {
        printf("Unable to fork\n");
        return;
    }

    (*numChilds)++;
    if (pid == 0)
    {
        execl("../CircuitRouter-SeqSolver/CircuitRouter-SeqSolver", "./CircuitRouter-SeqSolver", inputFile, (char *)NULL);
    }
}

/* =============================================================================
 * exitCommand
 * =============================================================================
 */
void exitCommand(int numChilds, ChildProcess *finishedProc, int nFinishedProc)
{
    int i, status, pid;
    for (i = 0; i < nFinishedProc; i++)
    {
        ChildProcess ps = finishedProc[i];
        printChildExit(ps.pid, ps.status);
    }
    for (i = 0; i < numChilds; i++)
    {
        pid = wait(&status);
        printChildExit(pid, status);
    }
    printf("END\n");
}

/* =============================================================================
 * main
 * =============================================================================
 */
int main(int argc, char *argv[])
{
    int numArgs, numChilds = 0, MAX_CHILDS = 0, nFinishedProc = 0;
    char **argsVector = (char **)malloc(NUM_INPUTS * sizeof(char *));
    char *buffer = (char *)malloc(BUFFER_SIZE * sizeof(char));
    ChildProcess *finishedProc = (ChildProcess*)malloc(0);

    if (argc > 1)
    {
        char *p;
        long ret = strtol(argv[1], &p, 10);

        if (*p != '\0')
            printf("Invalid MAXCHILDREN\n");
        else
        {
            MAX_CHILDS = ret;
            if (MAX_CHILDS < 1)
            {
                printf("MAXCHILDREN must be positive\n");
                exit(1);
            }
        }
    }

    while (TRUE)
    {
        numArgs = readLineArguments(argsVector, NUM_INPUTS, buffer, BUFFER_SIZE);
        if (argsVector[0] == NULL)
        {
            printf("No command\n");
        }
        else if (!strcmp("run", argsVector[0]))
        {
            if (numArgs == 1)
            {
                printf("Input file needed\n");
            }
            else
            {
                if (MAX_CHILDS != 0 && numChilds == MAX_CHILDS)
                {
                    int status, pid;
                    pid = wait(&status);
                    finishedProc = (ChildProcess *)realloc(finishedProc, ++nFinishedProc * sizeof(ChildProcess));
                    ChildProcess ps;
                    ps.pid = pid;
                    ps.status = status;
                    finishedProc[nFinishedProc - 1] = ps;
                    numChilds--;
                }
                runCommand(argsVector[1], &numChilds, MAX_CHILDS);
            }
        }
        else if (!strcmp("exit", argsVector[0]))
        {
            exitCommand(numChilds, finishedProc, nFinishedProc);
            break;
        }
        else
        {
            printf("Invalid command\n");
        }
    }

    /* Clean up */
    free(argsVector);
    free(buffer);
    free(finishedProc);

    return 0;
}