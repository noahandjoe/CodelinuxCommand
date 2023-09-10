#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>

#define PATH_MAX 4096

// 递归删除目录
int rm_dir(char *dir)
{
    DIR *d;
    struct dirent *dir_entry;
    struct stat st;
    char path[PATH_MAX];

    // 打开目录
    if ((d = opendir(dir)) == NULL)
    {
        printf("Error: cannot open directory %s\n", dir);
        return -1;
    }

    // 逐个遍历目录下的文件和目录，并递归删除
    while ((dir_entry = readdir(d)) != NULL)
    {
        if (strcmp(dir_entry->d_name, ".") == 0 || strcmp(dir_entry->d_name, "..") == 0)
        {
            // 跳过 . 和 .. 目录
            continue;
        }

        // 构造文件路径
        snprintf(path, PATH_MAX, "%s/%s", dir, dir_entry->d_name);

        // 获取文件状态信息
        if (lstat(path, &st) == -1)
        {
            printf("Error: cannot get status of %s\n", path);
            continue;
        }

        if (S_ISDIR(st.st_mode))
        {
            // 如果是目录，则递归删除
            if (rm_dir(path) == -1)
            {
                printf("Error: cannot remove directory %s\n", path);
                continue;
            }
        }
        else
        {
            // 如果是文件，则删除
            if (unlink(path) == -1)
            {
                printf("Error: cannot remove file %s\n", path);
                continue;
            }
        }
    }

    // 关闭目录并删除目录
    closedir(d);
    if (rmdir(dir) == -1)
    {
        printf("Error: cannot remove directory %s\n", dir);
        return -1;
    }

    return 0;
}

// 删除文件或目录
int rm(char *path, int recursive)
{
    struct stat st;

    // 获取文件或目录状态信息
    if (lstat(path, &st) == -1)
    {
        printf("Error: cannot get status of %s\n", path);
        return -1;
    }

    if (S_ISDIR(st.st_mode))
    {
        // 如果是目录，检查是否需要递归删除
        if (!recursive)
        {
            printf("Error: %s is a directory. Use -r option to remove it.\n", path);
            return -1;
        }

        return rm_dir(path);
    }
    else
    {
        // 如果是文件，直接删除
        if (unlink(path) == -1)
        {
            printf("Error: cannot remove file %s\n", path);
            return -1;
        }

        return 0;
    }
}

int main(int argc, char **argv)
{
    int recursive = 0;
    char *path;

    // 检查参数个数
    if (argc < 2)
    {
        printf("Usage: rm [-r] file/dir\n");
        return -1;
    }
    // 检查是否有递归参数
    if (argc == 3 && strcmp(argv[1], "-r") == 0)
    {
        recursive = 1;
        path = argv[2];
    }
    else if (argc == 2)
    {
        path = argv[1];
    }
    else
    {
        printf("Error: invalid arguments.\n");
        printf("Usage: rm [-r] file/dir\n");
        return -1;
    }

    // 删除文件或目录
    return rm(path, recursive);
}
