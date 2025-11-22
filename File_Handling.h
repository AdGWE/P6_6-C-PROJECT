#include <stdio.h>
int Save_File(FILE *file_pointer);

int Revert_Changes(FILE* file_pointer,char *file_name);

int Close_File(FILE *file_pointer);

FILE* Open_File(char *file_name, char *file_operation);