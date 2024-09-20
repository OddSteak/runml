//  CITS2002 Project 1 2024
//  Student1:   STUDENT-24000581   NAME-Mohammad Ashraf Qureshi
//  Student2:   STUDENT-23895849   NAME-Baasil Sidiqui
//  Platform:   Linux

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <sys/wait.h>
#include <unistd.h>

// maximum Identifiers the program will have is 50
#define MAX_ID 50

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

// track global line count for error messages
int line_count = 0;
// structure to hold the names of generated files
struct {
    char* inputpath;
    char* varpath;
    char* mainpath;
    char* fnpath;
    char* cpath;
    char* binpath;
} fpaths;

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
struct fds init_fds(int argc, char** argv);
void merge_files(struct fds fdlist);
void runml();
void error_and_clean(char* format, ...);
void clean_files();
void init_paths(char* inputpath);

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
    if (strncmp(name, "arg", strlen("arg")) || strlen(name) <= strlen("arg"))
        return false;

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
    if (is_arg(name))
        error_and_clean("!identifiers of the form arg<number> are reserved for arguments\n");

    if (!strcmp(name, "function") || !strcmp(name, "print") || !strcmp(name, "return"))
        error_and_clean("!reserved keyword '%s' cannot be used as an identifier name\n", name);

    for (int i = 0; i < (int)strlen(name); i++)
        if (!islower(name[i]) || i == 12)
            error_and_clean("!Identifier name '%s' is invalid\n", name);
}

