#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int i, printNewLine = 1;

    //检查当前的-n参数
    if(argc>1 && strcmp(argv[1], "-n") == 0){
        printNewLine = 0;
        i = 2;
    }else{
        i = 1;
    }

    for(; i<argc; i++){
        char* arg = argv[i];
        //以 $ 开头的字符串
        if(arg[0] == '$'){
            char* envVar = getenv(arg + 1);
            if(envVar != NULL){
                printf("%s", envVar);
            }
        }else{
            printf("%s", arg);
        }

        //在参数之间需要用空格连接，除非它是最后一个
        if(i != argc - 1){
            printf(" ");
        }
    }

    if(printNewLine){
        printf("\n");
    }

    
    return 0;
}
