# Assumptions

- extra spaces are allowed everywhere
- empty lines are not allowed
- tab character at the start of the line is only allowed in function body
- over indentation is allowed
- names of the form arg<number> cannot be reassigned or used as new identifiers even if they are not supplied
    - **Reason**: usage of the names indicate that the programmer was expecting an argument to be supplied and we should throw an error if that's not the case
- reassignment of all variables (including function arguments) except the command line arguments is allowed
- function parameter names must be unique and cannot override global variables
- No meaningful code is written on the # line once the "#" appears
- Assuming statements are wriiten one per line

- behavior is undefined if -
    - a real number is outside the range defined in data type double in c
    - a reserved C11 keyword is used as an identifier
    - If an invalid expression is used
    - the number of unique identifiers (including command line arguments) exceeds 50

- Assuming our program should be terminated when:
    - identifier name is invalid
    - a reserved keyword is used to name identifiers such as:
        function
        print
        arg<number>
    - functions is not defined before it is called or the number of arguments is less than the number of parameters defined in the function definition
    - an expression was expected but not received
    - there are any syntax errors in the code such as more than one "<-" in assignment statement
    - if a line outside function definition is indented
    - A function is defined more than once or the function name clashes with a variable name
    - if there is a semicolon or any other unexpected symbols in the file
    - if any command line argument is not a valid real number