// return true if the identifier name is defined in the array names var_arr
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
        if (i >= (int)strlen(string))
            error_and_clean("!brackets are invalid on line %d\n", line_count);

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

    if (!strcmp(line, ""))
        // Assumming the expressions are valid, handle_exp() would be called with an
        // empty string if and only if an expression was expected but not received
        error_and_clean("!Expression expected at line %d", line_count);

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
                // this is function call since the opening bracket doesn't start at 0 but ends at the end
                handle_fncalls(line, var_arr, size, varfd);
                return;
            }

            // otherwise we will simply move the pointer to the closing bracket
            i = close + 1;
        } else if (line[i] == '+' || line[i] == '-' || line[i] == '*' || line[i] == '/') {
            // splitting at the operators
            char first_part[i + 1];
            strncpy(first_part, line, i);
            first_part[i] = 0;
            char* second_part = line + i + 1;

            // we know line + i + 1 is a valid address these operators can't be at the end
            // assuming the expressions are valid
            handle_exp(first_part, var_arr, size, varfd);
            handle_exp(second_part, var_arr, size, varfd);
            return;
        }
    }

    // there are no operators or brackets in the expression so it's either a real number or an identifier
    char* endptr;
    errno = 0;
    strtod(line, &endptr);

    // if strtod failed, we can assume it's an identifier, we don't need to do anything if it's a real number
    if ((errno != 0 || *endptr != '\0') && !is_var_defined(line, var_arr, *size)) {
        if (is_fn_defined(line))
            error_and_clean("!line %d - identifier already defined as a function '%s'", line_count, line);
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
    if (strstr(var_val, delim) != NULL)
        error_and_clean("!Line: %d - multiple arrows in assignment statement\n", line_count);

    var_name = strip(var_name);
    var_val = strip(var_val);
    validate_id(var_name);
    handle_exp(var_val, var_arr, size, fdlist.varfd);

    // if it's a new variable we need to declare it first
    if (!is_var_defined(var_name, var_arr, *size)) {
        if (is_fn_defined(var_name))
            error_and_clean("!line %d - identifier already defined as a function '%s'", line_count, var_name);

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
    // called with procline and it's not a function call
    if (opbrack == 0 || opbrack == (int)strlen(line) || resolve_bracket(line + opbrack) != (int)strlen(line + opbrack) - 1)
        error_and_clean("!Syntax ERROR: unrecognized statement on line %d\n", line_count);

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
                        if (args != 0 || strcmp(strip(call + exp_start), ""))
                            error_and_clean("!Line %d - empty argument in function call '%s'\n", line_count, call);

                        break;
                    }

                    args++;
                    handle_exp(exp, var_arr, size, varfd);
                    exp_start = exp_end + 1;
                }
            }

            if (args < fn_ac)
                error_and_clean("!Line %d - expected at least %d arguments in function call '%s'\n", line_count, fn_ac, line);

            return;
        }
    }

    error_and_clean("!Line %d - function '%s' is not defined\n", line_count, fn_name);
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

    if (is_fn_defined(fnname) || is_var_defined(fnname, vars, num_vars))
        error_and_clean("!Line %d - An identifier with name '%s' is already defined\n", line_count, strfn.name);

    while (true) {
        char* buf = strtok(NULL, " ");
        if (buf == NULL)
            break;

        char* param = malloc(strlen(buf) + 1);
        strcpy(param, buf);
        param = strip(param);
        validate_id(param);

        if (is_var_defined(param, local_ids, num_locids) || is_fn_defined(param))
            error_and_clean("!line %d - An identifier with name '%s' is already defined\n", line_count, param);

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
    if (line[0] == '\t' && fdlist.fnfd != NULL)
        error_and_clean("!Line %d - statement outside a function definition is indented\n", line_count);

    if (!strcmp(strip(line), ""))
        error_and_clean("!Line %d is empty", line_count);

    line = preprocess(line);
    if (!strcmp(line, "")) // must be a comment because we already checked for empty lines
        return;

    if (strstr(line, "<-") != NULL) {
        handle_assignment(line, var_arr, size, fdlist);
    } else if (strncmp(line, "print ", 6) == 0) {
        handle_print(line + 6, var_arr, size, fdlist);
    } else if (strncmp(line, "function ", 9) == 0) {
        if (fdlist.fnfd == NULL)
            error_and_clean("!line %d - nested functions are not allowed\n", line_count);

        handle_fndef(line + 9, fdlist);
    } else if (strncmp(line, "return ", 7) == 0) {
		// return statements are handled by handle_fndef so we must be outside function body
        error_and_clean("!line %d - return statement is not allowed outside function body\n", line_count);
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

struct fds init_fds(int argc, char** argv)
{
    struct fds fdlist;
    fdlist.infd = fopen(fpaths.inputpath, "r");
    fdlist.varfd = fopen(fpaths.varpath, "w+");
    fdlist.mainfd = fopen(fpaths.mainpath, "w+");
    fdlist.fnfd = fopen(fpaths.fnpath, "w+");

    fputs("#include <stdio.h>\n\n", fdlist.varfd);

    for (int i = 0; i < argc; i++) {
		// check if all the areguments are real numbers and define them as variables in the translated c file
        char* endptr;
        errno = 0;
        strtod(argv[i], &endptr);
        if (errno != 0 || *endptr != '\0' || (int)strlen(argv[i]) > 1023)
            error_and_clean("!Argument '%s' is not a real number or is too long\n", argv[i]);

        char* var_name = malloc(1024);
        fprintf(fdlist.varfd, "double arg%d = %s;\n", i, argv[i]);
        snprintf(var_name, 1024, "arg%d", i);
        vars[num_vars++] = var_name;
    }

	// we will use __val__ variable for print statements to ensure that integers are printed without decimal places
    fputs("double __val__;\n", fdlist.varfd);
    fputs("int main(void) {\n", fdlist.mainfd);
    return fdlist;
}

void merge_files(struct fds fdlist)
{
    fputs("}\n", fdlist.mainfd);
    rewind(fdlist.varfd);
    rewind(fdlist.mainfd);
    rewind(fdlist.fnfd);
    FILE* outfd = fopen(fpaths.cpath, "w");

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

void runml()
{
    int pid = fork();
    if (pid == 0) {
        char* args[] = { "cc", "-std=c11", "-o", fpaths.binpath, fpaths.cpath, NULL };
        execvp(args[0], args);
    } else {
        wait(&pid);
    }

    pid = fork();
    if (pid == 0) {
        char* args[] = { fpaths.binpath, NULL };
        execvp(args[0], args);
    } else {
        wait(&pid);
    }
}

// print error message, clean the generated files and exit
void error_and_clean(char* format, ...)
{
    va_list args;
    va_start(args, format);

    vfprintf(stderr, format, args);
    clean_files();
    exit(EXIT_FAILURE);
}

// clean the generated files
void clean_files()
{
    unlink(fpaths.varpath);
    unlink(fpaths.mainpath);
    unlink(fpaths.fnpath);
    unlink(fpaths.cpath);
    unlink(fpaths.binpath);
}

// initialize the global struct of filenames
void init_paths(char* inputpath)
{
    int pid = getpid();
    fpaths.inputpath = inputpath;
    fpaths.varpath = malloc(1024);
    fpaths.mainpath = malloc(1024);
    fpaths.fnpath = malloc(1024);
    fpaths.cpath = malloc(1024);
    fpaths.binpath = malloc(1024);

    sprintf(fpaths.varpath, "ml-%d-vars.c", pid);
    sprintf(fpaths.mainpath, "ml-%d-main.c", pid);
    sprintf(fpaths.fnpath, "ml-%d-fn.c", pid);
    sprintf(fpaths.cpath, "ml-%d.c", pid);
    sprintf(fpaths.binpath, "./ml-%d", pid);
}

// our program takes the file name and optional command line arguments
int main(int argc, char* argv[])
{
    if (argc < 2) {
        fprintf(stderr, "!Usage: runml input-file [argument]...\n");
        exit(EXIT_FAILURE);
    }

    init_paths(argv[1]);
    struct fds fdlist = init_fds(argc - 2, &argv[2]);
    procfile(fdlist);

    merge_files(fdlist);
    runml();
    clean_files();
}
