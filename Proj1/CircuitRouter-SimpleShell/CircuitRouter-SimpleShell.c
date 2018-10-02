#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include "lib/types.h"
#include "lib/commandlinereader.h"

#define BUFFER_SIZE 80
#define NUM_INPUTS 3

void runCommand(char *inputFile, int *numChilds)
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
        if (inputFile != NULL)
        {
            execl("../CircuitRouter-SeqSolver/CircuitRouter-SeqSolver", inputFile, (char *)NULL);
        }
        else
        {
            printf("Input file needed\n");
            exit(1);
        }
    }
}

void exitCommand(int numChilds)
{
    int i, status, pid;
    for (i = 0; i < numChilds; i++)
    {
        pid = wait(&status);
        printf("CHILD EXITED (PID=%d; return %s)\n", pid, (!WIFEXITED(status)) ? "OK" : "NOK");
    }
    printf("END");
}

int main(int argc, char *argv[])
{
    int numArgs, numChilds = 0;
    char **argsVector = (char **)malloc(NUM_INPUTS * sizeof(char *));
    char *buffer = (char *)malloc((BUFFER_SIZE) * sizeof(char));

    while (TRUE)
    {
        numArgs = readLineArguments(argsVector, NUM_INPUTS, buffer, BUFFER_SIZE);
        if (!strcmp("run", argsVector[0]))
        {
            runCommand(argsVector[1], &numChilds);
        }
        else if (!strcmp("exit", argsVector[0]))
        {
            exitCommand(numChilds);
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

    return 0;
}