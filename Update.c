#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
// For string functions
#include <string.h>
// For dynamic memory allocation
#include <stdlib.h>
// For character type functions
#include <ctype.h>

// Include header file
#include "Update.h"

//Declare variables
#define name_len 100
#define programme_len 100
#define skip_update "skip"
 
// Function to trim whitespace from both ends of string
void trim(char* str) {
	int len = strlen(str);

	// Trim trailing spaces
	while (len > 0 && isspace(str[len - 1])) str[--len] = '\0';

	// Trim leading spaces
	char* start = str;
	while (*start && isspace(*start)) start++;

	if (start != str) memmove(str, start, strlen(start) + 1);
}

// Function to normalize spaces in a string (reduce multiple spaces to single space)
// To compat with show_all format
void normalize_spaces(char* s) {
	char* src = s, * dst = s;
	int space_count = 0;
	while (*src) {
		if (*src == ' ') {
			space_count++;
		}
		else {
			space_count = 0;
		}
		if (space_count <= 1) {
			*dst++ = *src;
			src++;
		}
		*dst = '\0';
	}
}

// Function to count the number of lines from the text file
static int count_lines(FILE* fp) {
	int lines = 0;
	// Buffer to store each line
	char buffer[256];

	// Set file position to the beginning of the file
	rewind(fp);

	// Iterate through each line and count
	while (fgets(buffer, sizeof(buffer), fp) != NULL) {
		lines++;
	}

	// Reset file position to the beginning of the file
	rewind(fp);
	return lines;
}

// Function takes in file pointer and user input to update record
void update_record(FILE* fp, char* user_input, const char *filename) {
	// Validate file pointer
	if (!fp) {
		printf("File failed to open.\n");
		return;
	}

	// Extract ID from command
	int update_id;
	if (sscanf(user_input, "UPDATE ID=%d", &update_id) != 1) {
		printf("Invalid UPDATE command format. Use: UPDATE ID=<ID>\n");
		return;
	}

	// Load all records into memory
	// Get total number of records in the file
	int total_lines = count_lines(fp);
	// Exclude the header/description lines
	int total_records = total_lines - 4;

	if (total_records <= 0) {
		printf("CMS: No records available to update.\n");
		return;
	}

	// Read all records into memory as normal arrays
	int* id = malloc(total_records * sizeof(int));
	char (*name)[name_len] = malloc(total_records * sizeof(*name));
	char (*programme)[programme_len] = malloc(total_records * sizeof(*programme));
	float* mark = malloc(total_records * sizeof(float));

	// Rewind file pointer to the beginning of the file
	rewind(fp);

	// Skip header/description lines
	char line[256];
	for (int i = 0; i < 4; i++) {
		fgets(line, sizeof(line), fp);
	}

	// Counter to track number of records read
	int count = 0;
	
	// Read one record at a time and store in the corresponding arrays
	while (count < total_records && fgets(line, sizeof(line), fp)) {
		// Remove newline character
		line[strcspn(line, "\n")] = 0;

		// Pointer to traverse the line
		char* p = line;

		// Field 1: ID
		char* id_token = p;
		// Move until first occurence of double spaces
		while (*p && !(*p == ' ' && p[1] == ' ')) p++;
		// If end of line reached, skip to next line
		if (!*p) continue;
		// Terminate the token
		*p = '\0';
		// Move pointer past the double spaces
		p += 2;
		// Skip any additional spaces
		while (*p == ' ') p++;

		// Field 2: Name
		char* name_token = p;
		while (*p && !(*p == ' ' && p[1] == ' ')) p++;
		if (!*p) continue;
		*p = '\0';
		p += 2;
		while (*p == ' ') p++;

		// Field 3: Programme
		char* programme_token = p;
		while (*p && !(*p == ' ' && p[1] == ' ')) p++;
		if (!*p) continue;
		*p = '\0';
		p += 2;
		while (*p == ' ') p++;

		// Field 4: Mark
		char* mark_token = p;

		id[count] = atoi(id_token);
		strcpy(name[count], name_token);
		strcpy(programme[count], programme_token);
		mark[count] = atof(mark_token);
		// Increase count if exactly four fileds are read
		count++;
	}


	// Array index for updating record
	int index = -1;
	// Iterate through ID array to find matching ID
	for (int i = 0; i < count; i++) {
		if (id[i] == update_id) {
			index = i;
			break;
		}
	}

	if (index == -1) {
		printf("CMS: The record with ID=%d does not exist.\n", update_id);
		goto cleanup;
	}

	// Ask user for new values
	char input[256];
	printf("Type 'skip' to keep the old value.\n");
	// New name value
	printf("Enter new Name: ");
	fgets(input, sizeof(input), stdin);
	// Remove newline
	input[strcspn(input, "\n")] = 0;
	trim(input);
	normalize_spaces(input);
	if (strcmp(input, skip_update) != 0) {
		strcpy(name[index], input);
	}

	// New programme value
	printf("Enter new Programme: ");
	fgets(input, sizeof(input), stdin);
	// Remove newline
	input[strcspn(input, "\n")] = 0;
	trim(input);
	normalize_spaces(input);
	if (strcmp(input, skip_update) != 0) {
		strcpy(programme[index], input);
	}
	

	// New mark value
	printf("Enter new Mark: ");
	fgets(input, sizeof(input), stdin);
	// Remove newline
	input[strcspn(input, "\n")] = 0;
	trim(input);

	// Validate and update mark
	if (strcmp(input, skip_update) != 0) {
		char* endptr;
		float new_mark = strtof(input, &endptr);
		if (endptr == input) {
			printf("CMS: Invalid mark input. Keeping old value.\n");
		}
		else {
			mark[index] = new_mark;
		}
	}

	// Write back updated records to the file
	// Rewind file pointer to the beginning of the file
	fp = freopen(filename, "w", fp);
	if (!fp) {
		printf("CMS: Error reopening file for writing.\n");
		goto cleanup;
	}

	// Write back header/description lines
	fprintf(fp, "Database Name: P6-6 CMS\n");
	fprintf(fp, "Authors: Ashwin Kavidasan, Adriel Goh Wei En, Saranesan So V Saravanan, He Yong Jie\n");
	fprintf(fp, "\n");
	fprintf(fp, "ID  Name  Programme  Mark\n");

	// Iterate through all records and write to file
	for (int i = 0; i < count; i++) {
		fprintf(fp, "%d  %s  %s  %.2f\n",
			id[i],
			name[i],
			programme[i],
			mark[i]);
	}

	printf("CMS: Record with ID=%d has been updated successfully.\n", update_id);

// Free allocated memory
cleanup:

	free(id);
	free(name);
	free(programme);
	free(mark);
}