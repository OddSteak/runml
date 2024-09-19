//  CITS2002 Project 1 2024
//  Student1:   STUDENT-24000581   NAME-Mohammad Ashraf Qureshi
//  Student2:   STUDENT-23895849   NAME-Baasil Sidiqui
//  Platform:   Linux

#include <ctype.h>
#include <errno.h>
#include <linux/limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

// TODO delete the generated files before exit_failures before submitting
// maximum Identifiers the program will have is 50
#define MAX_ID 50

// track global line count for error messages
int line_count = 0;

// defines the stucture of fn which hold the name and the number of aruments a function has will use these to process functions
struct fn {
    char* name;
    int ac;
};
// structure to manage file descriptors
struct fds {
    FILE* infd;
    FILE* varfd;
    FILE* mainfd;
    FILE* fnfd;
};

// fn_list is a list of the stuctures for function so we can track them
// num_fns is used to track the number of function we have in the program
struct fn fn_list[MAX_ID];
int num_fns = 0;

// var is a list a list of unique identifiers which are variables in the program
// num_vars keeps a count of the number of variables
char* vars[MAX_ID];
int num_vars = 0;

// declaring all the function prototypes used in the program
char* strip(char* line);
char* preprocess(char* line);
bool is_arg(char* name);
void validate_id(char* name);
bool is_var_defined(char* name, char* var_arr[], int size);
bool is_fn_defined(char* name);
int resolve_bracket(char* string);
void handle_exp(char* line, char* var_arr[], int* size, FILE* varfd);
void handle_print(char* line, char* var_arr[], int* size, struct fds fdlist);
void handle_assignment(char* line, char* var_arr[], int* size, struct fds fdlist);
void handle_fncalls(char* line, char* var_arr[], int* size, FILE* varfd);
void handle_fndef(char* line, struct fds fdlist);
void procline(char* line, char* var_arr[], int* size, struct fds fdlist);
void procfile(struct fds fdlist);
struct fds init_fds(char* inpath, char* varpath, char* mainpath, char* fnpath, int argc, char** argv);
void merge_files(struct fds fdlist, char* outpath);
void runml(char* filename, char* binpath);

// strip function removes the leading and trailing whitespaces
char* strip(char* line)
{
    for (int i = 0; i <= (int)strlen(line); i++) {
        if (line[i] != ' ' && line[i] != '\t') {
            line += i;
            break;
        }
    }
    for (int i = strlen(line) - 1; i >= 0; i--) {
        if (line[i] == ' ' || line[i] == '\n') {
            line[i] = 0;
            continue;
        }
        break;
    }

    return line;
}

// preprocessor function ends the string at # as the rest of the line is not needed for the compiler
// and strips the leading and trailing whitespace
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

// returns true if the name is of the form arg<number>
bool is_arg(char* name)
{
    if (strncmp(name, "arg", strlen("arg")) || strlen(name) <= strlen("arg")) {
        return false;
    }

    char* endptr = name + 3;
    errno = 0;
    strtol(name + 3, &endptr, 10);

    if (errno != 0 || *endptr != '\0') {
        // strtol faied
        return false;
    }
    return true;
}

// fail if the variable name is invalid
void validate_id(char* name)
{
    if (is_arg(name)) {
        fprintf(stderr, "!identifiers of the form arg<number> are reserved for arguments\n");
        exit(EXIT_FAILURE);
    }
    if (!strcmp(name, "function") || !strcmp(name, "print") || !strcmp(name, "return")) {
        fprintf(stderr, "!reserved keyword '%s' cannot be used as an identifier name\n", name);
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < (int)strlen(name); i++) {
        if (!islower(name[i]) || i == 12) {
            fprintf(stderr, "!Identifier name '%s' is invalid\n", name);
            exit(EXIT_FAILURE);
        }
    }
}

