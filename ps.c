#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>

int is_numeric(const char* str){
    for(int i = 0; i<strlen(str); i++){
        if(!isdigit(str[i])){
            return 0;
        }
    }

    return 1;
}

void ps();


int main(int argc, char *argv[])
{
    if(argc == 1){
        ps();
    }else if(argc == 2&& strcmp(argv[1], "-aux") == 0){
        ps();
    }else{
        fprintf(stderr, "Usage: %s [-aux]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}



