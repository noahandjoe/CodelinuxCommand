/*
接受一个或多个目录名作为参数，并显示这些目录下的所有文件和子目录。

*/

#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

void do_ls(char[]);

int main(int argc, char* argv[])
{
    //如果没有传递任何参数，则显示当前目录下的所有文件和子目录。
    if(argc == 1)
        do_ls(".");
    else
        //如果传递了一个或多个参数，则显示每个目录下的所有文件和子目录
        while(--argc){
            printf("%s:\n", ++argv);
            do_ls(*argv);
        }

    return 0;
}

void do_ls(char dirname[]){
    DIR* dir_ptr;
    struct dirent* direntp;
    //打开指定的目录，返回一个指向该目录的指针
    if((dir_ptr = opendir(dirname)) == NULL)
        fprintf(stderr, "ls1: cannot open %s\n", dirname);
    else{
        //循环读取该目录下的每个文件和子目录，返回一个指向struct dirent结构体的指针
        while((direntp = readdir(dir_ptr)) != NULL)
            printf("%s\n", direntp->d_name);

        closedir(dir_ptr);
    }
}


/*

需要继续完成的：
排序
分栏
-a 参数时才会显示 ‘.’目录
-l 详细信息
*/