#include <string.h>
#include <stdlib.h>


/* =============================================================================
 * strConcatenate
 * =============================================================================
 */
char* strConcatenate(char* str1, char* str2) {
	char* concat = (char*)malloc(strlen(str1) + strlen(str2) + 1);

	strcpy(concat, str1);
	strcat(concat, str2);
	return concat;
}