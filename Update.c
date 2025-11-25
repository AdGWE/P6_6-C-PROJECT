#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
// For string function
#include <string.h>
// For to validate string characters
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

// Allows special characters like hyphen, backslash and full stop.
static int valid_name(const char* string) {
    if (!string || string[0] == '\0') {
        return 0;
    }

    int has_letter = 0;

    for (size_t i = 0; string[i] != '\0'; i++) {
        unsigned char c = (unsigned char)string[i];

        if (isalpha(c)) {
            has_letter = 1;
        }           
        else if (isspace(c)) {
            continue;
        }          
        else if (c == '-' || c == '\'' || c == '.') {
            continue;
        }
        else{
            return 0;
        }    
    }
    return has_letter;
}

// Allows special characters like hyphen, &, slash and brackets
static int valid_programme(const char* string) {
    if (!string || string[0] == '\0') {
        return 0;
    }

    int has_letter = 0;

    for (size_t i = 0; string[i] != '\0'; i++) {
        unsigned char c = (unsigned char)string[i];
        if (isalpha(c)) {
            has_letter = 1;
        }
        else if (isspace(c)) {
            continue;
        }
        else if (c == '-' || c == '&' || c == '/' ||
            c == '(' || c == ')') {
            continue;
        }
        else {
            return 0;
        }
    }
    return has_letter;
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


// Return 1 for any failure, and 0 for success
int update_record(FILE* fp) {

    // Validate if file is opened
    if (fp == NULL) {
        printf("Error: File not opened. Unable to update.\n");
        return 1;
    }

    char id[50];
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

    // Copy anything before header line
    while (fgets(line, sizeof(line), fp)) {
        // Iterate the lines till header line
        if (strstr(line, "ID") && strstr(line, "Name") && strstr(line, "Programme")) {
            break;
        }
        fputs(line, temp);
    }

    // Copy the field header
    fputs(line, temp);

    // Variables to store old data
    char old_name[MAX_NAME_LEN];
    char old_prog[MAX_PROG_LEN];
    float old_mark;

    // Process student records
    while (fgets(line, sizeof(line), fp)) {

        // Ignore blank lines
        if (strlen(line) < ID_LEN)
            continue;

        // Extract the 7 digit IDs from the record line
        char rec_id[ID_LEN + 1];
        sscanf(line, "%7s", rec_id);

        // Store file position if value matches.
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
            old_mark = atof(last_space + 1);
            // Cut value of mark from record line
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

            // Validate data format
            if (!start_prog) {
                printf("Record format corrupted.\n");
                fclose(temp);
                return 1;
            }

            // Store value of name and programme
            strcpy(old_name, copy);
            strcpy(old_prog, start_prog);

            trim(old_name);
            trim(old_prog);

            continue;
        }
        // Copy normally if not the record
        fputs(line, temp);
          
    }

    // If record not found
    if (!found) {
        printf("Record with ID %s not found.\n", id);
        fclose(temp);
        rewind(fp);
        return 1;
    }

    // Display confirmation prompt for current data
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


    // Get new name
    while (1) {
        printf("Enter new name (or KEEP to retain existing): ");
        read_line(new_name, sizeof(new_name));
        trim(new_name);

        if (strcmp(new_name, "KEEP") == 0) {
            strcpy(new_name, old_name);
            break;
        }

        if (!valid_name(new_name)) {
            printf("Invalid name.\n");
            continue;
        }

        if (strlen(new_name) > MAX_NAME_LEN) {
            printf("Name too long. Maximum %d character.\n", MAX_NAME_LEN);
            continue;
        }
        break;
    }

    // Get new programme name
    while (1) {
        printf("Enter new programme (or KEEP to retain existing): ");
        read_line(new_prog, sizeof(new_prog));
        trim(new_prog);

        if (strcmp(new_prog, "KEEP") == 0) {
            strcpy(new_prog, old_prog);
            break;
        }

        if (!valid_programme(new_prog)) {
            printf("Invalid programme.\n");
            continue;
        }

        if (strlen(new_prog) > MAX_PROG_LEN) {
            printf("Programme too long. Maximum %d character.\n", MAX_PROG_LEN);
            continue;
        }
        break;
    }
    
    // Get new mark
    while (1) {
        printf("Enter new mark (0-100, or KEEP to retain existing): ");
        read_line(new_mark_str, sizeof(new_mark_str));
        trim(new_mark_str);

        if (strcmp(new_mark_str, "KEEP") == 0) {
            new_mark = old_mark;
            break;
        }
        if (!parse_mark(new_mark_str, &new_mark)) {
            printf("Invalid mark.\n");
            continue;
        }
        break;
    }

    fprintf(temp, "%s  %s  %s  %.2f\n", id, new_name, new_prog, new_mark);

    // Set pointer of both file to start
    rewind(fp);
    rewind(temp);

    // Clear original file by opeining using w+
    freopen(NULL, "w+", fp);

    // Copy data from temp file to original file
    while (fgets(line, sizeof(line), temp)) {
        fputs(line, fp);
    }
    

    // Write buffer data to drive
    fflush(fp);
    // Rest pointer to start of file
    rewind(fp);
    fclose(temp);

    // Success message and confirmation prompt
    printf("\n=== Record Successfully Updated ===\n");
    printf("ID       : %s\n", id);
    printf("Name     : %s\n", new_name);
    printf("Programme: %s\n", new_prog);
    printf("Mark     : %.2f\n", new_mark);
    printf("===================================\n\n");

    return 0;
}
