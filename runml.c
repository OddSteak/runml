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
#include <unistd.h>
#include <sys/wait.h>

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

// strip function removes unnecessary spaces from the code
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
        if (line[i] == ' ' || line[i] == '\n') {
            line[i] = 0;
            continue;
        }
        break;
    }

    return line;
}

// preprocessor function ends the string at # as the rest of the line is not needed for the compiler
char* preprocess(char* line)
{
    for (int i = 0; i < (int)strlen(line); i++) {
        if (line[i] == '#') {
            line[i] = 0;
            break;
        }
    }

    return strip(line);
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

void handle_exp(char* line, char* var_arr[], int* size, FILE* varout)
{
    line = strip(line);

    for (int i = 0; i < (int)strlen(line); i++) {
        if (line[i] == '(') {
            int close = i + bracks(line + i);
            if (i == 0 && close == (int)strlen(line) - 1) {
                // everything is within the bracket
                char cont[close];
                strncpy(cont, line + 1, close - 1);
                handle_exp(cont, var_arr, size, varout);
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
            first_part[i] = 0;

            // we know line + i + 1 is a valid address these operators can't be at the end
            // assuming the expressions are valid
            handle_exp(first_part, var_arr, size, varout);
            handle_exp(line + i + 1, var_arr, size, varout);
            return;
        }
    }

    double convert = strtod(line, NULL);

    // if strtod failed, we can assume it's an identifier
    if ((strcmp(line, "0") && convert == 0) && !isDefined(line, var_arr, *size)) {
        isValidId(line);
        var_arr[size[0]++] = line;
        fprintf(varout, "double %s = 0.0;\n", line);
    }
}

void handle_print(char* line, char* var_arr[], int* size, FILE* outfile)
{
    handle_exp(line + 6, var_arr, size, outfile);
    fprintf(outfile, "printf(\"%%lf\", %s);\n", line + 6);
}

int handle_assignment(char* line, char* var_arr[], int* size, FILE* outfile)
{
    const char* delim = "<-";

    char* var_name = malloc(strlen(line) + 1);
    char* var_val = malloc(strlen(line) + 1);

    strcpy(var_name, strtok(line, delim));
    strcpy(var_val, strtok(NULL, delim));

    // invalid syntax if we have multiple arrows
    if (strtok(NULL, delim) != NULL) {
        fprintf(stderr, "!multiple arrows are not allowed in assignment statement\n");
        exit(EXIT_FAILURE);
    }

    var_name = strip(var_name);
    var_val = strip(var_val);

    isValidId(var_name);

    handle_exp(var_val, var_arr, size, outfile);
    var_arr[size[0]++] = var_name;
    fprintf(outfile, "double %s = %s;\n", var_name, var_val);
    return 0;
}

void handle_fndef(char* line, FILE* outfile)
{
    assert(false && "handle_function is not implemented yet\n");
}

void procline(char* line, char* var_arr[], int* size, FILE* varout, FILE* mainout)
{
    if (strncmp(line, "print ", 6) == 0)
        handle_print(line, vars, &num_vars, mainout);
    else if (strncmp(line, "function ", 9) == 0)
        handle_fndef(line, varout);
    else if (strstr(line, "<-") != NULL)
        handle_assignment(line, vars, &num_vars, varout);
    else
        handle_exp(line, vars, &num_vars, varout);
}

void procfile(char* filename, FILE* varout, FILE* mainout)
{
    FILE* infd = fopen(filename, "r");
    char line[10000];

    while (fgets(line, 10000, infd) != NULL) {
        char* prepped = preprocess(line);
        if (strcmp(prepped, "")) {
            procline(prepped, vars, &num_vars, varout, mainout);
        }
    }
}

FILE* init_vars(char* varpath)
{
    FILE* varout = fopen(varpath, "w");
    fputs("#include <stdio.h>\n\n", varout);
    return varout;
}

FILE* init_main(char* mainpath)
{
    FILE* mainout = fopen(mainpath, "w");

    fputs("int main(int ac, char *av[]) {\n", mainout);
    return mainout;
}

void close_main(FILE* mainout)
{
    fputs("}\n", mainout);
    fclose(mainout);
}

char* merge_files(char* varpath, char *mainpath, char* outpath)
{
    FILE* varfd = fopen(varpath, "r");
    FILE* mainfd = fopen(mainpath, "r");
    FILE* outfd = fopen(outpath, "w");

    char buf[10000];

    while (fgets(buf, 10000, varfd) != NULL) {
        fputs(buf, outfd);
    }
    fputs("\n", outfd);
    while (fgets(buf, 10000, mainfd) != NULL) {
        fputs(buf, outfd);
    }

    fclose(varfd);
    fclose(mainfd);
    fclose(outfd);

    unlink(varpath);
    unlink(mainpath);

    return outpath;
}

void comp_run(char* filename)
{
    char outpath[strlen(filename) + 1];
    strncat(strcpy(outpath, "./"), filename, strlen(filename) - 2);

    int pid = fork();

    if (pid == 0) {
        char* args[] = { "/usr/bin/gcc", "-o", outpath, filename, NULL };
        execvp(args[0], args);
    } else {
        wait(&pid);
    }

    pid = fork();

    if (pid == 0) {
        char* args[] = { outpath, NULL };
        execvp(args[0], args);
    } else {
        wait(&pid);
    }
}

// main function takes 1 anrgument which is file name
int main(int argc, char* argv[])
{
    if (argc != 2) {
        fprintf(stderr, "!Usage:runml [input-file]");
        exit(EXIT_FAILURE);
    }

    char varpath[1024];
    sprintf(varpath, "ml-%d-vars.c", getpid());

    char mainpath[1024];
    sprintf(mainpath, "ml-%d-main.c", getpid());

    FILE* varout = init_vars(varpath);
    FILE* mainout = init_main(mainpath);

    procfile(argv[1], varout, mainout);

    fclose(varout);
    close_main(mainout);

    char outpath[1024];
    sprintf(outpath, "ml-%d.c", getpid());

    merge_files(varpath, mainpath, outpath);
    comp_run(outpath);
}
