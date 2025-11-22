#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

FILE *Open_File(char *file_name,char *file_operation) {
	FILE* file_pointer = fopen(file_name, file_operation);
	if (file_pointer == NULL) {
		printf("Make sure %s exists in same directory!\n", file_name);
	}
	else {
		printf("Successfully opened %s\n", file_name);
	}
	return file_pointer;
}
