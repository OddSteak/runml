//  CITS2002 Project 1 2024
//  Student1:   STUDENT-24000581   NAME-Mohammad Ashraf Qureshi
//  Student2:   STUDENT-23895849   NAME-Baasil Sidiqui
//  Platform:   Linux

// Importing all the the libraries we need
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// maximum Identifiers a the program will have is 50
#define MAX_ID 50

// defines the stucture of fn which hold the name and the number of aruments a function has will use these to process funvctions
struct fn {
    char* name;
    int args;
};
// fn_list is a list of the stuctures for function so we can track them, num_fns is used to track the number of function we have in the program
struct fn fn_list[MAX_ID];
int num_fns = 0;
// var is a list a list of unique identifiers which are variables in the program
char* vars[MAX_ID];
// num_vars keeps a count of the number of variables
int num_vars = 0;

int bracks(char* brack)
{
    int op_brack = 1;
    int cl_brack = 0;
    int i = 1;
    while (op_brack != cl_brack) {
        if (i >= (int)strlen(brack)) {
            fprintf(stderr, "!brackets are invalid - '%s'\n", brack);
            exit(EXIT_FAILURE);
        }
        if (brack[i] == '(') {
            op_brack = op_brack + 1;
        } else if (brack[i] == ')') {
            cl_brack = cl_brack + 1;
        }
        i++;
    }
    return i - 1;
}

// strip function removes unnessisary spaces from the code
char* strip(char* line)
{
    // first loop removes the leading empty space
    for (int i = 0; i < (int)strlen(line); i++) {
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

    return line;
}

// preprocessor function ends the string at # as the rest of the line is not needed for the compiler
void preprocess(char* line)
{
    for (int i = 0; i < (int)strlen(line); i++) {
        if (line[i] == '#') {
            line[i] = 0;
            break;
        }
    }

    line = strip(line);
}

int isValidId(char* name)
{
    for (int i = 0; i < (int)strlen(name); i++) {
        if (!islower(name[i]) || i == 12) {
            fprintf(stderr, "!Identifier name '%s' is invalid\n", name);
            exit(EXIT_FAILURE);
        }
    }
    return 0;
}

bool isDefined(char* name, char* var_arr[], int size)
{
    for (int i = 0; i < size; i++) {
        if (strcmp(var_arr[i], name) == 0) {
            return true;
        }
    }

    return false;
}

void handle_fncalls(char* line)
{
    assert(false && "handle_fncalls is not implemented yet\n");
}

void handle_exp(char* line, char* var_arr[], int* size)
{
    line = strip(line);

    for (int i = 0; i < (int)strlen(line); i++) {
        if (line[i] == '(') {
            int close = i + bracks(line + i);
            if (i == 0 && close == (int)strlen(line) - 1) {
                // everything is within the bracket
                char cont[close];
                strncpy(cont, line + 1, close - 1);
                handle_exp(cont, var_arr, size);
                return;
            } else if (close == (int)strlen(line) - 1) {
                // this is fucntion call since the opening brack doesn't
                // start at 0 but ends at the end
                handle_fncalls(line);
                return;
            }

            // otherwise we will simply move the pointer to the closing brack
            i = close + 1;
        } else if (line[i] == '+' || line[i] == '-'
            || line[i] == '*' || line[i] == '/') {
            char first_part[i + 1];
            strncpy(first_part, line, i);

            // we know line + i + 1 is a valid address these operators can't be at the end
            // assuming the expressions are valid
            handle_exp(first_part, var_arr, size);
            handle_exp(line + i + 1, var_arr, size);
            return;
        }
    }

    double convert = strtod(line, NULL);

    // if strtod failed, we can assume it's an identifier
    if ((strcmp(line, "0") && convert == 0) && !isDefined(line, var_arr, *size)) {
        isValidId(line);
        var_arr[size[0]++] = line;
    }
}

void handle_print(char* line, char* var_arr[], int* size)
{
    handle_exp(line + 6, var_arr, size);
}

int handle_assignment(char* line, char* var_arr[], int* size)
{
    const char* delim = "<-";

    char* var_name = malloc(strlen(line) + 1);
    char* var_val = malloc(strlen(line) + 1);

    strcpy(var_name, strtok(line, delim));
    strcpy(var_val, strtok(NULL, delim));

    // invalid syntax if we have multiple arrows
    if (strtok(NULL, delim) != NULL) {
        fprintf(stderr, "invalid syntax\n");
        exit(EXIT_FAILURE);
    }

    isValidId(var_name);

    line = strip(var_name);
    line = strip(var_val);

    handle_exp(var_val, var_arr, size);
    var_arr[size[0]++] = var_name;
    return 0;
}

void handle_function(char* line)
{
    assert(false && "handle_function is not implemented yet\n");
}

void procline(char* line, char* var_arr[], int* size)
{
    if (strncmp(line, "print ", 6) == 0)
        handle_print(line, vars, &num_vars);
    else if (strncmp(line, "function ", 9) == 0)
        handle_function(line);
    else if (strstr(line, "<-") == NULL)
        handle_assignment(line, vars, &num_vars);
    else
        handle_exp(line, vars, &num_vars);
}

void procfile(char* filename)
{
    FILE* infd = fopen(filename, "r");
    char line[10000];

    while (fgets(line, 10000, infd) != NULL) {
        preprocess(line);
        procline(line, vars, &num_vars);
    }
}

// main function takes 1 anrgument which is file name
int main(int argc, char* argv[])
{
    if (argc != 2) {
        fprintf(stderr, "!Usage:runml [input-file]");
        exit(EXIT_FAILURE);
    }

    procfile(argv[1]);
}
