#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include "lib/types.h"
#include "lib/commandlinereader.h"

#define BUFFER_SIZE 80
#define NUM_INPUTS 2

void runCommand(char *inputFile)
{
    int pid;

    pid = fork();

    if (pid < 0)
    {
        printf("Unable to fork\n");
    }
    else if (pid == 0)
    {
        printf("-----filho\n");
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
    /* else
    {
        wait(&state);
    }  */
}

void exitCommand()
{
    printf("----exit\n");
}

int main(int argc, char *argv[])
{
    int numArgs;
    char **argsVector = (char **)malloc(NUM_INPUTS * sizeof(char *));
    char *buffer = (char *)malloc((BUFFER_SIZE + 1) * sizeof(char));

    while (TRUE)
    {
        numArgs = readLineArguments(argsVector, NUM_INPUTS + 1, buffer, BUFFER_SIZE);
        if (!strcmp("run", argsVector[0]))
        {
            runCommand(argsVector[1]);
        }
        else if (!strcmp("exit", argsVector[0]))
        {
            exitCommand();
            break;
        }
        else
        {
            printf("Invalid command\n");
        }
    }

    return 0;
}