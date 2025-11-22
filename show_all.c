#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "show_all.h"


#define TABLE_DESCRIPTION 4
#define MAX 100
//Data of each user
int id[MAX];
char name[MAX][50];
char program[MAX][50];
float mark[MAX];


FILE* fp;
int sortBy;
int order;


void fillArray(int row, int col, char **arr, FILE *fp){
    char buffer[256];
    int i = 0;
    while (i < row && fgets(buffer, sizeof(buffer), fp)) {
        size_t len = strlen(buffer);

        if (len > 0 && buffer[len - 1] == '\n') 
            buffer[len - 1] = '\0';

        //To replace double space to \t
        char temp[256];
        int src = 0, dst =0;

        while(buffer[src] != '\0' && dst < 255){
            if(buffer[src] == ' ' && buffer[src + 1] == ' '){
                temp[dst++] = '\t';
                src +=2; // to skip two spaces
            }
            else
                temp[dst++] = buffer[src++];            
        }
        temp[dst] = '\0'; // adding termination at the end.


        // copy buffer into array row safely
        snprintf(arr[i], col, "%s", temp);         
        i++;
    }

}
int count_lines(FILE *fp){
    int rows = 0;
    char buffer[256];
    while(fgets(buffer,sizeof(buffer),fp)){
        rows++;
    }
    return rows;
}
int sort(const void *first_index, const void *second_index){    
    int f = *(int *)first_index;
    int s = *(int *)second_index;
    int result = 0;

    //Ascending Order = 1
    //1-ID 2-Name 3-Program 4-Mark
    switch(sortBy){
        case 1:
            result = id[f] - id[s];
            break;
        case 2:
            result = strcmp(name[f],name[s]);
            break;
        case 3:
            result = strcmp(program[f],program[s]);
            break;
        case 4:
            if(mark[f] < mark[s])
                result = -1;
            else if(mark[f] > mark[s])
                result = 1;
            else
                result = 0;
    }

    //Descending Order = 2
    if(order == 2)
        result = -result;

    return result;
}
int validateInput(const char *msg, int min, int max){
    char buff[256];
    int value;
    char *endptr;
    
    while(1){
        printf("%s",msg);

        //For EOF or READ ERROR
        if(!fgets(buff,sizeof(buff),stdin))
            continue;

        //Remove the newline
        buff[strcspn(buff,"\n")] = 0;

        value = strtol(buff,&endptr,10);

        if(endptr != buff && *endptr == '\0' && value >= min && value <= max){            
            return (int)value;
        }

        printf("Invalid Input\n");
    }
}
void displayData(int row, int order_index[]){    
    printf("+---------+----------------------+----------------------+-------+\n");
    printf("| ID      | Name                 | Program               | Mark |\n");
    printf("+---------+----------------------+----------------------+-------+\n");

    for(int x = 0; x < row; x++) {
        int idx = order_index[x];
        printf("| %-6d | %-20s | %-20s | %5.1f |\n",
               id[idx], name[idx], program[idx], mark[idx]);
    }

    printf("+---------+----------------------+----------------------+-------+\n");
}

