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

#define PIPE_NAME "/tmp/CircuitRouter-AdvShell.pipe"
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
	char responsePipeName[strlen(pathName) + strlen(placeholder)];
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

	if (mkfifo(responsePipeName, 0777) < 0)
	{ //FIXME: change permissions
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
		fflush(fpAdvShell);

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
	char *responsePipePath;
	FILE *fpAdvShell;
	FILE *fpResponse;

	if (argv[1] != NULL)
	{
		responsePipePath = (char *)malloc(strlen(argv[1]) + 7); // 6*X and \0
		strcpy(responsePipePath, argv[1]);
	}
	else
	{
		showUsage();
		exit(EXIT_FAILURE);
	}

	if ((fpAdvShell = fopen(PIPE_NAME, "w")) == NULL)
	{
		perror("error open fifo");
		exit(EXIT_FAILURE);
	}

	fpResponse = createResponseFifo(responsePipePath);

	clientShell(fpAdvShell, fpResponse, responsePipePath);

	unlink(responsePipePath);
	fclose(fpResponse);
	fclose(fpAdvShell);
	free(responsePipePath);

	return 0;
}