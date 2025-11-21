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

//void show_all(FILE FP){}
void fillArray(int row, int col, char arr[row][col], FILE *fp){
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
        //printf("LINE[%d]:%s\n",i,arr[i]);
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

void displayData(int row, int order_index[], const char *db_name, const char *description){    
    printf("%s\n",db_name);
    printf("%s\n",description);
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
void show_all(FILE *fp){       
    size_t row = count_lines(fp); // amount of rows
    size_t col = 256; //amount of chars per row    
    char data[row][col];
    char *token;

    //Sort Sequence
    
    int order_index[row-TABLE_DESCRIPTION];
    for(int x=0;x<row-TABLE_DESCRIPTION;x++){
        order_index[x] = x;
        //printf("ORDERINDEX:%d\n",x);
    }

    rewind(fp);
    fillArray(row,col,data,fp);
    //printf("BEFORE FCLOSE");
    
        


    //Store each student data
    for(int x=0; x<row; x++){        
        //printf("data[%d]:%s\n",x-TABLE_DESCRIPTION,data[x]);
        int idx = x-TABLE_DESCRIPTION;
        token = strtok(data[x], "\t");
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
        
       // printf("ID:%d\tNAME:%s\tPROGRAM:%s\tMARK%.1f\n",id[idx],name[idx],program[idx],mark[idx]);
    }

    printf("Sort by: 0-Nothing 1-ID 2-Name 3-Program 4-Mark:\n");
    scanf("%d", &sortBy);

    printf("Order: 1-Ascending 2-Descending:\n");
    scanf("%d", &order);

    if(sortBy !=0)
        qsort(order_index,row-TABLE_DESCRIPTION,sizeof(int),sort);
    
    /*
    for(int x=0;x<row-TABLE_DESCRIPTION;x++){
        printf("orderindex:%d\tProgram:%s\n",order_index[x],program[order_index[x]]);
    }*/
    displayData(row-TABLE_DESCRIPTION,order_index,data[0],data[1]);


    
}        

