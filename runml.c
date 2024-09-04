//  CITS2002 Project 1 2024
//  Student1:   STUDENT-24000581   NAME- Mohammad Ashraf Qureshi
//  Student2:   STUDENT-23895849   NAME-Baasil Sidiqui
//  Platform:   Linux

//Importing all the the libraries we need 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//maximum Identifiers a the program will have is 50 
#define MAX_ID 50

//defines the stucture of fn which hold the name and the number of aruments a function has will use these to process funvctions
struct fn {
    char* name;
    int args;
};
//fn_list is a list of the stuctures for function so we can track them, num_fns is used to track the number of function we have in the program 
struct fn fn_list[MAX_ID];
int num_fns = 0;
// var is a list a list of unique identifiers which are variables in the program
char* vars[MAX_ID]; 
int num_vars = 0;
// num_vars keeps a count of the number of variables

// strip function removes unnessisary spaces from the code 
void strip(char* line)
{   // first loop removes the leading empty space  
    for (int i = 0; i < strlen(line); i++) {
        if (line[i] != ' ') {
            line += i;
            break;
        }
    }
 // removes all the trailing white spaces
    for (int i = strlen(line) - 1; i > 0; i--) {
        if (line[i] == ' ') {
            line[i] = '\0';
            continue;
        }
        break;
    }
}
// preprocessor function ends the string at # as the rest of the line is not needed for the compiler
void preprocess(char* line)
{
    for (int i = 0; i < strlen(line); i++) {
        if (line[i] == '#') {
            line[i] = 0;
            break;
        }
    }

    strip(line);
}

int isValidId(name){
    for (int i = 0; i < strlen(name); i++){
        if (!islower(name[i])){
            printf("Identifier name %s is invalid" , name);
            exit(EXIT_FAILURE);

        }
        else if (i == 12)
        {
            printf("Identifier name %s is invalid" , name);
            exit(EXIT_FAILURE);
        }
        
    }
    return 0
}

int handle_assignment(char* line, char *var_arr[], int size,
                    char *var_name, char *var_val)
{
    const char* delim = "<-";

    strcpy(var_name, strtok(line, delim));
    strcpy(var_val, strtok(NULL, delim));

    // invalid syntax if we have multiple arrows or if we have no arrows
    if (strtok(NULL, delim) != NULL || var_val == NULL) {
        fprintf(stderr, "invalid syntax");
        exit(EXIT_FAILURE);
    }

    isValidId(var_name);

    strip(var_name);
    strip(var_val);

    var_arr[size++] = var_name;
    return 0;
}

// 
void procfile(char *filename)
{
    FILE* infd = fopen(filename, "r");
    char line[1000];

    while (fgets(line, 1000, infd) != NULL) {
        preprocess(line);
        char var_name[1000];
        char var_val[1000];
        handle_assignment(line, vars, num_vars, var_name, var_val);
    }
}
//main function takes 1 anrgument which is file name
int main(int argc, char* argv[])
{
    if (argc != 2) {
        printf("Usage:runml [input-file]");
        exit(EXIT_FAILURE);
    }

    procfile(argv[1]);
}

