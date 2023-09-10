#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    FILE* fp;
    int c;

    if (argc<2){
        fprintf(stderr, "Usage: %s file1 [file2 ...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    for(int i = 1; i<argc; i++){
        if((fp = fopen(argv[i], "r")) == NULL){
            fprintf(stderr, "Error: cannot open file %s\n", argv[i]);
            exit(EXIT_FAILURE);
        }
        while((c = getc(fp)) != EOF){
            putchar(c);
        }
        fclose(fp);
    }
    exit(EXIT_SUCCESS);

    return 0;
}
