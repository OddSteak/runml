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
    char* local_ids[50];
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
