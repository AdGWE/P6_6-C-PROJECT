#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#define BUFFER_SIZE 16384 //16 Kilo Bytes
#define TIME_BUFFER_SIZE 30
#define BACKUP_NAME "_backup.txt"
#define TEMPORARY_FILE_NAME "tmp.txt"

int Backup_File(FILE *original_file_pointer) {
	char *backup_naming_scheme = BACKUP_NAME;
	char *backup_file_name;
	char *read_buffer = NULL;
	char formatted_time_string[TIME_BUFFER_SIZE];
	int return_code = 0;
	FILE *backup_file_pointer = NULL;
	size_t read_bytes;

	//Get time to set as file name
	time_t get_time;
	struct tm* time_struct;
	time(&get_time);
	time_struct = localtime(&get_time);
	//Time if formatted as hour+minute+second_day+month+year
	strftime(formatted_time_string, sizeof(formatted_time_string), "%H%M%S_%d%m%Y", time_struct);

	//Allocate memory for backup file name to concat the two strings
	int backup_buffer_length = strlen(formatted_time_string) + strlen(backup_naming_scheme) + 1;
	backup_file_name = (char *)calloc(backup_buffer_length, sizeof(char));
	if (backup_file_name == NULL) {
		printf("Unable to create backup file!\n");
		return 1;
	}
	snprintf(backup_file_name, backup_buffer_length, "%s%s", formatted_time_string, backup_naming_scheme);

	//Open or create backup file
	backup_file_pointer = fopen(backup_file_name, "wb");
	if (backup_file_pointer == NULL) {
		printf("Unable to open backup file!\n");
		return_code = 1;
		goto free_memory;
	}
	//Allocate read buffer
	read_buffer = (char*)malloc(BUFFER_SIZE);
	if (read_buffer == NULL) {
		printf("Cannot allocate memory to read file!\n");
		return_code = 1;
		goto free_memory;
	}

	// Read from original file and write to backup file
	while ((read_bytes = fread(read_buffer, 1, BUFFER_SIZE, original_file_pointer)) > 0) {
		if (fwrite(read_buffer, 1, read_bytes, backup_file_pointer) != read_bytes) {
			printf("Failed to write to backup file!\n");
			return_code = 1;
			goto free_memory;
		}
	}
	
	//Free readbuffer & close file
	printf("Backup successfully saved!\n");
	goto free_memory;

free_memory:
	free(backup_file_name);
	free(read_buffer);
	rewind(original_file_pointer);
	if (backup_file_pointer != NULL) {
		fclose(backup_file_pointer);
	}
	return return_code;
}


int Create_Temporary_File(FILE* original_file_pointer) {
	char* temp_filename = TEMPORARY_FILE_NAME;
	char* read_buffer = NULL;
	int return_code = 0;
	FILE* copy_file_pointer = NULL;
	size_t read_bytes;

	//Open or create backup file
	copy_file_pointer = fopen(temp_filename, "wb");
	if (copy_file_pointer == NULL) {
		printf("Unable to open backup file!\n");
		return_code = 1;
		goto free_memory;
	}
	//Allocate read buffer
	read_buffer = (char*)malloc(BUFFER_SIZE);
	if (read_buffer == NULL) {
		printf("Cannot allocate memory to read file!\n");
		return_code = 1;
		goto free_memory;
	}

	// Read from original file and write to backup file
	while ((read_bytes = fread(read_buffer, 1, BUFFER_SIZE, original_file_pointer)) > 0) {
		if (fwrite(read_buffer, 1, read_bytes, copy_file_pointer) != read_bytes) {
			printf("Failed to write to backup file!\n");
			return_code = 1;
			goto free_memory;
		}
	}

	//Free readbuffer & close file
	goto free_memory;

free_memory:
	free(read_buffer);
	rewind(original_file_pointer);
	if (copy_file_pointer != NULL) {
		fclose(copy_file_pointer);
	}
	rewind(original_file_pointer);
	return return_code;
}
