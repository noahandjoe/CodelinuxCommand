#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
// #include <linux/limits.h>   //PATH_MAX

#define BUFFER_SIZE 1024
#define PATH_MAX    4096

// 复制单个文件
int copy_file(char* src_file, char* dst_file){
    int fd_src, fd_dst, num_read, num_write;
    char buffer[BUFFER_SIZE];

    // 打开源文件
    if((fd_src = open(src_file, O_RDONLY)) == -1){
        printf("Error: cannot open source file %s\n", src_file);
    }

    // 打开目标文件
    // O_TRUNC : 若文件存在，则长度被截为0，属性不变
    if((fd_dst = open(dst_file, O_WRONLY|O_CREAT|O_TRUNC, 0666)) == -1){
        printf("Error: cannot create target file %s\n", dst_file);
        return -1;
    }

    //复制数据
    while((num_read = read(fd_src, buffer, BUFFER_SIZE)) > 0){
        if((num_write = write(fd_dst, buffer, num_read)) != num_read){
            printf("Error: cannot write to target file %s\n", dst_file);
            close(fd_src);
            close(fd_dst);
            return -1;
        }
    }

    close(fd_src);
    close(fd_dst);
    return 0;
}

// 复制整个目录（递归）
int copy_dir(char* src_dir, char* dst_dir){
    DIR* dir_src;
    struct dirent* entry_src;
    struct stat stat_src;
    char src_path[PATH_MAX], dst_path[PATH_MAX];

    //打开源目录
    if((dir_src = opendir(src_dir)) == NULL){
        printf("Error: cannot open source directory %s\n", src_dir);
        return -1;
    }

    // 检查目标目录是否存在，如果不存在则创建
    if(access(dst_dir, F_OK) == -1){
        if(mkdir(dst_dir, 0777) == -1){
            printf("Error: cannot create target directory %s\n", dst_dir);
            closedir(dir_src);
            return -1;
        }
    }

    // 遍历源目录中的文件和子目录
    while((entry_src = readdir(dir_src)) != NULL){
        //忽略 . 和 ..
        if(strcmp(entry_src->d_name, ".") == 0 || strcmp(entry_src->d_name, "..") == 0){
            continue;
        }

        // 构造源路径和目标路径
        snprintf(src_path, PATH_MAX, "%s/%s", src_dir, entry_src->d_name);
        snprintf(dst_path, PATH_MAX, "%s/%s", dst_dir, entry_src->d_name);

        // 获取文件或目录的状态信息
        if(lstat(src_path, &stat_src) == -1){
            printf("Error: cannot get status of %s\n", src_path);
            continue;
        }

        if(S_ISDIR(stat_src.st_mode)){
            // 如果是目录，则递归复制
            if(copy_dir(src_path, dst_path) == -1){
                printf("Error: cannot copy directory %s\n", src_path);
                continue;
            }
        }else if(S_ISREG(stat_src.st_mode)){
            // 如果是普通文件，则复制
            if(copy_file(src_path, dst_path) == -1){
                printf("Error: cannot copy file %s\n", src_path);
                continue;
            }
        }else{
            // 如果是其他类型的文件，则忽略
            printf("Warning: ignoring %s\n", src_path);
            continue;
        }
        closedir(dir_src);

        return 0;
    }
}

int main(int argc, char *argv[])
{
    if(argc<3){
        printf("Usage: cp source_file target_file\n");
        printf(" cp -r source_directory target_directory\n");
        return -1;
    }
    // 检查是否递归复制目录
    if(strcmp(argv[1], "-r") == 0){
        if(copy_dir(argv[2], argv[3]) == -1){
            printf("Error: cannot copy directory %s\n", argv[2]);
            return -1;
        }
    }else{
        // 复制单个文件
        if(copy_file(argv[1], argv[2]) == -1){
            printf("Error: cannot copy file %s\n", argv[1]);
            return -1;
        }
    }

    
    return 0;
}
