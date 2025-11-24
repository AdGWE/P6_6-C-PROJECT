#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

//Add in header files 
#include "Declaration.h"
#include "File_Handling.h"
#include "Backup_file.h"
#include "insert.h"
#include "show_all.h"
#include "query.h"
#include "Update.h"
#include "Delete.h"

#define MAX_LEN 255
#define FILE_NAME "P6_6-CMS.txt"
//Variables
FILE* file_pointer;
int main() {
	//Define variables
	bool file_opened = false, file_saved = true, file_closed = false;
	char user_input[MAX_LEN + 1];
	char *P6_6_filename = FILE_NAME;
	
	//Print plagarism declaration when starting program
	Print_Declaration();
	while (!file_closed){
		//Get user input
		printf("P6_6:");
		fgets(user_input, sizeof(user_input), stdin);
		user_input[strcspn(user_input, "\n")] = '\0';

		//Full caps user input
		for (int i = 0; user_input[i] != '\0'; i++) {
			user_input[i] = toupper(user_input[i]);
		}

		if (!file_opened){
			//If user choses OPEN 
			if (strcmp(user_input,"OPEN") == 0) {
				file_pointer = Open_File(P6_6_filename,"r+");
				if (file_pointer != NULL) {
					file_opened = true;
					//Copy file generates a temporary file with the contents of the CMS
					Create_Temporary_File(file_pointer);
				}
			} 
			//If user closes the programme immediately after opening
			else if(strcmp(user_input, "CLOSE") == 0)
			{
				printf("P6_6 Application Closing");
				return 0;
			}
			//Warn user that file has not been opened yet
			else {
				printf("File has not been opened yet!\n");
			}
		}
		else {
			//If user choses SHOW ALL 
			if (strcmp(user_input, "SHOW ALL") == 0) {
				show_all(file_pointer);
			}
			//If user choses SHOW SUMMARY 
			else if (strcmp(user_input, "SHOW SUMMARY") == 0) {
				show_summary(file_pointer);
			}
			//If user choses INSERT
			else if (strcmp(user_input, "INSERT") == 0) {
				if (!Insert_Record(file_pointer)) {
					file_saved = false;
				}
			}
			//If user choses QUERY 
			else if (strcmp(user_input, "QUERY") == 0) {
				Query_Record(file_pointer);
			}
			//If user choses UPDATE
			else if (strcmp(user_input, "UPDATE") == 0) {
				if (!update_record(file_pointer)) {
					file_saved = false;
				}
			}
			//If user choses DELETE 
			else if (strcmp(user_input, "DELETE") == 0) {
				if (!delete_record(file_pointer)) {
					file_saved = false;
				}
			}
			//If user choses SAVE
			else if (strcmp(user_input, "SAVE") == 0) {
				//Call save file function
				Save_File(file_pointer);
				file_saved = true;
			}
			//If user choses BACKUP
			else if (strcmp(user_input, "BACKUP") == 0) {
				Backup_File(file_pointer);
			}
			//If user choses CLOSE
			else if (strcmp(user_input, "CLOSE") == 0) {
				//Check if user has saved file yet before closing
				if (file_saved == false) {
					printf("File has not been saved, are you sure you want to exit? Type YES to exit | Type anything else to cancel\n");
					fgets(user_input, sizeof(user_input), stdin);
					user_input[strcspn(user_input, "\n")] = '\0';
					//If user enters YES, close file without saving
					if (strcmp(user_input, "YES") == 0) {
						Revert_Changes(file_pointer,P6_6_filename);
						printf("P6_6 Application Closing");
						Close_File(file_pointer);
						goto end_programme;
					}
				}
				else {
					//Close programme if file has been saved 
					Close_File(file_pointer);
					goto end_programme;
				}
			}
			else {
				//No command found
				printf("Invalid Command!\n");
			}
		}
	}
end_programme:
	return 0;
}
