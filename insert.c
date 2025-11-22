#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include "insert.h"

#define LINE_MAX       512
#define ID_LEN         7
#define MAX_NAME_LEN   100
#define MAX_PROG_LEN   100
#define MAX_INPUT      256

// ******************* HELPER FUNCTIONS **************************

/**
BUFFER OVERFLOW PROTECTION AND VALIDATION
 */
static bool read_line(char *buf, size_t size) {
    if (buf == NULL || size == 0) {
        return false;
    }

    if (fgets(buf, (int)size, stdin) == NULL) {
        // Check if EOF or actual error
        if (feof(stdin)) {
            clearerr(stdin);  // Clear EOF flag
        }
        return false;
    }

    // Remove newline character
    size_t len = strcspn(buf, "\n");
    if (buf[len] == '\n') {
        buf[len] = '\0';
    } else {
        // Input was too long and flush the rest of the line
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {
            // Discard excess characters
        }
        printf("Warning: Input truncated to %zu characters.\n", size - 1);
    }

    return true;
}

/**
 * Function to Validate Student ID: HAS TO BE EXACTLY 7 CHARACTERS
 */
static bool valid_id(const char *s) {
    if (s == NULL) {
        return false;
    }

    size_t len = strlen(s);
    
    // Check that user ID is exactly 7 digits.
    if (len != ID_LEN) {
        return false;
    }

    // Check that every charcater is a digit.
    for (size_t i = 0; i < len; i++) {
        if (!isdigit((unsigned char)s[i])) {
            return false;
        }
    }

    return true;
}

/**
 * Validates student name:
 * - Cannot be empty or only whitespace
 * - Must contain at least one alphabetic character
 * - Can contain letters, spaces, hyphens, apostrophes
 * - No numbers or special characters (except - and ')
 */
static bool valid_name(const char *s) {
    if (s == NULL || s[0] == '\0') {
        return false;
    }

    bool has_letter = false;
    bool all_whitespace = true;

    for (size_t i = 0; s[i] != '\0'; i++) {
        unsigned char c = (unsigned char)s[i];
        
        if (isalpha(c)) {
            has_letter = true;
            all_whitespace = false;
        } else if (isspace(c)) {
            // Spaces are allowed
            continue;
        } else if (c == '-' || c == '\'' || c == '.') {
            // Hyphens, apostrophes, and periods are allowed in names
            all_whitespace = false;
        } else if (isdigit(c)) {
            // Numbers in names are generally not allowed
            return false;
        } else {
            // Other special characters not allowed
            return false;
        }
    }

    // Name must have at least one letter and not be all whitespace
    return has_letter && !all_whitespace;
}

/**
 * Validates programme name:
 * - Cannot be empty or only whitespace
 * - Must contain at least one alphabetic character
 * - Can contain letters, spaces, hyphens, ampersands
 */
static bool valid_programme(const char *s) {
    if (s == NULL || s[0] == '\0') {
        return false;
    }

    bool has_letter = false;
    bool all_whitespace = true;

    for (size_t i = 0; s[i] != '\0'; i++) {
        unsigned char c = (unsigned char)s[i];
        
        if (isalpha(c)) {
            has_letter = true;
            all_whitespace = false;
        } else if (isspace(c)) {
            // Spaces are allowed
            continue;
        } else if (c == '-' || c == '&' || c == '/' || c == '(' || c == ')') {
            // Common characters in programme names
            all_whitespace = false;
        } else {
            // Disallow other special characters
            return false;
        }
    }

    return has_letter && !all_whitespace;
}

/*
    Function that handles and trims all cases of whitespace from leading to trailing.
*/
static void trim_whitespace(char *s) {
    if (s == NULL) {
        return;
    }

    // Trim leading whitespace
    char *start = s;
    while (*start && isspace((unsigned char)*start)) {
        start++;
    }

    // If string is all whitespace
    if (*start == '\0') {
        s[0] = '\0';
        return;
    }

    // Trim trailing whitespace
    char *end = start + strlen(start) - 1;
    while (end > start && isspace((unsigned char)*end)) {
        end--;
    }
    *(end + 1) = '\0';

    // Move trimmed string to beginning if needed
    if (start != s) {
        memmove(s, start, strlen(start) + 1);
    }
}

/**
 * Checks if a student ID already exists in the file
 * Returns: true if duplicate found, false otherwise
 * Handles: corrupted files, empty files, missing headers
 */
