#include <stdio.h>

int Save_File(FILE *file_pointer) {
	if (file_pointer != NULL) {
		fclose(file_pointer);
		printf("P6_6-CMS.txt successfully saved!\n");
	}
	else
	{
		printf("Error! Cannot save file!");
	}
	return 0;
}