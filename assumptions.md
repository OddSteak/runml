# Assumptions

- whitespace is allowed everywhere
- empty lines are not allowed
- tab character at the start of the line is only allowed in functions
- over indentation is allowed
- names of the form arg<number> cannot be reassigned or used as new identifiers even if they are not supplied
    - **Reason**: usage of the names indicate that the programmer was expecting an argument to be supplied and we should throw an error if that's not the case
- functions must be defined before they are called and the number of arguments must be at least equal to the number of parameters defined in the function definition
- reassignment of all variables (including function arguments) except the command line arguments is allowed
- function parameter names must be unique and cannot override global variables
- All exprestions passed are valid expressions
- There are no semi-colans in the file
- No meaningful code is written on the # line once the "#" appears
- Assuming statements are wriiten one per line
- Assuming there are no random semi colans\
- Assuming the function should be terminated when:
    - when brackets are in validate_id
    - when resurved words are used to name functions such as:
        function
        print
        arg<number>
    - when number of arguments passed to a function don't match the number of arguments that should be passed to the function
    - function is not defined before its used
    - when there are 2 or more "<-" in one line of code
    - if there are indentation error on the line following a function definition
    - if a function is tried to be defined more than once
    - if argument is not a valid number
