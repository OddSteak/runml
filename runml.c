#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct fn {
    char* name;
    int args;
};

struct fn fn_list[50];
int num_fns = 0;

char* vars[50];
int num_vars = 0;

void strip(char* line)
{
    for (int i = 0; i < strlen(line); i++) {
        if (line[i] != ' ') {
            line += i;
            break;
        }
    }

    for (int i = strlen(line) - 1; i > 0; i--) {
        if (line[i] == ' ') {
            line[i] = '\0';
            continue;
        }
        break;
    }
}

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

    if (strlen(var_name) > 12) {
        fprintf(stderr, "identifier %s longer than max 12 characters", var_name);
        exit(EXIT_FAILURE);
    }

    strip(var_name);
    strip(var_val);

    var_arr[size++] = var_name;
    return 0;
}

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

int main(int argc, char* argv[])
{
    if (argc == 1) {
        printf("No files provided");
        exit(EXIT_FAILURE);
    }

    procfile(argv[1]);
}