// return true if the identifier name is defined in the list of identifiers var_arr
bool is_var_defined(char* name, char* var_arr[], int size)
{
    for (int i = 0; i < size; i++)
        if (strcmp(var_arr[i], name) == 0)
            return true;

    return false;
}

// returns true if a function is defined with the name
bool is_fn_defined(char* name)
{
    for (int i = 0; i < num_fns; i++)
        if (strcmp(fn_list[i].name, name) == 0)
            return true;

    return false;
}

// must be called with a string starting with an opening bracket and it will return the position of the closing bracket
int resolve_bracket(char* string)
{
    int op_brack = 1;
    int cl_brack = 0;
    int i = 1;
    while (op_brack != cl_brack) {
        if (i >= (int)strlen(string)) {
            fprintf(stderr, "!brackets are invalid on line %d\n", line_count);
            exit(EXIT_FAILURE);
        }
        if (string[i] == '(') {
            op_brack = op_brack + 1;
        } else if (string[i] == ')') {
            cl_brack = cl_brack + 1;
        }
        i++;
    }
    return i - 1;
}

void handle_exp(char* line, char* var_arr[], int* size, FILE* varfd)
{
    line = strip(line);

    if (!strcmp(line, "")) {
        // Assumming the expressions are valid, handle_exp() would be called with an
        // empty string if and only if an expression was expected but not received
        fprintf(stderr, "!Expression expected at line %d", line_count);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < (int)strlen(line); i++) {
        if (line[i] == '(') {
            int close = i + resolve_bracket(line + i);
            if (i == 0 && close == (int)strlen(line) - 1) {
                // everything is within the bracket
                char cont[close];
                strncpy(cont, line + 1, close - 1);
                cont[close - 1] = 0;
                handle_exp(cont, var_arr, size, varfd);
                return;
            } else if (close == (int)strlen(line) - 1) {
                // this is fucntion call since the opening brack doesn't
                // start at 0 but ends at the end
                handle_fncalls(line, var_arr, size, varfd);
                return;
            }

            // otherwise we will simply move the pointer to the closing brack
            i = close + 1;
        } else if (line[i] == '+' || line[i] == '-' || line[i] == '*' || line[i] == '/') {
            char first_part[i + 1];
            strncpy(first_part, line, i);
            first_part[i] = 0;

            // we know line + i + 1 is a valid address these operators can't be at the end
            // assuming the expressions are valid
            handle_exp(first_part, var_arr, size, varfd);
            handle_exp(line + i + 1, var_arr, size, varfd);
            return;
        }
    }

    char* endptr;
    errno = 0;
    strtod(line, &endptr);

    // if strtod failed, we can assume it's an identifier
    if ((errno != 0 || *endptr != '\0') && !is_var_defined(line, var_arr, *size)) {
        if (is_fn_defined(line)) {
            fprintf(stderr, "!ERROR: line %d - identifier already defined as a function '%s'", line_count, line);
            exit(EXIT_FAILURE);
        }
        validate_id(line);
        char* var_name = malloc(strlen(line) + 1);
        strcpy(var_name, line);
        var_arr[(*size)++] = var_name;
        fprintf(varfd, "double %s = 0.0;\n", var_name);
    }
}

void handle_print(char* line, char* var_arr[], int* size, struct fds fdlist)
{
    handle_exp(line, var_arr, size, fdlist.varfd);
    fprintf(fdlist.mainfd, "__val__ = %s;\n", line);
    fprintf(fdlist.mainfd, "if (__val__ == (int)(__val__))\n");
    fprintf(fdlist.mainfd, "\tprintf(\"%%.0lf\\n\", __val__);\n");
    fprintf(fdlist.mainfd, "else\n\tprintf(\"%%.6lf\\n\", __val__);\n\n");
}

