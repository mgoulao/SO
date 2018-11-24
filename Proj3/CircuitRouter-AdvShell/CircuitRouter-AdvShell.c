
/*
// Projeto SO - exercise 1, version 1
// Sistemas Operativos, DEI/IST/ULisboa 2018-19
*/

#include "lib/commandlinereader.h"
#include "lib/vector.h"
#include "CircuitRouter-AdvShell.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <limits.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <fcntl.h>

#define COMMAND_EXIT "exit"
#define COMMAND_RUN "run"

#define MAXARGS 3
#define BUFFER_SIZE 124

void waitForChild(vector_t *children)
{
    while (1)
    {
        child_t *child = malloc(sizeof(child_t));
        if (child == NULL)
        {
            perror("Error allocating memory");
            exit(EXIT_FAILURE);
        }
        child->pid = wait(&(child->status));
        if (child->pid < 0)
        {
            if (errno == EINTR)
            {
                /* Este codigo de erro significa que chegou signal que interrompeu a espera
                   pela terminacao de filho; logo voltamos a esperar */
                free(child);
                continue;
            }
            else
            {
                perror("Unexpected error while waiting for child.");
                exit(EXIT_FAILURE);
            }
        }
        vector_pushBack(children, child);
        return;
    }
}

void printChildren(vector_t *children)
{
    for (int i = 0; i < vector_getSize(children); ++i)
    {
        child_t *child = vector_at(children, i);
        int status = child->status;
        pid_t pid = child->pid;
        if (pid != -1)
        {
            const char *ret = "NOK";
            if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
            {
                ret = "OK";
            }
            printf("CHILD EXITED: (PID=%d; return %s)\n", pid, ret);
        }
    }
    puts("END.");
}

int advShell(FILE *fpInput, int MAXCHILDREN, int *runningChildrenPtr, vector_t *children)
{
    int requestFromPipe = (fpInput == stdin) ? 0 : 1;
    int numArgs;
    char *args[MAXARGS + 1];
    char buffer[BUFFER_SIZE];
    FILE *fpResponse;

    numArgs = readLineArguments(args, MAXARGS + 1, buffer, BUFFER_SIZE, fpInput);

    if (requestFromPipe && ((fpResponse = fopen(args[numArgs - 1], "w")) == NULL))
    {
        perror("error fopen");
        exit(EXIT_FAILURE);
    }

    if (!(strcmp(args[0], COMMAND_RUN) == 0) && requestFromPipe)
    {
        fprintf(fpResponse, "%s\n", "Command not supported.");
        fflush(fpResponse);
        fclose(fpResponse);
        return 0;
    }

    else if (numArgs < 0 || (numArgs > 0 && (strcmp(args[0], COMMAND_EXIT) == 0)))
    {
        printf("CircuitRouter-AdvShell will exit.\n--\n");

        /* Espera pela terminacao de cada filho */
        printf("--exit- %d\n", (*runningChildrenPtr));
        while ((*runningChildrenPtr) > 0)
        {
            waitForChild(children);
            (*runningChildrenPtr)--;
        }

        printChildren(children);
        printf("--\nCircuitRouter-AdvShell ended.\n");
        return -1;
    }

    else if (numArgs > 0 && strcmp(args[0], COMMAND_RUN) == 0)
    {
        int pid;
        if (requestFromPipe && numArgs < 3)
        {
            fprintf(fpResponse, "%s: invalid syntax. Try again.\n", COMMAND_RUN);
            fflush(fpResponse);
            fclose(fpResponse);
            return 0;
        }
        else if (numArgs < 2)
        {
            printf("%s: invalid syntax. Try again.\n", COMMAND_RUN);
            return 0;
        }
        if (MAXCHILDREN != -1 && (*runningChildrenPtr) >= MAXCHILDREN)
        {
            waitForChild(children);
            (*runningChildrenPtr)--;
        }

        pid = fork();
        if (pid < 0)
        {
            perror("Failed to create new process.");
            exit(EXIT_FAILURE);
        }

        if (pid > 0)
        {
            (*runningChildrenPtr)++;
            printf("%s: background child started with PID %d.\n\n", COMMAND_RUN, pid);
            if (requestFromPipe)
                fclose(fpResponse);
            return 0;
        }
        else
        {
            if (requestFromPipe)
            {
                int fdResponse = fileno(fpResponse);
                close(1);
                dup(fdResponse);
                fclose(fpResponse);
            }

            char seqsolver[] = "../CircuitRouter-SeqSolver/CircuitRouter-SeqSolver";
            char *newArgs[3] = {seqsolver, args[1], NULL};

            execv(seqsolver, newArgs);
            perror("Error while executing child process");
            exit(EXIT_FAILURE);
        }
    }

    else if (numArgs == 0)
    {
        /* Nenhum argumento; ignora e volta a pedir */
        return 0;
    }
    else
        printf("Unknown command. Try again.\n");

    return 0;
}

int main(int argc, char **argv)
{

    int MAXCHILDREN = -1;
    vector_t *children;
    int runningChildrenPtr = 0;

    FILE *fpClient;
    int fdClient;
    fd_set readFdSet;

    if (argv[1] != NULL)
    {
        MAXCHILDREN = atoi(argv[1]);
    }

    children = vector_alloc(MAXCHILDREN);

    unlink(PIPE_NAME);

    if (mkfifo(PIPE_NAME, 0777) < 0)
    { //FIXME: change permissions
        perror("error mkfifo");
        exit(EXIT_FAILURE);
    }

    if ((fpClient = fopen(PIPE_NAME, "r+")) == NULL)
    {
        perror("error open fifo");
        exit(EXIT_FAILURE);
    }

    fdClient = fileno(fpClient);

    printf("Welcome to CircuitRouter-AdvShell\n\n");

    while (1)
    {
        int selRet;

        FD_ZERO(&readFdSet);
        FD_SET(0, &readFdSet);
        FD_SET(fdClient, &readFdSet);

        while ((selRet = select(fdClient + 1, &readFdSet, NULL, NULL, NULL)) < 0)
        {
            if (errno == EINTR)
                continue;
            perror("select");
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(0, &readFdSet))
        {
            if (advShell(stdin, MAXCHILDREN, &runningChildrenPtr, children) == -1)
                break;
        }
        if (FD_ISSET(fdClient, &readFdSet))
        {
            if (advShell(fpClient, MAXCHILDREN, &runningChildrenPtr, children) == -1)
                break;
        }
    }

    fclose(fpClient);
    unlink(PIPE_NAME);

    for (int i = 0; i < vector_getSize(children); i++)
    {
        free(vector_at(children, i));
    }
    vector_free(children);

    return EXIT_SUCCESS;
}
