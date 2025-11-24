#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "Backup_file.h"
#define TEMPORARY_FILE_NAME "tmp.txt"
char* temp_filename = TEMPORARY_FILE_NAME;

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
		rewind(file_pointer);
		//Remove old tmp file
		remove(temp_filename);
		//Make a new copy, temporary file name defined in copy_file function
		Create_Temporary_File(file_pointer);
		
		printf("File successfully saved!\n");
		return 0;
	} else {
		printf("Error! Cannot save file!\n");
		return 1;

	}
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
		return 1;
	}
}

int Close_File(FILE* file_pointer) {
	//When closing programme, close the file pointer and remove temporary file
	fclose(file_pointer);
	remove(temp_filename);
	return 0;
}

