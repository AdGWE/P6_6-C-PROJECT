#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
// For string functions
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

// Helper function to remove leading and trailing spaces
static void trim(char* s) {
    // Does nothing for NULL pointer
    if (!s) {
        return;
    }
    char* start = s;
    while (*start && isspace((unsigned char)*start)) {
        start++;
    }
    // Return empty string if all spaces
    if (*start == '\0') { 
        s[0] = '\0'; return; 
    }
    char* end = start + strlen(start) - 1;
    while (end > start && isspace((unsigned char)*end)) {
        end--;
    }
    *(end + 1) = '\0';
    if (start != s) {
        memmove(s, start, strlen(start) + 1);
    }
}

// Function to check if ID is valid (7 digits)
static int valid_id(const char* s) {
    if (strlen(s) != 7) {
        return 0;
    }
    for (int i = 0; i < 7; i++) {
        if (!isdigit(s[i])) {
            return 0;
        }
    }      
    return 1;
}

// Buffer flow protection to read line safely from input
static int read_line(char* buf, size_t size) {
    if (!fgets(buf, (int)size, stdin)) {
        return 0;
    }
    buf[strcspn(buf, "\n")] = '\0';
    return 1;
}

// Validation for Mark value and convert string to float
static int parse_mark(const char* s, float* m) {
    char* end;
    float v = strtof(s, &end);
    while (*end && isspace((unsigned char)*end)) {
        end++;
    }
    if (*end != '\0') {
        return 0;
    }
    if (v < 0 || v > 100) {
        return 0;
    }
    *m = v;
    return 1;
}

// ---- MAIN UPDATE FUNCTION ----
int update_record(FILE* fp) {

    if (fp == NULL) {
        printf("Error: File not opened.\n");
        return;
    }

    char id[50];
    char line[LINE_MAX];
    long pos_before_record = -1;
    int found = 0;

    printf("\n--- UPDATE Student Record ---\n");
    printf("Enter Student ID to update: ");
    read_line(id, sizeof(id));
    trim(id);

    if (!valid_id(id)) {
        printf("Invalid ID. Must be exactly 7 digits.\n");
        // Failed
        return 1;
    }

    // Move to beginning and locate the record
    rewind(fp);

    // Skip header lines (same behaviour as show_all)
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, "ID") && strstr(line, "Name") && strstr(line, "Program"))
            break;
    }

    // Search record
    while (1) {
        long pos = ftell(fp);

        if (!fgets(line, sizeof(line), fp)) break;
        if (strlen(line) < 7) continue;

        char rec_id[20];
        sscanf(line, "%7s", rec_id);

        if (strcmp(rec_id, id) == 0) {
            found = 1;
            pos_before_record = pos;
            break;
        }
    }

    if (!found) {
        printf("Record with ID %s not found.\n", id);
        rewind(fp);
        return 1;
    }

    // Extract existing fields
    char old_name[200], old_prog[200], * ptr;
    float old_mark;

    strcpy(old_name, "");
    strcpy(old_prog, "");

    ptr = line + 7;
    while (*ptr == ' ') ptr++;

    char copy[LINE_MAX];
    strcpy(copy, ptr);

    char* last_space = strrchr(copy, ' ');
    old_mark = atof(last_space + 1);
    *last_space = '\0';

    char* prog_start = NULL;
    for (int i = (int)strlen(copy) - 1; i > 0; i--) {
        if (copy[i] == ' ' && copy[i - 1] == ' ') {
            prog_start = &copy[i];
            while (*prog_start == ' ') prog_start++;
            copy[i] = '\0';
            break;
        }
    }

    if (prog_start == NULL) {
        printf("Corrupted record format.\n");
        rewind(fp);
        return 1;
    }

    strcpy(old_name, copy);
    strcpy(old_prog, prog_start);

    // --- Display old data ---
    printf("\nCurrent Record:\n");
    printf("ID       : %s\n", id);
    printf("Name     : %s\n", old_name);
    printf("Programme: %s\n", old_prog);
    printf("Mark     : %.2f\n", old_mark);

    // --- Prompt new information ---
    char new_name[200], new_prog[200], mark_str[50];
    float new_mark;

    printf("\nEnter NEW Name (leave blank to keep existing): ");
    read_line(new_name, sizeof(new_name));
    trim(new_name);
    if (strlen(new_name) == 0) strcpy(new_name, old_name);

    printf("Enter NEW Programme (leave blank to keep existing): ");
    read_line(new_prog, sizeof(new_prog));
    trim(new_prog);
    if (strlen(new_prog) == 0) strcpy(new_prog, old_prog);

    printf("Enter NEW Mark (0-100, leave blank to keep existing): ");
    read_line(mark_str, sizeof(mark_str));
    trim(mark_str);
    if (strlen(mark_str) == 0) new_mark = old_mark;
    else if (!parse_mark(mark_str, &new_mark)) {
        printf("Invalid mark.\n");
        rewind(fp);
        return 1;
    }

    // ---- WRITE UPDATED RECORD BACK TO FILE ----
    fseek(fp, pos_before_record, SEEK_SET);
    fprintf(fp, "%s  %s  %s  %.2f\n", id, new_name, new_prog, new_mark);

    fflush(fp);
    rewind(fp);

    printf("\n=== Record Successfully Updated ===\n");
    printf("ID       : %s\n", id);
    printf("Name     : %s\n", new_name);
    printf("Programme: %s\n", new_prog);
    printf("Mark     : %.2f\n", new_mark);
    printf("===================================\n\n");

    return 0;
}