void handle_assignment(char* line, char* var_arr[], int* size, struct fds fdlist)
{
    const char* delim = "<-";
    char* token = strstr(line, delim);
    *token = 0;

    char* var_name = malloc(strlen(line) + 1);
    strcpy(var_name, line);
    char* var_val = token + strlen(delim);

    // invalid syntax if we have multiple arrows
    if (strstr(var_val, delim) != NULL) {
        fprintf(stderr, "!Line: %d - multiple arrows in assignment statement\n", line_count);
        exit(EXIT_FAILURE);
    }

    var_name = strip(var_name);
    var_val = strip(var_val);
    validate_id(var_name);
    handle_exp(var_val, var_arr, size, fdlist.varfd);

    // if it's a new variable we need to declare it first
    if (!is_var_defined(var_name, var_arr, *size)) {
        if (is_fn_defined(var_name)) {
            fprintf(stderr, "!ERROR: line %d - identifier already defined as a function '%s'", line_count, var_name);
            exit(EXIT_FAILURE);
        }
        var_arr[(*size)++] = var_name;
        fprintf(fdlist.varfd, "double %s;\n", var_name);
    }
    fprintf(fdlist.mainfd, "%s = %s;\n", var_name, var_val);
}

void handle_fncalls(char* line, char* var_arr[], int* size, FILE* varfd)
{
    int opbrack;
    for (opbrack = 0; opbrack < (int)strlen(line); opbrack++)
        if (line[opbrack] == '(')
            break;

    // handle_exp confirms it's a function call before calling handle_fncalls and we can't recognize the statement if
    // called with procline and it's not a funciton call
    if (opbrack == 0 || opbrack == (int)strlen(line) || resolve_bracket(line + opbrack) != (int)strlen(line + opbrack) - 1) {
        fprintf(stderr, "!Syntax ERROR: unrecognized statement on line %d\n", line_count);
        exit(EXIT_FAILURE);
    }

    // copying line to another buffer because we want to modify it
    char call[strlen(line) + 1];
    strcpy(call, line);
    call[strlen(call) - 1] = 0;

    char buf[opbrack + 1];
    strncpy(buf, call, opbrack);
    buf[opbrack] = '\0';
    char* fn_name = buf;
    fn_name = strip(fn_name);

    for (int i = 0; i < num_fns; i++) {
        if (strcmp(fn_list[i].name, fn_name) == 0) {
            int fn_ac = fn_list[i].ac;
            int exp_start = opbrack + 1;
            int exp_end = opbrack + 1;
            int args = 0;

            for (; exp_end < (int)strlen(call) + 1; exp_end++) {
                if (call[exp_end] == '(')
                    exp_end += resolve_bracket(&call[exp_end]) + 1;

                if (call[exp_end] == ',' || call[exp_end] == 0) {
                    char exp[exp_end - exp_start + 1];
                    strncpy(exp, call + exp_start, exp_end - exp_start);
                    exp[exp_end - exp_start] = '\0';

                    // empty argument is only allowed if no arguments are passed
                    if (!strcmp(strip(exp), "")) {
                        if (args != 0 || strcmp(strip(call + exp_start), "")) {
                            fprintf(stderr, "!Line %d - empty argument in function call '%s'\n", line_count, call);
                            exit(EXIT_FAILURE);
                        }
                        break;
                    }

                    args++;
                    handle_exp(exp, var_arr, size, varfd);
                    exp_start = exp_end + 1;
                }
            }

            if (args < fn_ac) {
                fprintf(stderr, "!Line %d - expected at least %d arguments in function call '%s'\n", line_count, fn_ac, line);
                exit(EXIT_FAILURE);
            }
            return;
        }
    }

    fprintf(stderr, "!Line %d - function '%s' is not defined\n", line_count, fn_name);
    exit(EXIT_FAILURE);
}

