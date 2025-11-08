#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

FILE *Open_File(char file_name[],char file_operation[]) {
	FILE* file_pointer = fopen(file_name, file_operation);
	if (file_pointer == NULL) {
		printf("Make sure file exists in same directory!\n");
	}
	else {
		printf("Successfully opened P6_6-CMS.txt\n");
	}
	return file_pointer;
}