#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PIPE_NAME "/tmp/servidor"

#define BUFF_SIZE 124

int main(int argc, char *argv[])
{
	char *command, *inputFile;
	FILE* fpAdvShell;

	if (argv[1] != NULL && argv[2] != NULL)
	{
		command = argv[1];
		inputFile = argv[2];
	}

	if ((fpAdvShell = fopen(PIPE_NAME, "w")) == NULL)
	{
		perror("error open fifo");
		exit(EXIT_FAILURE);
	}

	int buffSize = strlen(command) + strlen(inputFile) + 2;
	char* buff = (char*)malloc(buffSize);
	if(buff == NULL) {
		perror("error buff malloc");
		exit(EXIT_FAILURE);
	}
	strcpy(buff, command);
	strcat(buff, " ");
	strcat(buff, inputFile);
	
	printf("%s\n", buff);
	fprintf(fpAdvShell, "%s\n", buff);
	fflush(fpAdvShell);

	free(buff);
	fclose(fpAdvShell);

	return 0;
}