void handle_fndef(char* line, struct fds fdlist)
{
    struct fn strfn;

    char* local_ids[MAX_ID];
    int num_locids = num_vars;
    for (int i = 0; i < num_vars; i++)
        local_ids[i] = vars[i];

    char* fnname = strtok(line, " ");
    validate_id(fnname);
    strfn.name = malloc(strlen(fnname) + 1);
    strcpy(strfn.name, fnname);

    if (is_fn_defined(fnname) || is_var_defined(fnname, vars, num_vars)) {
        fprintf(stderr, "!Line %d - An identifier with name '%s' is already defined\n", line_count, strfn.name);
        exit(EXIT_FAILURE);
    }

    while (true) {
        char* buf = strtok(NULL, " ");
        if (buf == NULL)
            break;

        char* param = malloc(strlen(buf) + 1);
        strcpy(param, buf);
        param = strip(param);
        validate_id(param);

        if (is_var_defined(param, local_ids, num_locids) || is_fn_defined(param)) {
            fprintf(stderr, "!line %d - An identifier with name '%s' is already defined\n", line_count, param);
            exit(EXIT_FAILURE);
        }

        local_ids[num_locids++] = param;
    }

    strfn.ac = num_locids - num_vars;
    fn_list[num_fns++] = strfn;

    fprintf(fdlist.fnfd, "double %s(", strfn.name);
    if (strfn.ac > 0) {
        for (int i = 0; i < strfn.ac - 1; i++) {
            fprintf(fdlist.fnfd, "double %s, ", local_ids[i + num_vars]);
        }
        fprintf(fdlist.fnfd, "double %s) {\n", local_ids[num_locids - 1]);
    } else {
        fprintf(fdlist.fnfd, ") {\n");
    }

    char buf[BUFSIZ];
    // used to track if the function has a return statement
    bool has_return = false;
    // struct to write everything to the function file descriptor
    // setting fnfd to NULL to indicate that we are inside a funciton
    struct fds fnfdlist = { .infd = fdlist.infd, .varfd = fdlist.fnfd, .mainfd = fdlist.fnfd, .fnfd = NULL };

    while (fgets(buf, sizeof buf, fdlist.infd) != NULL) {
        line_count++;
        // leave the function if the next line doesn't start with a tab
        if (buf[0] != '\t') {
            // return 0.0 by default if the function doesn't have a return statement
            if (!has_return)
                fprintf(fdlist.fnfd, "return 0.0;\n");
            fprintf(fdlist.fnfd, "}\n\n");
            procline(buf, vars, &num_vars, fdlist);
            return;
        }

        if (!strncmp(buf, "\treturn ", 8)) {
            has_return = true;
            handle_exp(buf + 8, local_ids, &num_locids, fdlist.fnfd);
            fprintf(fdlist.fnfd, "return %s;\n", buf + 8);
            continue;
        }

        procline(buf, local_ids, &num_locids, fnfdlist);
    }

    fprintf(fdlist.fnfd, "}\n\n");
}

void procline(char* line, char* var_arr[], int* size, struct fds fdlist)
{
    if (line[0] == '\t' && fdlist.fnfd != NULL) {
        fprintf(stderr, "!Line %d - statement outside a function definition is indented\n", line_count);
        exit(EXIT_FAILURE);
    }
    if (!strcmp(strip(line), "")) {
        fprintf(stderr, "!Line %d is empty", line_count);
        exit(EXIT_FAILURE);
    }
    line = preprocess(line);
    if (!strcmp(line, ""))
        return;

    if (strstr(line, "<-") != NULL) {
        handle_assignment(line, var_arr, size, fdlist);
    } else if (strncmp(line, "print ", 6) == 0) {
        handle_print(line + 6, var_arr, size, fdlist);
    } else if (strncmp(line, "function ", 9) == 0) {
        if (fdlist.fnfd == NULL) {
            fprintf(stderr, "!line %d - nested functions are not allowed\n", line_count);
            exit(EXIT_FAILURE);
        }
        handle_fndef(line + 9, fdlist);
    } else if (strncmp(line, "return ", 7) == 0) {
        fprintf(stderr, "!line %d - return statement is not allowed outside function definition\n", line_count);
        exit(EXIT_FAILURE);
    } else {
        handle_fncalls(line, var_arr, size, fdlist.varfd);
        fprintf(fdlist.mainfd, "%s;\n", line);
    }
}