int init_table(FILE *fp, int* row, char ***data, int** order_index){    
    int col = 256;
    *row = count_lines(fp);
    
    //allocate mem
    *data = malloc((*row)*sizeof(char*));
    if(*data == NULL)
        return 0;
    for(int x=0;x< *row;x++){
        (*data)[x] = malloc(col);
        if((*data)[x] == NULL)
            return 0;
    }
    

    //reset line and fill content
    rewind(fp);
    fillArray(*row,col,*data,fp);
    

    //allocate mem for order_index
    int orderSize = *row - 1;
    *order_index = malloc(orderSize*sizeof(int));
    if(*order_index == NULL)
        return 0;

    //Initialise order_index
    for(int x=0;x<orderSize;x++)
        (*order_index)[x] = x;       

    
    //Store each student data
    for(int x=0; x< *row; x++){                
        int idx = x-TABLE_DESCRIPTION;
        char *token = strtok((*data)[x], "\t");
        if(token) 
            id[idx] = atoi(token);

        token = strtok(NULL, "\t");
        if(token)
            strcpy(name[idx],token);

        token = strtok(NULL, "\t");
        if(token)
            strcpy(program[idx],token);

        token = strtok(NULL, "\t");
        if(token)
            mark[idx] = atof(token);
        
    }
    

    return 1;
}
void free_table(char **data, int row, int *order_index){
    for(int x=0;x<row;x++)
        free(data[x]);

    free(data);
    free(order_index);
}
void show_all(FILE *fp){       
    int row;
    char **data;
    int *order_index;
    
    //Init table
    if(!init_table(fp,&row,&data,&order_index)){
        printf("Failed to retrieve data");
        return;
    }

    //Sort Data base on user input
    sortBy = validateInput("Sort by: 0-Nothing 1-ID 2-Name 3-Program 4-Mark:\n",0,4);
    if(sortBy != 0){
        order = validateInput("Order: 1-Ascending 2-Descending:\n",1,2);    
        qsort(order_index,row-TABLE_DESCRIPTION,sizeof(int),sort);
    }
    
    //Display CMS
    displayData(row-TABLE_DESCRIPTION,order_index);

    //Free malloc
    free_table(data,row,order_index);
    rewind(fp);
}  


void display_summary(int h_index, int l_index){
    printf("+---------+----------------------+----------------------+-------+\n");
    printf("|                            HIGHEST                            |\n");
    printf("+---------+----------------------+----------------------+-------+\n");
    printf("| ID      | Name                 | Program              |  Mark |\n");
    printf("+---------+----------------------+----------------------+-------+\n");
    printf("| %-6d | %-20s | %-20s | %5.1f |\n",
        id[h_index], name[h_index], program[h_index], mark[h_index]);
    printf("+---------+----------------------+----------------------+-------+\n\n\n");

    printf("+---------+----------------------+----------------------+-------+\n");
    printf("|                             LOWEST                            |\n");
    printf("+---------+----------------------+----------------------+-------+\n");
    printf("| ID      | Name                 | Program              |  Mark |\n");
    printf("+---------+----------------------+----------------------+-------+\n");
    printf("| %-6d | %-20s | %-20s | %5.1f |\n",
        id[l_index], name[l_index], program[l_index], mark[l_index]);
    printf("+---------+----------------------+----------------------+-------+\n\n");        
}
void displayTotal(int row, int *order_index){          
    int i = 0;
    int program_idx[MAX] = {-1};
    program_idx[0] = 0;    
    for(int x = 0; x < row; x++) {        
        if(strcmp(program[order_index[x]],program[order_index[i]])!=0){
            i++;            
            program_idx[i] = x;                        
        }
    }
    printf("+---------------------------+\n");
    printf("|Total Students:            |\n");
    printf("+---------------------------+\n");
    for(int x=0;x<=i;x++){
        int total = x < i?(program_idx[x+1] - program_idx[x]):(row-program_idx[x]);
        printf("|%-20s:%-6d|\n",program[order_index[program_idx[x]]],total);
    }
    printf("+---------------------------+\n");


}

void show_summary(FILE *fp){
    int row;
    char **data;
    int *order_index;    
    //Init table
    if(!init_table(fp,&row,&data,&order_index)){
        printf("Failed to retrieve data");
        return;
    }    
    ///get highest mark
    order = 2;
    sortBy = 4;    
    qsort(order_index,row-TABLE_DESCRIPTION,sizeof(int),sort);
    display_summary(order_index[0],order_index[row-(TABLE_DESCRIPTION+1)]);

    sortBy = 3;
    qsort(order_index,row-TABLE_DESCRIPTION,sizeof(int),sort);
    displayTotal(row-TABLE_DESCRIPTION,order_index);
    free_table(data,row,order_index);
    rewind(fp);


}

