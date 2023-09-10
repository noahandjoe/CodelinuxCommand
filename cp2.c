#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#define BUFFERSIZE 4096     // 定义缓冲区大小
#define COPYMODE 0644       // 定义目标文件的权限

/*
0664表示文件的权限为：

文件所有者（User）拥有读（4）、写（2）的权限，但没有执行（1）的权限；
文件所有者所在的用户组（Group）拥有读（4）和写（2）的权限，但没有执行（1）的权限；
其他用户（Other）拥有读（4）的权限，但没有写（2）和执行（1）的权限。
因此，0664权限表示文件所有者和用户组有读写权限，其他用户只有读权限。

*/


void oops(const char*, const char*);

int main(int argc, char const *argv[])
{
    int in_fd, out_fd, n_chars;
    char buf[BUFFERSIZE];

    // 检查命令行参数的数量，如果不是3个，则打印用法提示并退出程序
    if(argc != 3){
        fprintf(stderr, "usage: %s source destination\n", *argv);
        exit(1);
    }

    //源文件和目标文件相同时的情况
    if(strcmp(argv[1], argv[2]) == 0){
        fprintf(stderr, "cp: '%s' and '%s' are the same file\n", argv[1], argv[2]);
        exit(EXIT_FAILURE);
    }
    
    // 打开源文件
    if((in_fd = open(argv[1], O_RDONLY)) == -1)
        oops("Cannot open", argv[1]);
    // 创建目标文件
    if((out_fd = creat(argv[2], COPYMODE)) == -1)
        oops("Cannot creat", argv[2]);
        
    // 读取源文件的内容，并将其写入目标文件中
    while((n_chars = read(in_fd, buf, BUFFERSIZE)) > 0){
        if(write(out_fd, buf, n_chars) != n_chars)
            oops("write err to ",argv[2]);
    }
    // 处理读取源文件时的错误
    if(n_chars == -1)
        oops("Read err from ", argv[1]);
    // 关闭文件
    if(close(in_fd) == -1 || close(out_fd) == -1)
        oops("Error closing files", "");
    
    return 0;
}

// 错误处理函数
void oops(const char* s1, const char* s2){
    fprintf(stderr, "Error: %s ", s1);
    perror(s2);
    exit(1);
}