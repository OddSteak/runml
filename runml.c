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
// num_vars holds the number of variable that are in the function
int proc_assignment(char* line, char *var_name, char *var_val);

void strip(char* line, char* out);
 
int main(int argc, char* argv[])
{
    if (argc == 1) {
        printf("No files provided");
        exit(EXIT_FAILURE);
    }

    FILE* inputfile = fopen(argv[1], "r");
    char line[1000];
    while (fgets(line, 1000, inputfile) != NULL) {
        char var_name[100];
        char var_val[100];
        proc_assignment(line, var_name, var_val);
    }

    printf("%s\n", vars[0]);
}

int proc_assignment(char* line, char *var_name, char *var_val)
{
    const char* delim = "<-";
    const char* hash = "#";

    char* statement = strtok(line, hash);
    if (statement == NULL) {
        return 0;
    }

    char* first_part = strtok(statement, delim);
    char* second_part = strtok(NULL, delim);

    if (strtok(NULL, delim) != NULL || second_part == NULL) {
        return 1;
    }

    strip(first_part, var_name);
    strip(second_part, var_val);

    vars[num_vars++] = var_name;
    return 0;
}

void proc_statement(char* statement)
{
}

void process_fn(char* block)
{
    char* local_ids[MAX_ID];
}

void strip(char* line, char* out)
{
    for (int i = 0; i < strlen(line); i++) {
        if (line[i] != ' ') {
            strcpy(out, &line[i]);
            break;
        }
    }

    for (int i = strlen(out) - 1; i > 0; i--) {
        if (out[i] == ' ') {
            out[i] = '\0';
            continue;
        }

        break;
    }
}
