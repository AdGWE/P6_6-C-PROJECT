#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
// For string function
#include <string.h>
// For string validations
#include <ctype.h>
// For memory allocation
#include <stdlib.h>

// Header library
#include "Update.h"

// Buffer size for each line in student file
#define LINE_MAX 512
// for student ID to be 7 digits
#define ID_LEN 7
#define MAX_NAME_LEN 100
#define MAX_PROG_LEN 100

// Helper function to remove leading and trailing spaces
static void trim(char* string) {
    // Does nothing for NULL pointer
    if (!string) {
        return;
    }
    // Skip leading space
    char* start = string;
    while (*start && isspace((unsigned char)*start)) {
        start++;
    }
    // Return empty string if all spaces
    if (*start == '\0') { 
        string[0] = '\0'; return; 
    }

    // Trim trailing spaces
    char* end = start + strlen(start) - 1;
    while (end > start && isspace((unsigned char)*end)) {
        end--;
    }
    *(end + 1) = '\0';

    // Shift trimmed string to the beginning if required
    if (start != string) {
        memmove(string, start, strlen(start) + 1);
    }
}

// Function to check if ID is valid (7 digits)
static int valid_id(const char* string) {
    // Return 0 if ID is less than 7 digit
    if (strlen(string) != 7) {
        return 0;
    }
    // Return 0 if any character of string is not a digit
    for (int i = 0; i < 7; i++) {
        if (!isdigit(string[i])) {
            return 0;
        }
    }      
    return 1;
}

// Buffer overflow protection to read line safely from input
static int read_line(char* buffer, size_t size) {
    if (!fgets(buffer, (int)size, stdin)) {
        return 0;
    }
    // // Remove newline character from fgets input
    buffer[strcspn(buffer, "\n")] = '\0';
    return 1;
}

// Convert string to float for mark, validate if within 0 - 100
static int parse_mark(const char* string, float* mark) {
    char* end;
    float convert = strtof(string, &end);

    // Skip trailing spaces
    while (*end && isspace((unsigned char)*end)) {
        end++;
    }
    // Invalid if theres any leftover characters
    if (*end != '\0') {
        return 0;
    }
    // Validate if within range
    if (convert < 0 || convert > 100) {
        return 0;
    }
    *mark = convert;
    return 1;
}

// ---- MAIN UPDATE FUNCTION ----
// Return 1 for any failure, and 0 for success
int update_record(FILE* fp) {

    // Validate if file is opened
    if (fp == NULL) {
        printf("Error: File not opened. Unable to update.\n");
        return 1;
    }

    // Variable declaration 
    char id[50];
    char line[LINE_MAX];
    // Where to overwrite
    long pos_before_record = -1;
    // Counter to check if record exists
    int found = 0;

    // Prompt user for ID to update
    printf("\n--- Update Student Record ---\n");
    printf("Enter Student ID to update: ");
    read_line(id, sizeof(id));
    // Trim user input to be read by subsequent segment
    trim(id);

    // Validate ID
    if (!valid_id(id)) {
        printf("Invalid ID. Must be exactly 7 number digits.\n");
        // Failed
        return 1;
    }

    // Move to start of file
    rewind(fp);

    // Skip header lines
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, "ID") && strstr(line, "Name") && strstr(line, "Program"))
            break;
    }

    // Search record
    while (1) {
        // Start of current line
        long pos = ftell(fp);

        // Stops when reach the end of file
        if (!fgets(line, sizeof(line), fp)) {
            break;
        }

        // Skip blank / invalid lines
        if (strlen(line) < 7) {
            continue;
        }

        // Extract the 7 digit IDs from the record line
        char rec_id[ID_LEN + 1];
        sscanf(line, "%7s", rec_id);

        // Store file position if value matches.
        if (strcmp(rec_id, id) == 0) {
            found = 1;
            pos_before_record = pos;
            break;
        }
    }

    // If record not found
    if (!found) {
        printf("Record with ID %s not found.\n", id);
        rewind(fp);
        return 1;
    }

    // Extract existing fields from record lines stored
    char old_name[MAX_NAME_LEN];
    char old_prog[MAX_PROG_LEN];
    float old_mark;
    char* ptr;

    strcpy(old_name, "");
    strcpy(old_prog, "");

    // Stores current pointer -> end of ID
    ptr = line + ID_LEN;

    // Skip spaces after ID
    while (*ptr == ' ') {
        ptr++;
    }

    // Copy and store the rest of the fields
    char copy[LINE_MAX];
    strcpy(copy, ptr);

    // Extract value of mark
    // Find last occurence of space, which is always before mark
    char* last_space = strrchr(copy, ' ');
    // Convert string after last space to float
    old_mark = atof(last_space + 1);
    // Cut value of mark from record line
    *last_space = '\0';

    // Extract value of programme by searching backwards
    // Split remaining string into name (left) and programme (right)
    char* start_prog = NULL;
    for (int i = strlen(copy) - 1; i > 0; i--) {
        if (copy[i] == ' ' && copy[i - 1] == ' ') {
            // Skip the spaces to find start of programme
            start_prog = &copy[i];       
            while (*start_prog == ' ') {
                start_prog++;
            }
            // Terminate name before the two spaces
            copy[i] = '\0';
            break;
        }
    }

    // Validate data format
    if (!start_prog) {
        printf("Record format corrupted.\n");
        rewind(fp);
        return 1;
    }

    // Store value of name and programme
    strcpy(old_name, copy);
    strcpy(old_prog, start_prog);

    // Display current data
    printf("\nCurrent Record:\n");
    printf("ID       : %s\n", id);
    printf("Name     : %s\n", old_name);
    printf("Programme: %s\n", old_prog);
    printf("Mark     : %.2f\n", old_mark);

    // Variables to store new value 
    char new_name[MAX_NAME_LEN];
    char new_prog[MAX_PROG_LEN];
    char new_mark_str[50];
    float new_mark;

    printf("\nEnter new name (leave blank to keep existing record): ");
    read_line(new_name, sizeof(new_name));
    trim(new_name);
    if (strlen(new_name) == 0) {
        strcpy(new_name, old_name);
    }

    printf("Enter new programme (leave blank to keep existing): ");
    read_line(new_prog, sizeof(new_prog));
    trim(new_prog);
    if (strlen(new_prog) == 0) {
        strcpy(new_prog, old_prog);
    }

    printf("Enter new mark (0-100, leave blank to keep existing): ");
    read_line(new_mark_str, sizeof(new_mark_str));
    trim(new_mark_str);
    if (strlen(new_mark_str) == 0) {
        new_mark = old_mark;
    }
    // Validate value of mark
    else if (!parse_mark(new_mark_str, &new_mark)) {
        printf("Invalid mark.\n");
        rewind(fp);
        return 1;
    }

    // Write updated record back to file
    fseek(fp, pos_before_record, SEEK_SET);
    // Overwrite old data in place
    fprintf(fp, "%s  %s  %s  %.2f\n", id, new_name, new_prog, new_mark);

    // Ensure changes are saved.
    fflush(fp);
    // Rest pointer to start of file
    rewind(fp);

    // Success message
    printf("\n=== Record Successfully Updated ===\n");
    printf("ID       : %s\n", id);
    printf("Name     : %s\n", new_name);
    printf("Programme: %s\n", new_prog);
    printf("Mark     : %.2f\n", new_mark);
    printf("===================================\n\n");

    return 0;
}