static bool check_duplicate_id(FILE *fp, const char *id) {
    if (fp == NULL || id == NULL) {
        return false;
    }

    char line[LINE_MAX];
    char existing_id[ID_LEN + 2];  // Extra space for safety
    bool header_found = false;
    long original_pos = ftell(fp);  // Save current position

    // Move to beginning of file
    if (fseek(fp, 0, SEEK_SET) != 0) {
        // If seek fails, restore position and return false (assume no duplicate)
        fseek(fp, original_pos, SEEK_SET);
        return false;
    }

    // Find the header line that contains "ID"
    while (fgets(line, sizeof(line), fp) != NULL) {
        // Check if this line contains the header
        if (strstr(line, "ID") != NULL && strstr(line, "Name") != NULL) {
            header_found = true;
            break;
        }
    }

    // If no header found, file might be corrupted or empty
    if (!header_found) {
        fseek(fp, original_pos, SEEK_SET);
        return false;  // Assume no duplicate in corrupted file
    }

    // Now scan all data records
    while (fgets(line, sizeof(line), fp) != NULL) {
        // Skip empty lines
        if (line[0] == '\n' || line[0] == '\r' || line[0] == '\0') {
            continue;
        }

        // Skip lines that are all whitespace
        bool all_space = true;
        for (size_t i = 0; line[i] != '\0' && line[i] != '\n'; i++) {
            if (!isspace((unsigned char)line[i])) {
                all_space = false;
                break;
            }
        }
        if (all_space) {
            continue;
        }

        // Try to extract the ID (first field, exactly 7 characters)
        int items = sscanf(line, "%7s", existing_id);
        if (items == 1) {
            // Ensure we only compare exactly 7 characters
            existing_id[ID_LEN] = '\0';
            
            // Verify it's actually a valid ID format
            if (valid_id(existing_id) && strcmp(existing_id, id) == 0) {
                // Duplicate found!
                fseek(fp, original_pos, SEEK_SET);  // Restore position
                return true;
            }
        }
    }

    // Restore file position
    fseek(fp, original_pos, SEEK_SET);
    return false;
}

/**
 * Validates mark input
 */
static bool parse_mark(const char *str, float *result) {
    if (str == NULL || result == NULL || str[0] == '\0') {
        return false;
    }

    // Check for obviously invalid input
    if (strcmp(str, "inf") == 0 || strcmp(str, "-inf") == 0 || 
        strcmp(str, "nan") == 0 || strcmp(str, "NaN") == 0) {
        return false;
    }

    char *endptr;
    errno = 0;  // Reset errno before conversion
    
    float value = strtof(str, &endptr);

    // Check for conversion errors
    if (errno == ERANGE) {
        // Overflow or underflow occurred
        return false;
    }

    // Skip trailing whitespace
    while (*endptr && isspace((unsigned char)*endptr)) {
        endptr++;
    }

    // If there are any non-whitespace characters left, input was invalid
    if (*endptr != '\0') {
        return false;
    }

    // Check for valid range
    if (value < 0.0f || value > 100.0f) {
        return false;
    }

    // Check for special floating point values
    if (value != value) {  // NaN check
        return false;
    }

    *result = value;
    return true;
}

// ==================== MAIN INSERT FUNCTION ====================

