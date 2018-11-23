
/*
// Projeto SO - exercise 1, version 1
// Sistemas Operativos, DEI/IST/ULisboa 2018-19
*/

#include "../lib/commandlinereader.h"
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
#include <sys/time.h>

#define PIPE_NAME "/tmp/servidor"

#define COMMAND_EXIT "exit"
#define COMMAND_RUN "run"

#define MAXARGS 3
#define BUFFER_SIZE 100

int main(int argc, char **argv)
{
    int fdClient;
    fd_set readFdSet;

    unlink(PIPE_NAME);

    if (mkfifo(PIPE_NAME, 0777) < 0)
    { //FIXME: change permissions
        perror("error mkfifo");
        exit(EXIT_FAILURE);
    }

    if ((fdClient = open(PIPE_NAME, O_RDONLY | O_NONBLOCK)) == -1)
    {
        perror("error open fifo");
        exit(EXIT_FAILURE);
    }

    printf("Welcome to CircuitRouter-AdvShell\n\n");

    while (1)
    {
        int selRet;
        int fdInput;
        int maxFd = fdClient;

        printf("%ld\n" , &readFdSet);
        FD_ZERO(&readFdSet);
        FD_SET(fdClient, &readFdSet);
        FD_SET(0, &readFdSet);

        selRet = select(fdClient + 1, &readFdSet, 0, 0, 0);
        if (selRet < 0)
        {
            perror("select");
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(0, &readFdSet))
        {
            fdInput = 0;
            char c;
            while((c = getchar()) != EOF && c != '\n') {printf("%s");} // consume input
        }
        else if (FD_ISSET(fdClient, &readFdSet))
        {
            char buf[124];
            printf("--%d\n", fdClient);
            int res = read(fdClient, buf, 124);
            printf("%s\n", buf);
            fdInput = fdClient;
            FD_CLR(fdClient, &readFdSet);
        }
        else
        {
            perror("error");
            exit(EXIT_FAILURE);
        }

    }

    close(fdClient);
    unlink(PIPE_NAME);

    return EXIT_SUCCESS;
}
