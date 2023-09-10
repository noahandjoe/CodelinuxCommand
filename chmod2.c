//一个简单的实现chmod命令的示例代码，该代码可以修改文件或目录的访问权限
//例： ./chmod 0777 testfile
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int main(int argc, char *argv[])
{
    if(argc < 3){
        printf("Usage: chmod MODE FILE\n");
        exit(EXIT_FAILURE);
    }

    char* mode_str = argv[1];
    char* file_path = argv[2];

    //将权限字符转换为数字模式
    int mode = strtol(mode_str, 0, 8);

    //修改文件权限
    if(chmod(file_path, mode) == -1){
        perror("chmod");
        exit(EXIT_FAILURE);
    }
    
    return 0;
}
