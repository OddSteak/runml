# Assumptions

- whitespace is allowed everywhere
- empty lines are allowed
- tab character at the start of the line is only allowed in functions
- over indentation is allowed
- names of the form arg<number> cannot be reassigned or used as new identifiers even if they are not supplied
    - *Reason*: usage of the names indicate that the programmer was expecting an argument to be supplied and we should throw an error if that's not the case
- functions must be defined before being called and the number of arguments must match the number of parameters defined in the function definition
- reassignment of all variables (including function arguments) except the command line arguments is allowed
