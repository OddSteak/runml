#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (int argc, char * argv[]){
    if (argc == 1){
        printf("No files provided");
        exit(EXIT_FAILURE);
    }

    FILE *inputfile =  fopen(argv[1] , "r");
    char line[1000];
    readline(line);


    

}

void readline(char* line){
    fget

}