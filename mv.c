#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>

#define PATH_MAX 4096

// 移动文件或目录
int mv(char *old_path, char *new_path);

int main(int argc, char *argv[])
{
    char *old_path, *new_path;

    // 检查参数个数
    if (argc < 3)
    {
        printf("Usage: mv <old_path> <new_path>\n");
        return -1;
    }

    old_path = argv[1];
    new_path = argv[2];

    // 移动文件或目录
    if (mv(old_path, new_path) == -1)
    {
        printf("Error: cannot move %s to %s\n", old_path, new_path);
        return -1;
    }

    printf("Successfully moved %s to %s\n", old_path, new_path);

    return 0;
}

int mv(char *old_path, char *new_path)
{
    struct stat st;
    int result;

    // 获取原始文件或目录状态信息
    if (lstat(old_path, &st) == -1)
    {
        printf("Error: cannot get status of %s\n", old_path);
        return -1;
    }

    // 如果原始文件或目录和目标路径相同，直接返回成功
    if (strcmp(old_path, new_path) == 0)
    {
        return 0;
    }

    // 如果原始文件或目录是一个目录，移动目录
    if (S_ISDIR(st.st_mode))
    {
        // 创建新的目录
        result = mkdir(new_path, st.st_mode);
        if (result == -1)
        {
            printf("Error: cannot create directory %s\n", new_path);
            return -1;
        }

        // 逐个遍历目录下的文件和目录，并递归移动
        DIR *d;
        struct dirent *dir_entry;
        char old_subpath[PATH_MAX], new_subpath[PATH_MAX];

        // 先打开目录
        if ((d = opendir(old_path)) == NULL)
        {
            printf("Error: cannot open directory %s\n", old_path);
            return -1;
        }

        // 循环读取文件夹中的每一项
        while ((dir_entry = readdir(d)) != NULL)
        {
            // 跳过 . 和 .. 目录
            if (strcmp(dir_entry->d_name, ".") == 0 || strcmp(dir_entry->d_name, "..") == 0)
            {
                continue;
            }

            // 构造原始路径和目标路径
            snprintf(old_subpath, PATH_MAX, "%s/%s", old_path, dir_entry->d_name);
            snprintf(new_subpath, PATH_MAX, "%s/%s", new_path, dir_entry->d_name);

            // 递归移动文件或目录
            if (mv(old_subpath, new_subpath) == -1)
            {
                printf("Error: cannot move %s to %s\n", old_subpath, new_subpath);
                continue;
            }
        }

        // 关闭原始目录并删除
        closedir(d);
        if (rmdir(old_path) == -1)
        {
            printf("Error cannot remove directory %s\n", old_path);
            return -1;
        }
    }
    else
    {
        // 如果原始文件或目录是一个文件，移动文件
        result = rename(old_path, new_path);
        if (result == -1)
        {
            printf("Error: cannot move file %s to %s\n", old_path, new_path);
            return -1;
        }
    }
    return 0;
}