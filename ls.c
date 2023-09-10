#include <stdio.h>
#include <dirent.h>

int main(int argc, char *argv[])
{
    DIR* dir;       // DIR类型指针，用于打开目录
    struct dirent* entry;   // 目录项类型，用于存储每个目录项的信息
    char* dir_name; // 存储目录名
    if(argc == 1)   // 如果ls命令没有指定目录名，则默认为当前目录
        dir_name = ".";
    else
        dir_name = argv[1];
    
    dir = opendir(dir_name);    // 打开目录
    if(!dir){
        perror("opendir");
        return 1;
    }

    while((entry = readdir(dir)) != NULL){  // 循环读取目录流中的下一个目录项
        printf("%s    ", entry->d_name);
    }
    printf("\n");
    closedir(dir);

    return 0;
}