void procfile(struct fds fdlist)
{
    char line[BUFSIZ];

    while (fgets(line, sizeof line, fdlist.infd) != NULL) {
        line_count++;
        procline(line, vars, &num_vars, fdlist);
    }
}

struct fds init_fds(char* inpath, char* varpath, char* mainpath, char* fnpath, int argc, char** argv)
{
    struct fds fdlist;
    fdlist.infd = fopen(inpath, "r");
    fdlist.varfd = fopen(varpath, "w+");
    fdlist.mainfd = fopen(mainpath, "w+");
    fdlist.fnfd = fopen(fnpath, "w+");

    fputs("#include <stdio.h>\n\n", fdlist.varfd);

    for (int i = 0; i < argc; i++) {
        char* endptr;
        errno = 0;
        strtod(argv[i], &endptr);
        if (errno != 0 || *endptr != '\0' || (int)strlen(argv[i]) > 1023) {
            fprintf(stderr, "!Argument '%s' is not a real number or is too long", argv[i]);
            exit(EXIT_FAILURE);
        }

        char* var_name = malloc(1024);
        fprintf(fdlist.varfd, "double arg%d = %s;\n", i, argv[i]);
        snprintf(var_name, 1024, "arg%d", i);
        vars[num_vars++] = var_name;
    }

    fputs("double __val__;\n", fdlist.varfd);
    fputs("int main(void) {\n", fdlist.mainfd);
    return fdlist;
}

void merge_files(struct fds fdlist, char* outpath)
{
    fputs("}\n", fdlist.mainfd);
    rewind(fdlist.varfd);
    rewind(fdlist.mainfd);
    rewind(fdlist.fnfd);
    FILE* outfd = fopen(outpath, "w");

    char buf[BUFSIZ];

    while (fgets(buf, sizeof buf, fdlist.varfd) != NULL)
        fputs(buf, outfd);

    fputs("\n", outfd);

    while (fgets(buf, sizeof buf, fdlist.fnfd) != NULL)
        fputs(buf, outfd);

    fputs("\n", outfd);

    while (fgets(buf, sizeof buf, fdlist.mainfd) != NULL)
        fputs(buf, outfd);

    fclose(fdlist.infd);
    fclose(fdlist.varfd);
    fclose(fdlist.mainfd);
    fclose(fdlist.fnfd);
    fclose(outfd);
}

void runml(char* filename, char* binpath)
{
    int pid = fork();
    if (pid == 0) {
        char* args[] = { "cc", "-Wall", "-o", binpath, filename, NULL };
        execvp(args[0], args);
    } else {
        wait(&pid);
    }

    pid = fork();
    if (pid == 0) {
        char* args[] = { binpath, NULL };
        execvp(args[0], args);
    } else {
        wait(&pid);
    }
}

// our program takes the file name and optional command line arguments
int main(int argc, char* argv[])
{
    if (argc < 2) {
        fprintf(stderr, "!Usage: runml input-file [arguments]...\n");
        exit(EXIT_FAILURE);
    }

    int pid = getpid();
    char varpath[1024];
    char mainpath[1024];
    char fnpath[1024];
    char outpath[1024];
    char binpath[1024];

    sprintf(varpath, "ml-%d-vars.c", pid);
    sprintf(mainpath, "ml-%d-main.c", pid);
    sprintf(fnpath, "ml-%d-fn.c", pid);
    sprintf(outpath, "ml-%d.c", pid);
    sprintf(binpath, "./ml-%d", pid);

    struct fds fdlist = init_fds(argv[1], varpath, mainpath, fnpath, argc - 2, &argv[2]);
    procfile(fdlist);

    merge_files(fdlist, outpath);
    runml(outpath, binpath);

    unlink(varpath);
    unlink(mainpath);
    unlink(fnpath);
    unlink(outpath);
    unlink(binpath);
}
