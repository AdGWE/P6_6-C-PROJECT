#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

//Add in header files 
#include "Open_file.h"
#include "Declaration.h"
#include "Save_file.h"
#include "Backup_file.h"
#include "insert.h"

//Variables
FILE* file_pointer;
int main() {
	//Define variables
	#define MAX_LEN 255
	bool file_opened = false, file_saved = false, file_closed = false;
	char user_input[MAX_LEN + 1];

	//Print plagarism declaration when starting program
	Print_Declaration();
	while (!file_closed){
		//Get user input
		printf("P6_6:");
		fgets(user_input, sizeof(user_input), stdin);
		user_input[strcspn(user_input, "\n")] = '\0';
		if (!file_opened){
			if (strcmp(user_input,"OPEN") == 0) {
				file_pointer = Open_File("P6_6-CMS.txt","r+");
				if (file_pointer != NULL) {
					file_opened = true;
				}
			} 
			else {
				printf("File has not been opened yet!\n");
			}
		}
		else {
			if (strcmp(user_input, "SHOW ALL") == 0) {
				show_all(file_pointer);
			}
			else if (strcmp(user_input, "SHOW SUMMARY") == 0) {
				// INSERT YOUR CODE HERE
			}
			else if (strcmp(user_input, "INSERT") == 0) {
				Insert_Record(file_pointer);
			}
			else if (strcmp(user_input, "QUERY") == 0) {
				Query_Record(file_pointer);
			}
			else if (strcmp(user_input, "UPDATE") == 0) {
				// INSERT YOUR CODE HERE
			}
			else if (strcmp(user_input, "DELETE") == 0) {
				// INSERT YOUR CODE HERE
			}
			else if (strcmp(user_input, "SAVE") == 0) {
				//Call save file function
				Save_File(file_pointer);
				file_saved = true;
			}
			else if (strcmp(user_input, "BACKUP") == 0) {
				Backup_File(file_pointer);
			}
			else if (strcmp(user_input, "CLOSE") == 0) {
				//Check if user has saved file yet before closing
				if (file_saved == false) {
					printf("File has not been saved, are you sure you want to exit? Type YES to exit\n");
					fgets(user_input, sizeof(user_input), stdin);
					//If user enters YES, close file without saving
					if (strcmp(user_input, "YES\n") == 0) {
						printf("P6_6 Application Closing");
						return 0;
					}
				}
				else {
					//Close program if file has been saved 
					return 0;
				}
			}
			else {
				//No command found
				printf("Invalid Command!\n");
			}
		}
	}
	return 0;
}