int Insert_Record(FILE *fp) {
    // Validate file pointer
    if (fp == NULL) {
        printf("Error: File not opened. Cannot insert record.\n");
        return 1;
    }

    // Check if file is writable
    long test_pos = ftell(fp);
    if (test_pos < 0) {
        printf("Error: File position error. Cannot insert record.\n");
        return 1;
    }

    // Declare variables
    char id[MAX_INPUT];
    char name[MAX_INPUT];
    char programme[MAX_INPUT];
    char mark_str[MAX_INPUT];
    float mark;

    int attempts = 0;
    const int MAX_ATTEMPTS = 5;  // Give user limited attempts and prevent infinite loop.

    // ====================1: Get and Validate Student ID ====================
    printf("\n--- Insert New Student Record ---\n");
    
    while (attempts < MAX_ATTEMPTS) {
        printf("Enter Student ID (7 digits, or 'cancel' to abort): ");
        
        if (!read_line(id, sizeof(id))) {
            printf("Error: Failed to read input.\n");
            attempts++;
            continue;
        }

        // Allow user to cancel
        if (_stricmp(id, "cancel") == 0 || _stricmp(id, "exit") == 0) {
            printf("Insert operation cancelled.\n");
            return 1;
        }

        // Trim any whitespace
        trim_whitespace(id);

        // Validate ID format
        if (!valid_id(id)) {
            printf("Invalid ID. Must be exactly 7 digits (0-9 only).\n");
            attempts++;
            continue;
        }

        // Check for duplicate
        if (check_duplicate_id(fp, id)) {
            printf("Error: Student ID %s already exists in the database.\n", id);
            printf("Please enter a different ID.\n");
            attempts++;
            continue;
        }

        // Valid and unique ID
        break;
    }

    if (attempts >= MAX_ATTEMPTS) {
        printf("Too many invalid attempts. Insert operation cancelled.\n");
        return 1;
    }

    // ====================2: Get and Validate Name ====================
    attempts = 0;
    while (attempts < MAX_ATTEMPTS) {
        printf("Enter Student Name (or 'cancel' to abort): ");
        
        if (!read_line(name, sizeof(name))) {
            printf("Error: Failed to read input.\n");
            attempts++;
            continue;
        }

        // Allow user to cancel
        if (_stricmp(name, "cancel") == 0 || _stricmp(name, "exit") == 0) {
            printf("Insert operation cancelled.\n");
            return 1;
        }

        // Trim whitespace
        trim_whitespace(name);

        // Validate name
        if (!valid_name(name)) {
            printf("Invalid name. Name must contain letters and cannot be empty.\n");
            printf("Only letters, spaces, hyphens (-), apostrophes ('), and periods (.) are allowed.\n");
            attempts++;
            continue;
        }

        // Check length
        if (strlen(name) > MAX_NAME_LEN) {
            printf("Error: Name too long. Maximum %d characters.\n", MAX_NAME_LEN);
            attempts++;
            continue;
        }

        // Valid name
        break;
    }

    if (attempts >= MAX_ATTEMPTS) {
        printf("Too many invalid attempts. Insert operation cancelled.\n");
        return 1;
    }

    // ====================3: Get and Validate Programme ====================
    attempts = 0;
    while (attempts < MAX_ATTEMPTS) {
        printf("Enter Programme (or 'cancel' to abort): ");
        
        if (!read_line(programme, sizeof(programme))) {
            printf("Error: Failed to read input.\n");
            attempts++;
            continue;
        }

        // Allow user to cancel
        if (_stricmp(programme, "cancel") == 0 || _stricmp(programme, "exit") == 0) {
            printf("Insert operation cancelled.\n");
            return 1;
        }

        // Trim whitespace
        trim_whitespace(programme);

        // Validate programme
        if (!valid_programme(programme)) {
            printf("Invalid programme name. Must contain letters and cannot be empty.\n");
            attempts++;
            continue;
        }

        // Check length
        if (strlen(programme) > MAX_PROG_LEN) {
            printf("Error: Programme name too long. Maximum %d characters.\n", MAX_PROG_LEN);
            attempts++;
            continue;
        }

        // Valid programme
        break;
    }

    if (attempts >= MAX_ATTEMPTS) {
        printf("Too many invalid attempts. Insert operation cancelled.\n");
        return 1;
    }

    // ====================4: Get and Validate Mark ====================
    attempts = 0;
    while (attempts < MAX_ATTEMPTS) {
        printf("Enter Mark (0-100, or 'cancel' to abort): ");
        
        if (!read_line(mark_str, sizeof(mark_str))) {
            printf("Error: Failed to read input.\n");
            attempts++;
            continue;
        }

        // Allow user to cancel
        if (_stricmp(mark_str, "cancel") == 0 || _stricmp(mark_str, "exit") == 0) {
            printf("Insert operation cancelled.\n");
            return 1;
        }

        // Trim whitespace
        trim_whitespace(mark_str);

        // Validate and parse mark
        if (!parse_mark(mark_str, &mark)) {
            printf("Invalid mark. Must be a number between 0 and 100.\n");
            attempts++;
            continue;
        }

        // Valid mark
        break;
    }

    if (attempts >= MAX_ATTEMPTS) {
        printf("Too many invalid attempts. Insert operation cancelled.\n");
        return 1;
    }

    // ====================5: Write Record to File ====================
    
    // Move to end of file
    if (fseek(fp, 0, SEEK_END) != 0) {
        printf("Error: Failed to position file pointer.\n");
        return 1;
    }

    // Check if we need to add a newline before our record
    long file_size = ftell(fp);
    if (file_size > 0) {
        // Go back one character to check last char
        if (fseek(fp, -1, SEEK_END) == 0) {
            int last_char = fgetc(fp);
            // If last character wasn't a newline, add one
            if (last_char != '\n' && last_char != EOF) {
                fseek(fp, 0, SEEK_END);
                fputc('\n', fp);
            } else {
                fseek(fp, 0, SEEK_END);
            }
        }
    }

    // Write the record with two spaces between fields (matching file format)
    int write_result = fprintf(fp, "%s  %s  %s  %.2f\n", id, name, programme, mark);
    
    if (write_result < 0) {
        printf("Error: Failed to write record to file.\n");
        return 1;
    }

    // Force write to disk
    if (fflush(fp) != 0) {
        printf("Warning: Failed to flush file buffer. Changes may not be saved.\n");
    }

    // Reset file pointer to beginning for subsequent operations
    rewind(fp);

    // Success message
    printf("\n=== Record Inserted Successfully ===\n");
    printf("Student ID: %s\n", id);
    printf("Name:       %s\n", name);
    printf("Programme:  %s\n", programme);
    printf("Mark:       %.2f\n", mark);
    printf("====================================\n\n");
    return 0;
}