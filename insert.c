#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "insert.h"

#define LINE_MAX 512

void Insert_Record(FILE *fp) {
    char id[32];
    char name[128];
    char programme[128];
    char mark_str[32];
    float mark;

    char line[LINE_MAX];
    char existing_id[32];
    bool duplicate = false;

    if (fp == NULL) {
        printf("File not opened.\n");
        return;
    }

    /* ===== 1. Get ID ===== */
    printf("Enter Student ID: ");
    if (fgets(id, sizeof(id), stdin) == NULL) {
        printf("Input error.\n");
        return;
    }
    id[strcspn(id, "\n")] = '\0';

    if (strlen(id) == 0) {
        printf("Student ID cannot be empty.\n");
        return;
    }

    /* ===== 2. Check for duplicate ID ===== */
    rewind(fp);   // go to start

    // Find header line starting with "ID"
    while (fgets(line, sizeof(line), fp) != NULL) {
        char first[32];
        if (sscanf(line, "%31s", first) == 1 && strcmp(first, "ID") == 0) {
            break;
        }
    }

    // Scan data lines
    while (fgets(line, sizeof(line), fp) != NULL) {
        if (line[0] == '\n' || line[0] == '\r')
            continue;

        if (sscanf(line, "%31s", existing_id) == 1) {
            if (strcmp(existing_id, id) == 0) {
                duplicate = true;
                break;
            }
        }
    }

    if (duplicate) {
        printf("Error: A record with Student ID %s already exists. Insertion cancelled.\n", id);
        rewind(fp);
        return;
    }

    /* ===== 3. Get remaining fields ===== */
    printf("Enter Name: ");
    if (fgets(name, sizeof(name), stdin) == NULL) {
        printf("Input error.\n");
        return;
    }
    name[strcspn(name, "\n")] = '\0';

    printf("Enter Programme: ");
    if (fgets(programme, sizeof(programme), stdin) == NULL) {
        printf("Input error.\n");
        return;
    }
    programme[strcspn(programme, "\n")] = '\0';

    printf("Enter Mark: ");
    if (fgets(mark_str, sizeof(mark_str), stdin) == NULL) {
        printf("Input error.\n");
        return;
    }
    if (sscanf(mark_str, "%f", &mark) != 1) {
        printf("Invalid mark entered. Insertion cancelled.\n");
        return;
    }

    /* ===== 4. Append record ===== */
    fseek(fp, 0, SEEK_END);

    long pos = ftell(fp);
    if (pos > 0) {
        // Look at last char
        fseek(fp, -1, SEEK_END);
        int ch = fgetc(fp);   // READ

        // If last char isn't newline, add one
        if (ch != '\n') {
            fseek(fp, 0, SEEK_END); // reposition AFTER the read before writing
            fputc('\n', fp);        // WRITE
        } else {
            fseek(fp, 0, SEEK_END); // still reposition so we're safe
        }
    }

    // Two spaces between columns, matching your file
    fprintf(fp, "%s  %s  %s  %.1f\n", id, name, programme, mark);

    fflush(fp);
    rewind(fp);

    printf("Record inserted successfully.\n");
}
