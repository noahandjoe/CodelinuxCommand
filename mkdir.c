#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

int mkdir_p(char* path, mode_t mode);

int main(int argc, char *argv[])
{
    char* path;
    mode_t mode = 0777;
    int i, result;

    //解析参数
    if(argc < 2){
        printf("Usage: mkdir [-p] <path>\n");
        return -1;
    }

    if(strcmp(argv[1], "-p") == 0){
        if(argc < 3){
            printf("Usage: mkdir [-p] <path>\n");
            return -1;
        }
        path = argv[2];
    }else{
        path = argv[1];
    }

    //创建目录
    result = mkdir_p(path, mode);

    return result;
}

int mkdir_p(char* path, mode_t mode){
    char* p = path, *q = path;
    int len = strlen(path), result = 0;

    // 找到第一个存在的目录
    //循环会遍历 path 数组中的每一个字符，直到遇到字符串结束符 \0 为止
    while(q < (path + len)){
        if(*q == '/'){
            //截断path
            *q = '\0';
            //判断该目录是否存在，如果不存在，则创建该目录。
            if(access(path, F_OK) != 0){
                if(mkdir(path, mode) == -1){
                    printf("Error: cannot create directory %s\n", path);
                    result = -1;
                    break;
                }
            }
            //再还原回来
            *q = '/';
        }
        //指向下一个字符
        q++;
    }

    // 因为最后一节目录可能不是 '/'结尾
    // 创建路径最后一级的目录
    if(access(path, F_OK) != 0){
        if(mkdir(path, mode) == -1){
            printf("Error: cannot create directory %s\n", path);
            result = -1;
        }
    }
    return result;
}