#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>

int touch(const char* filename, int modify_time);

int main(int argc, char *argv[])
{
    //参数, 是否修改时间
    int opt, modify_time = 0;

    // 解析命令行参数
    while((opt = getopt(argc, argv, "m")) != -1){
        switch(opt){
            case 'm':
                modify_time = 1;
                break;
            default:
                printf("Usage: touch [-m] <filename>\n");
                return -1;
        }
    }

    //下一个将要被处理的参数在 argv 中的下标
    if(optind >= argc){
        printf("Usage: touch [-m] <filename>\n");
        return -1;
    }
    //此时下标指向的是filename
    char* filename = argv[optind];

    //调用touch函数
    if(touch(filename, modify_time) == -1){
        return -1;
    }

    return 0;
}

int touch(const char* filename, int modify_time){
    int fd;
    struct stat st;
    struct timeval tv[2];

     // 如果文件存在，获取文件状态信息
    if(stat(filename, &st) == 0){
        // 如果不是普通文件，返回错误
        if(!S_ISREG(st.st_mode)) {
            printf("Error: %s is not a regular file\n", filename);
            return -1;
        }

        // 如果设置了 -m 选项，更新修改时间戳
        if(modify_time){
            gettimeofday(&tv[0], NULL);
            tv[1].tv_sec = st.st_mtime;
            if(utimes(filename, tv) == -1){
                printf("Error: cannot modify timestamp of %s\n", filename);
                return -1;
            }
        }else{
            // 如果文件不存在，创建一个新的空文件
            fd = open(filename, O_CREAT | O_EXCL, 0666);
            if (fd == -1) {
                printf("Error: cannot create file %s\n", filename);
                return -1;
            }
            close(fd);
        }
    }
}
