# Assumptions

- whitespace is allowed everywhere
- empty lines are allowed
- tab character at the start of the line is only allowed in functions
- over indentation is allowed
- names of the form arg<number> cannot be used as identifiers even if they are not supplied
    - *Reason*: usage of the names indicate that the programmer was expecting an argument to be supplied and we should throw an error if that's not the case
