#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "query.h"

#define MAX_LINE 512

// Helper function to convert string to lowercase
void to_lower(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

// Helper function to check if string contains substring (case insensitive)
int contains(char *text, char *search) {
    char text_lower[MAX_LINE];
    char search_lower[MAX_LINE];
    
    strcpy(text_lower, text);
    strcpy(search_lower, search);
    
    to_lower(text_lower);
    to_lower(search_lower);
    
    return strstr(text_lower, search_lower) != NULL;
}

void Query_Record(FILE *fp) {
    if (fp == NULL) {
        printf("Error: File not opened.\n");
        return;
    }
    
    char line[MAX_LINE];
    char choice[10];
    char search_term[100];
    int found = 0;
    int search_type;
    
    // Ask user what to search by
    printf("\nSearch by:\n");
    printf("1. ID\n");
    printf("2. Name\n");
    printf("3. Programme\n");
    printf("Enter choice: ");
    fgets(choice, sizeof(choice), stdin);
    search_type = atoi(choice);
    
    if (search_type < 1 || search_type > 3) {
        printf("Invalid choice.\n");
        return;
    }
    
    // Get search term
    printf("Enter search term: ");
    fgets(search_term, sizeof(search_term), stdin);
    search_term[strcspn(search_term, "\n")] = '\0';  // Remove newline
    
    if (strlen(search_term) == 0) {
        printf("Search term cannot be empty.\n");
        return;
    }
    
    // Go to start of file
    rewind(fp);
    
    // Skip to header line
    while (fgets(line, sizeof(line), fp) != NULL) {
        if (strstr(line, "ID") && strstr(line, "Name") && strstr(line, "Programme")) {
            break;
        }
    }
    
    // Print header for results
    printf("\nSearch Results:\n");
    printf("%-10s %-25s %-25s %-10s\n", "ID", "Name", "Programme", "Mark");
    printf("------------------------------------------------------------------------\n");
    
    // Search through records
    while (fgets(line, sizeof(line), fp) != NULL) {
        // Skip empty lines
        if (line[0] == '\n' || line[0] == '\r') {
            continue;
        }
        
        char id[20];
        char name[100];
        char programme[100];
        float mark;
        
        // Parse the line - ID is first 7 chars, then we need to parse the rest
        if (sscanf(line, "%7s", id) != 1) {
            continue;
        }
        
        // Check if ID is all digits (valid record)
        int valid = 1;
        for (int i = 0; i < 7; i++) {
            if (!isdigit(id[i])) {
                valid = 0;
                break;
            }
        }
        if (!valid) continue;
        
        // Parse the rest of the line
        // Format: ID  Name  Programme  Mark
        char *ptr = line + 7;  // Skip ID
        
        // Skip spaces after ID
        while (*ptr == ' ') ptr++;
        
        // Get name (everything until we find two spaces followed by text that looks like a programme)
        char temp[MAX_LINE];
        strcpy(temp, ptr);
        
        // Find the mark at the end
        char *last_space = strrchr(temp, ' ');
        if (last_space == NULL) continue;
        
        // Get mark
        mark = atof(last_space + 1);
        *last_space = '\0';  // Remove mark from temp
        
        // Now find programme (look for last occurrence of double space)
        char *prog_start = NULL;
        for (int i = strlen(temp) - 1; i > 0; i--) {
            if (temp[i] == ' ' && temp[i-1] == ' ') {
                prog_start = &temp[i];
                while (*prog_start == ' ') prog_start++;
                temp[i] = '\0';  // Split name and programme
                break;
            }
        }
        
        if (prog_start == NULL) continue;
        
        strcpy(name, temp);
        strcpy(programme, prog_start);
        
        // Remove trailing spaces from name
        int len = strlen(name);
        while (len > 0 && name[len-1] == ' ') {
            name[len-1] = '\0';
            len--;
        }
        
        // Check if this record matches search criteria
        int match = 0;
        
        if (search_type == 1) {
            // Search by ID - exact match
            if (strcmp(id, search_term) == 0) {
                match = 1;
            }
        } else if (search_type == 2) {
            // Search by Name - partial match, case insensitive
            if (contains(name, search_term)) {
                match = 1;
            }
        } else if (search_type == 3) {
            // Search by Programme - partial match, case insensitive
            if (contains(programme, search_term)) {
                match = 1;
            }
        }
        
        // If match found, print the record
        if (match) {
            printf("%-10s %-25s %-25s %-10.2f\n", id, name, programme, mark);
            found++;
        }
    }
    
    printf("------------------------------------------------------------------------\n");
    printf("Total records found: %d\n\n", found);
    
    // Reset file pointer
    rewind(fp);
}