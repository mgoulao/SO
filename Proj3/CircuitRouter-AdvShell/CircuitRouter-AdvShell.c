
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

#define PIPE_NAME "/tmp/servidor"

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

int main(int argc, char **argv)
{

    char *args[MAXARGS + 1];
    char buffer[BUFFER_SIZE];
    int MAXCHILDREN = -1;
    vector_t *children;
    int runningChildren = 0;

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
        int numArgs;
        int selRet;
        FILE *fpInput;

        FD_ZERO(&readFdSet);
        FD_SET(0, &readFdSet);
        FD_SET(fdClient, &readFdSet);

        while ((selRet = select(FD_SETSIZE, &readFdSet, NULL, NULL, NULL)) < 0)
        {
            if (errno == EINTR)
                continue;
            perror("select");
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(0, &readFdSet))
        {
            fpInput = stdin;
        }
        else if (FD_ISSET(fdClient, &readFdSet))
        {
            printf("test\n");
            fpInput = fpClient;
        }
        else
        {
            perror("error");
            exit(EXIT_FAILURE);
        }

        numArgs = readLineArguments(args, MAXARGS + 1, buffer, BUFFER_SIZE, fpInput);

        /* EOF (end of file) do stdin ou comando "sair" */
        if (numArgs < 0 || (numArgs > 0 && (strcmp(args[0], COMMAND_EXIT) == 0)))
        {
            printf("CircuitRouter-AdvShell will exit.\n--\n");

            /* Espera pela terminacao de cada filho */
            while (runningChildren > 0)
            {
                waitForChild(children);
                runningChildren--;
            }

            printChildren(children);
            printf("--\nCircuitRouter-AdvShell ended.\n");
            break;
        }

        else if (numArgs > 0 && strcmp(args[0], COMMAND_RUN) == 0)
        {
            int pid;
            if (numArgs < 2)
            {
                printf("%s: invalid syntax. Try again.\n", COMMAND_RUN);
                continue;
            }
            if (MAXCHILDREN != -1 && runningChildren >= MAXCHILDREN)
            {
                waitForChild(children);
                runningChildren--;
            }

            pid = fork();
            if (pid < 0)
            {
                perror("Failed to create new process.");
                exit(EXIT_FAILURE);
            }

            if (pid > 0)
            {
                runningChildren++;
                printf("%s: background child started with PID %d.\n\n", COMMAND_RUN, pid);
                continue;
            }
            else
            {
                char seqsolver[] = "../CircuitRouter-SeqSolver/CircuitRouter-SeqSolver";
                char *newArgs[3] = {seqsolver, args[1], NULL};

                execv(seqsolver, newArgs);
                perror("Error while executing child process"); // Nao deveria chegar aqui
                exit(EXIT_FAILURE);
            }
        }

        else if (numArgs == 0)
        {
            /* Nenhum argumento; ignora e volta a pedir */
            continue;
        }
        else
            printf("Unknown command. Try again.\n");
    }

    close(fdClient);
    unlink(PIPE_NAME);

    for (int i = 0; i < vector_getSize(children); i++)
    {
        free(vector_at(children, i));
    }
    vector_free(children);

    return EXIT_SUCCESS;
}
