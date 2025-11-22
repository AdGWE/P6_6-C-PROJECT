#include <stdio.h>

int Save_File(FILE *file_pointer) {
	if (file_pointer != NULL) {
		fclose(file_pointer);
		printf("File successfully saved!\n");
	}
	else
	{
		printf("Error! Cannot save file!");
	}
	return 0;
}

int Revert_Changes(FILE* file_pointer, char *file_name) {
	//get name of temp file
	char* temp_filename = "tmp.txt";

	//Ensure filepointer is closed first before changing file name
	fclose(file_pointer);

	if (remove(file_name) == 0 && rename(temp_filename, file_name) == 0) {
		printf("Changes have been successfully reverted!\n");
		return 0;
	} else {
		printf("Unable to find tmp.txt!\n");
		perror("Error reverting changes (rename failed)");
		return 1;
	}
}
