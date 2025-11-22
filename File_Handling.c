#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "Backup_file.h"

char* temp_filename = "tmp.txt";

FILE* Open_File(char* file_name, char* file_operation) {
	FILE* file_pointer = fopen(file_name, file_operation);
	if (file_pointer == NULL) {
		printf("Make sure %s exists in same directory!\n", file_name);
	}
	else {
		printf("Successfully opened %s\n", file_name);
	}
	return file_pointer;
}

int Save_File(FILE *file_pointer) {
	if (file_pointer != NULL) {
		//Make sure all changes are saved to file
		fflush(file_pointer);
		//Remove old tmp file
		remove(temp_filename);
		//Make a new copy as the new tmp.file to edit
		Copy_File(file_pointer);
		printf("File successfully saved!\n");
	}
	else
	{
		printf("Error! Cannot save file!");
	}
	return 0;
}

int Revert_Changes(FILE* file_pointer, char *file_name) {
	//Ensure filepointer is closed first before changing file name
	fclose(file_pointer);

	//Remove the existing file and rename tmp file to the original file name
	if (remove(file_name) == 0 && rename(temp_filename, file_name) == 0) {
		printf("Changes have been successfully reverted!\n");
		return 0;
	} else {
		printf("Unable to find tmp.txt!\n");
		perror("Error reverting changes (rename failed)");
		return 1;
	}
}

int Close_File(FILE* file_pointer) {
	fclose(file_pointer);
	remove(temp_filename);
	return 0;
}

