
/*
// Projeto SO - exercise 1, version 1
// Sistemas Operativos, DEI/IST/ULisboa 2018-19
*/

#include "lib/commandlinereader.h"
#include "lib/vector.h"
#include "lib/timer.h"
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

vector_t *children;
int runningChildren;

void sigintHandler()
{
    int status;
    int pid = waitpid(-1, &status, WNOHANG);
    if (pid == -1)
    {
        perror("error waitpid");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
        return; //Nenhum filho acabou
    else
    {
        for (int i = 0; i < vector_getSize(children); ++i)
        {
            child_t *child = vector_at(children, i);
            if (child->pid == pid)
            {
                TIMER_READ(child->endTime);
                child->status = status;
                runningChildren--;
                break;
            }
        }
    }
}

void waitForChild(vector_t *children)
{
    while (1)
    {
        int status;
        int pid = wait(&status);
        if (pid < 0)
        {
            if (errno == EINTR)
            {
                /* Este codigo de erro significa que chegou signal que interrompeu a espera
                   pela terminacao de filho; logo voltamos a esperar */
                continue;
            }
            else
            {
                perror("Unexpected error while waiting for child.");
                exit(EXIT_FAILURE);
            }
        }

        for (int i = 0; i < vector_getSize(children); ++i)
        {
            child_t *child = vector_at(children, i);
            if (child->pid == pid)
            {
                TIMER_READ(child->endTime);
                child->status = status;
            }
        }
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
            printf("CHILD EXITED: (PID=%d; return %s; %f s)\n", pid, ret, TIMER_DIFF_SECONDS(child->startTime, child->endTime));
        }
    }
    puts("END.");
}

void runCommand(int MAXCHILDREN, FILE *fpResponse, int requestFromPipe, int numArgs, char* inputFile)
{
    int pid;
    if (requestFromPipe && numArgs < 3)
    {
        fprintf(fpResponse, "%s: invalid syntax. Try again.\n", COMMAND_RUN);
        fflush(fpResponse);
        fclose(fpResponse);
        return;
    }
    else if (numArgs < 2)
    {
        printf("%s: invalid syntax. Try again.\n", COMMAND_RUN);
        return;
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
        child_t *child = malloc(sizeof(child_t));
        if (child == NULL)
        {
            perror("Error allocating memory");
            exit(EXIT_FAILURE);
        }

        child->pid = pid;
        TIMER_READ(child->startTime);
        vector_pushBack(children, child);

        if (requestFromPipe)
            fclose(fpResponse);
        return;
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
        char *newArgs[3] = {seqsolver, inputFile, NULL};

        execv(seqsolver, newArgs);
        perror("Error while executing child process");
        exit(EXIT_FAILURE);
    }
}

void exitCommand()
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
}

int advShell(FILE *fpInput, int MAXCHILDREN, vector_t *children)
{
    int requestFromPipe = (fpInput == stdin) ? 0 : 1;
    int numArgs;
    char *args[MAXARGS + 1];
    char buffer[BUFFER_SIZE];
    FILE *fpResponse;

    numArgs = readLineArguments(args, MAXARGS + 1, buffer, BUFFER_SIZE, fpInput);

    while (requestFromPipe && ((fpResponse = fopen(args[numArgs - 1], "w")) == NULL))
    {
        if(errno == EINTR)
            continue;
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
        exitCommand();
        return -1;
    }

    else if (numArgs > 0 && strcmp(args[0], COMMAND_RUN) == 0)
    {
        runCommand(MAXCHILDREN, fpResponse, requestFromPipe, numArgs, args[1]);
        return 0;
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
    struct sigaction act;
    act.sa_handler = &sigintHandler;
    act.sa_flags = SA_RESTART;
    sigfillset(&act.sa_mask);
    if (sigaction(SIGCHLD, &act, NULL) == -1)
    {
        perror("Error: cannot handle SIGCHLD");
        exit(EXIT_FAILURE);
    }

    int MAXCHILDREN = -1;
    runningChildren = 0;

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

    if ((fpClient = fopen(PIPE_NAME, "r+")) == NULL) //ASK: Se faz sentido verificar EINTR
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
            if (advShell(stdin, MAXCHILDREN, children) == -1)
                break;
        }
        if (FD_ISSET(fdClient, &readFdSet))
        {
            if (advShell(fpClient, MAXCHILDREN, children) == -1)
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
