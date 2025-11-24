#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
// For string function
#include <string.h>
// For string validations
#include <ctype.h>
// For memory allocation
#include <stdlib.h>

// Header Library
#include "Delete.h"

#define LINE_MAX 512
#define ID_LEN 7
#define MAX_NAME_LEN 100
#define MAX_PROG_LEN 100

// Helper function to remove leading and trailing spaces
static void trim(char* string) {
    // Does nothing for NULL pointer
    if (!string) {
        return;
    }
    // Find first non-space character
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

// Return 1 for any failure, and 0 for success
int delete_record(FILE* fp) {
    if (fp == NULL) {
        printf("Error: File not opened.\n");
        return 1;
    }

    char id[50];
    printf("\n--- Delete Student Record ---\n");
    printf("Enter Student ID to delete: ");
    read_line(id, sizeof(id));
    trim(id);

    if (!valid_id(id)) {
        printf("Invalid ID. Must be exactly 7 digits.\n");
        return 1;
    }

    // Move pointer to start of file
    rewind(fp);

    // Create temporary file
    FILE* temp = tmpfile();
    if (!temp) {
        printf("Error: Unable to create temporary storage.\n");
        return 1;
    }

    char line[LINE_MAX];
    // Counter to check if record exists
    int found = 0;

    // Variables used later for confirmation prompts
    char del_name[MAX_NAME_LEN];
    char del_prog[MAX_PROG_LEN];
    float del_mark;

    rewind(fp);

    // Copy anything before header line
    while (fgets(line, sizeof(line), fp)) {
        // Copy header line till field header
        if (strstr(line, "ID") && strstr(line, "Name") && strstr(line, "Programme")) {
            break;
        }   
        fputs(line, temp);
    }

    // Copy the field header
    fputs(line, temp);

    // Process student records
    while (fgets(line, sizeof(line), fp)) {

        // Ignore blank lines
        if (strlen(line) < ID_LEN)
            continue;

        // Extract the 7 digit IDs from the record line
        char rec_id[ID_LEN + 1];
        sscanf(line, "%7s", rec_id);

        // Check if record ID matches ID to delete
        if (strcmp(rec_id, id) == 0) {
            found = 1;

            // Extract fields from line
            char* ptr = line + ID_LEN;
            // Skip spaces after ID
            while (*ptr == ' ') {
                ptr++;
            }

            // copy rest of the line after ID
            char copy[LINE_MAX];
            strcpy(copy, ptr);

            // Extract value of mark
             // Find last occurence of space, which is always before mark
            char* last_space = strrchr(copy, ' ');
            // Convert string after last space to float
            // Cut value of mark from record line
            del_mark = atof(last_space + 1);
            *last_space = '\0';

            // Extract value of programme by searching backwards
             // Split remaining string into name (left) and programme (right)
            char* start_prog = NULL;
            for (int i = strlen(copy) - 1; i > 0; i--) {
                if (copy[i] == ' ' && copy[i - 1] == ' ') {
                    // Terminate name before the two spaces
                    copy[i - 1] = '\0';
                    // Skip the spaces to find start of programme
                    start_prog = &copy[i];
                    // Programme begins at first non-space after position i
                    while (*start_prog == ' ') {
                        start_prog++;
                    }
                    break;
                }
            }
            // Store extracted fields and trim
            if (start_prog) {
                strcpy(del_name, copy);
                strcpy(del_prog, start_prog);
                trim(del_name);
                trim(del_prog);
            }
            continue;
        }

        // Copy the rest of the records
        fputs(line, temp);
    }

    // Back to main menu if record don't exits
    if (!found) {
        printf("Record with ID %s not found.\n", id);
        rewind(fp);
        fclose(temp);
        return 1;
    }

    // Display confirmation prompts
    char confirm[50];
    printf("\nRecord found:\n");
    printf("ID       : %s\n", id);
    printf("Name     : %s\n", del_name);
    printf("Programme: %s\n", del_prog);
    printf("Mark     : %.2f\n", del_mark);

    printf("\nConfirm delete record? (YES to confirm): ");
    read_line(confirm, sizeof(confirm));

    // Convert confirmation to uppercase
    for (int i = 0; confirm[i]; i++) {
        confirm[i] = toupper(confirm[i]);
    }

    // Delete only after user type YES, regardless of case
    if (strcmp(confirm, "YES") != 0) {
        printf("Delete cancelled.\n");
        fclose(temp);
        rewind(fp);
        return 1;
    }

    // Set pointer of both file to start
    rewind(fp);
    rewind(temp);

    // Clear original file by opeining using w+
    freopen(NULL, "w+", fp);

    // Copy data from temp file to original file
    while (fgets(line, sizeof(line), temp)) {
        fputs(line, fp);
    }
        

    fflush(fp);
    rewind(fp);
    fclose(temp);

    printf("\n=== Record Successfully Deleted ===\n");
    printf("Deleted ID : %s\n", id);
    printf("===================================\n\n");

    return 0;
}
