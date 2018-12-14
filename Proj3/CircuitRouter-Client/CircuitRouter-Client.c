#include "lib/commandlinereader.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/select.h>

#define COMMAND_EXIT "quit"
#define COMMAND_RUN "run"
#define BUFF_SIZE 124
#define MAXARGS 2

void showUsage()
{
	printf("\nMissing argument\n");
	printf("Usage: Client run input_filename response_pipe_name\n");
}

FILE *createResponseFifo(char *pathName)
{
	char *placeholder = "XXXXXX";
	char responsePipeName[strlen(pathName) + strlen(placeholder) + 1];
	int fdResponse;
	FILE *fpResponse;

	strcpy(responsePipeName, pathName);
	strcat(responsePipeName, placeholder);
	fdResponse = mkstemp(responsePipeName);
	if (fdResponse == -1)
	{
		perror("error mkstemp");
		exit(EXIT_FAILURE);
	}
	close(fdResponse);

	unlink(responsePipeName);

	if (mkfifo(responsePipeName, 0666) < 0)
	{
		perror("error mkfifo");
		exit(EXIT_FAILURE);
	}

	if ((fpResponse = fopen(responsePipeName, "r+")) == NULL)
	{
		perror("error fopen");
		exit(EXIT_SUCCESS);
	}

	strcpy(pathName, responsePipeName);

	return fpResponse;
}

void clientShell(FILE *fpAdvShell, FILE *fpResponse, char *responsePipePath)
{
	char *args[MAXARGS + 1], buffer[BUFF_SIZE], respBuffer[BUFF_SIZE];
	int numArgs;

	printf("\nWelcome to CircuitRouter-Client\n\n");

	while (1)
	{
		numArgs = readLineArguments(args, MAXARGS + 1, buffer, BUFF_SIZE, stdin);

		if (numArgs < 0 || (numArgs > 0 && (strcmp(args[0], COMMAND_EXIT) == 0)))
		{
			printf("--\nCircuitRouter-Client ended.\n");
			break;
		}
		if (numArgs == 1)
			fprintf(fpAdvShell, "%s %s\n", args[0], responsePipePath);
		else
			fprintf(fpAdvShell, "%s %s %s\n", args[0], args[1], responsePipePath);
		if(fflush(fpAdvShell) == EOF) {
            perror("error fflush");
            exit(EXIT_FAILURE);
        }

		if (fgets(respBuffer, BUFF_SIZE, fpResponse) == NULL)
		{
			perror("error fgets");
			exit(EXIT_FAILURE);
		}
		printf("%s\n", respBuffer);
	}
}

int main(int argc, char *argv[])
{
	char *responseStartName = "/tmp/response";
	char *responsePipePath = (char *)malloc(strlen(responseStartName) + 7);
	char *advShellPipeName;
	FILE *fpAdvShell;
	FILE *fpResponse;

	if (argv[1] != NULL)
	{
		advShellPipeName = (char *)malloc(strlen(argv[1])+1);
		strcpy(advShellPipeName, argv[1]);
	}
	else
	{
		showUsage();
		exit(EXIT_FAILURE);
	}

	if ((fpAdvShell = fopen(advShellPipeName, "w")) == NULL)
	{
		perror("error open fifo");
		exit(EXIT_FAILURE);
	}

	strcpy(responsePipePath, responseStartName);
	fpResponse = createResponseFifo(responsePipePath);

	clientShell(fpAdvShell, fpResponse, responsePipePath);

	unlink(responsePipePath);
	fclose(fpResponse);
	fclose(fpAdvShell);
	free(responsePipePath);
	free(advShellPipeName);

	return 0;
}
