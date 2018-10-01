#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib/types.h"
#include "lib/commandlinereader.h"

#define BUFFER_SIZE 80
#define NUM_INPUTS 2

int main(int argc, char *argv[])
{
    int numArgs;
    char **argsVector = (char **)malloc(NUM_INPUTS * sizeof(char *));
    char *buffer = (char *)malloc((BUFFER_SIZE + 1) * sizeof(char));

    while (TRUE)
    {
        numArgs = readLineArguments(argsVector, NUM_INPUTS + 1, buffer, BUFFER_SIZE);
        printf("%d", strcmp("run", argsVector[0]));
        if (strcmp("run", argsVector[0]))
        {
            printf("RUN FOREST RUN!!!!");
        }
        else if (strcmp("exit", argsVector[0]))
        {
            printf("Goodbye");
            break;
        }
        else
        {
            printf("Command is stupid");
        }
    }
    printf("numArgs - %d, arg1: %s, arg2: %s", numArgs, argsVector[0], argsVector[1]);

    return 0;
}

void prompt()
{
}