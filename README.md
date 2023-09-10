# CodelinuxCommand
记录学习过程中Linux常用命令的C语言实现

## 1.ls命令

所需函数:
`DIR * opendir(const char *name);`

> opendir() 函数打开与目录名对应的目录流，并返回一个指向该目录流的指针。该流定位于目录中的第一个条目。

`struct dirent *readdir(DIR *dirp);`

> readdir() 函数返回一个指向 dirent 结构体的指针，该结构体表示目录流 dirp 中的下一个目录条目。如果到达目录流的末尾或发生错误，则返回 NULL。

`int closedir(DIR *dirp);`

> closedir() 函数关闭与 dirp 相关联的目录流。成功调用 closedir() 也会关闭与 dirp 相关联的底层文件描述符。该目录流描述符在此调用之后不再可用。

数据结构:

```c
struct dirent {
    ino_t          d_ino;       /* inode number */
    off_t          d_off;       /* not an offset; see NOTES */
    unsigned short d_reclen;    /* length of this record */
    unsigned char  d_type;      /* type of file; not supported
                                    by all filesystem types */
    char           d_name[256]; /* filename */
};
```


示例代码:ls.c

```c
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
```

---

## 2.alias命令

alias命令允许您在shell会话中定义临时别名,如:`alias lh=ls -lah`

所需函数:
`char *strtok(char *str, const char *delim);`

> The  strtok()  function  breaks a string into a sequence of zero or more nonempty tokens.  On the first call to strtok() the string to be parsed should be specified in str.  In each subsequent call that should parse the same string, str must be NULL.

示例代码: alias.cpp

```cpp
#include <iostream>
#include <string>
#include <map>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>

using namespace std;

map<string, string> aliases;

void add_alias(const char* name, const char* command){
    string key(name), value(command);
    aliases[key] = value;
}

void print_aliases(){
    for(auto it = aliases.begin(); it!=aliases.end(); ++it){
        cout << it->first << "=\"" << it->second << "\"" << endl;
        // key="value"
    }
}

void execute_command(const char* command){
    string cmd(command);
    auto it = aliases.find(cmd);
    if(it!=aliases.end()){
        //找到了该命令的别名,使用 system 函数执行该别名对应的命令
        //c_str()函数将std::string类型的字符串转换为C风格的字符串
        system(it->second.c_str());
    }else{
        //没有找到该命令的别名,执行该命令字符串
        system(command);
    }
}

int main(){
    char buf[1024];
    while(true){
        printf("myshell> ");
        fgets(buf, sizeof(buf), stdin);
        buf[strlen(buf) - 1] = '\0'; //去掉换行符
        if(strncmp(buf, "alias ", 6) == 0){
            const char* name = strtok(&buf[6],"="); //分割后的别名
            const char* command = strtok(NULL, ""); //剩下的是对应命令
            //先存到别名表里
            add_alias(name, command);
        }else if(strcmp(buf, "$alias") == 0){
            //列出所有别名
            print_aliases();
        }else if(strcmp(buf, "exit") == 0){
            //退出
            break;
        }else{
            //执行命令
            execute_command(buf);
        }
    }
    return 0;
}
```

---

## 3.pwd命令

pwd命令代表“打印工作目录”，它输出您所在目录的绝对路径

所需函数:
`char *getcwd(char *buf, size_t size);`

> The  getcwd()  function  copies  an  absolute  pathname of the current working directory to the array pointed to by buf, which is of length size.

示例代码: pwd.cpp

```cpp
#include <cstdio>
#include <unistd.h>

using namespace std;

int main(){
    char buf[256];
    if(getcwd(buf, sizeof(buf))!=NULL){
        printf("%s\n", buf);
    }else{
        perror("getcwd() error");
        return -1;
    }
    return 0;
}
```

---

## 4.cd命令

切换到您试图访问的目录

所需函数:
`int stat(const char *path, struct stat *buf);`

> These  functions  return  information  about a file.  No permissions are required on the file itself, but—in the case of stat() and lstat() — execute (search) permission is required on all of the directories in path that lead to the file.

> stat() stats the file pointed to by path and fills in buf.

数据结构:

```c
struct stat {
    dev_t     st_dev;     /* ID of device containing file */
    ino_t     st_ino;     /* inode number */
    mode_t    st_mode;    /* protection */
    nlink_t   st_nlink;   /* number of hard links */
    uid_t     st_uid;     /* user ID of owner */
    gid_t     st_gid;     /* group ID of owner */
    dev_t     st_rdev;    /* device ID (if special file) */
    off_t     st_size;    /* total size, in bytes */
    blksize_t st_blksize; /* blocksize for filesystem I/O */
    blkcnt_t  st_blocks;  /* number of 512B blocks allocated */
    time_t    st_atime;   /* time of last access */
    time_t    st_mtime;   /* time of last modification */
    time_t    st_ctime;   /* time of last status change */
};
```

代码示例:
版本一: 代码运行时修改目录 cdir.cpp

```cpp
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>

using namespace std;

int main(int argc, char *argv[])
{
    if(argc < 2){
        cout<<"Usage: "<<argv[0]<<" directory_path"<<endl;
        return 1;
    }

    // 获取当前工作目录
    char currentDir[1024];
    if(getcwd(currentDir, sizeof(currentDir)) == NULL){
        cout<<"Failed to get current working directory."<<endl;
        return 1;
    }

    // 判断目录是否存在
    struct stat dirStat;
    if(stat(argv[1], &dirStat) != 0){
        cout<<"The directory does not exist."<<endl;
        return 1;
    }

    // 判断是否是目录
    if(!S_ISDIR(dirStat.st_mode)){
        cout<<"The path is not a directory."<<endl;
        return 1;
    }

    // 切换目录
    if(chdir(argv[1])!=0){
        cout<<"Failed to change directory."<<endl;
        return 1;
    }
    system("ls -la");

    // 获取新的工作目录
    char newDir[1024];
    if(getcwd(newDir, sizeof(newDir)) == NULL){
        cout<<"Failed to get current working directory."<<endl;
        return 1;
    }

    cout<<"Changed directory from "<<currentDir<<" to "<<newDir<<endl;

    return 0;
}
```

版本二: 在终端中永久性地更改当前目录 cdir.cpp
上述代码执行 `source cdir /dir`

---

## 5.cp命令

在Linux终端上复制文件和文件夹,还可以`-r`使用递归标志复制整个目录：

所需函数:

`int access(const char *pathname, int mode);`

> access() checks whether the calling process can access the file pathname.  If pathname is a symbolic link, it is dereferenced.

> F_OK tests for the existence of the file.

`int lstat(const char *path, struct stat *buf);`

> stat和lstat的区别：当文件是一个符号链接时，lstat返回的是该符号链接本身的信息；而stat返回的是该链接指向的文件的信息。

`int mkdir(const char *pathname, mode_t mode);`

> mkdir() attempts to create a directory named pathname. The argument mode specifies the permissions to use. 

示例代码: cp.c

```c
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

```

---

## 6.rm命令

使用rm命令删除文件和目录, 可以使用递归（-r）参数,删除包含内容的目录

所需函数:
`int unlink(const char *pathname);`

> unlink()  deletes  a  name from the filesystem.  If that name was the last link to a file and no processes have the file open the file is deleted and the space it was using is made available for reuse.

`int rmdir(const char *pathname);`

> rmdir() deletes a directory, which must be empty.


示例代码: rm.c , 递归删除目录使用了深度优先遍历算法

```c
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

```

---

## 7.mv命令

移动（或重命名）文件和目录, 支持子目录的递归移动

所需函数:
`int rename(const char *oldpath, const char *newpath);`

> rename() renames a file, moving it between directories if required.  Any other hard links to the file (as created using link(2)) are unaffected.  Open file descriptors for oldpath are also unaffected.


示例代码: mv.c

```c
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
```

---

## 8.mkdir命令

创建文件夹, 支持-p参数创建子目录, 如: `mkdir -p dir/abc/`

示例代码:mkdir.c

```c
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
```

---

## 9.touch命令

创建新的空文件, 支持-m参数修改日期更改为当前时间, 如:`touch -m old_file`

所需函数:
`int getopt(int argc, char * const argv[],const char *optstring);`

> The getopt() function parses the command-line arguments. If getopt() is called repeatedly, it returns successively each of the option characters from each of the option elements.

`int gettimeofday(struct timeval *tv, struct timezone *tz);`

> The  functions  gettimeofday() and settimeofday() can get and set the time as well as a timezone.

`int utimes(const char *filename, const struct timeval times[2]);`

> changes the access and modification times of the inode specified by filename to the actime and modtime fields of times respectively. times[0] specifies the new access time, and times[1] specifies the new modification time. 

---

## 10.chmod命令

允许快速更改文件的模式（权限), 支持+r、-r、+w、-w、+x、-x六种操作符，以及ugo（即用户、用户组、其他用户）三种身份和+rwx三种权限。
例如: 
`chmod +rwx abc` -rwxrwxrwx
`chmod +rw abc` -rw-rw-rw-
`chmod a abc` -rwxrwxrwx
`chmod a-r abc` --wx-wx-wx
`chmod o+x abc` ---x--xrwx
`chmod ug+rw abc` -rwxrwxrw-

示例代码: chmod.c 

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

void usage(char* name){
    printf("Usage: %s mode file\n", name);
    printf("  mode - the new mode of the file, using format 'ugoa+-rwx'\n");
    printf("  file - the file to modify\n");
}

int main(int argc, char *argv[])
{
    if(argc != 3){
        usage(argv[0]);
        return -1;
    }

    char* mode_str = argv[1];
    char* filename = argv[2];

    //解析权限掩码
    __mode_t mode = 0;
    for(int i = 0; i<strlen(mode_str); i++){
        if(mode_str[i] == 'u'){
            mode |= S_IRUSR | S_IWUSR | S_IXUSR;
        }else if(mode_str[i] == 'g'){
            mode |= S_IRGRP | S_IWGRP | S_IXGRP;
        }else if(mode_str[i] == 'o'){
            mode |= S_IROTH | S_IWOTH | S_IXOTH;
        }else if(mode_str[i] == 'a'){
            mode |= S_IRUSR | S_IWUSR | S_IXUSR |
                    S_IRGRP | S_IWGRP | S_IXGRP |
                    S_IROTH | S_IWOTH | S_IXOTH;
        }else if(mode_str[i] == '+'){
            i++;
            while(i<strlen(mode_str)){
                if(mode_str[i] == 'r'){
                    mode |= S_IRUSR | S_IRGRP | S_IROTH;
                }else if(mode_str[i] == 'w'){
                    mode |= S_IWUSR | S_IWGRP | S_IWOTH;
                }else if(mode_str[i] == 'x'){
                    mode |= S_IXUSR | S_IXGRP | S_IXOTH;
                }else{
                    printf("Error: invalid permission '%c'\n", mode_str[i]);
                    return -1;
                }
                i++;
            }
        }else if(mode_str[i] == '-'){
            i++;
            while(i<strlen(mode_str)){
                if(mode_str[i] == 'r'){
                    mode &= ~(S_IRUSR | S_IRGRP | S_IROTH);
                }else if(mode_str[i] == 'w'){
                    mode &= ~(S_IWUSR | S_IWGRP | S_IWOTH);
                }else if(mode_str[i] == 'x'){
                    mode &= ~(S_IXUSR | S_IXGRP | S_IXOTH);
                }else{
                    printf("Error: invalid permission '%c'\n", mode_str[i]);
                    return -1;
                }
                i++;
            }
        }
    }

    if(chmod(filename, mode) == -1){
        perror("chmod");
        return -1;
    }

    return 0;
}
```

---

## 11.echo命令

在终端中显示定义的文本,支持打印环境变量 如: echo "$USER", 支持-n 参数单行打印

所需函数:
`char *getenv(const char *name);`

> The getenv() function searches the environment list to find the environment variable name, and returns a pointer to the corresponding value string.

示例代码: echo.c

```c
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

        //在参数后打印一个空格，除非它是最后一个
        if(i != argc - 1){
            printf(" ");
        }
    }

    if(printNewLine){
        printf("\n");
    }

    
    return 0;
}
```

---

## 12.cat命令

直接从终端创建、查看和连接文件 支持多文件拼接打印 `cat a.txt b.txt`

示例代码: cat.c

```c
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    FILE* fp;
    int c;

    if (argc<2){
        fprintf(stderr, "Usage: %s file1 [file2 ...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    for(int i = 1; i<argc; i++){
        if((fp = fopen(argv[i], "r")) == NULL){
            fprintf(stderr, "Error: cannot open file %s\n", argv[i]);
            exit(EXIT_FAILURE);
        }
        while((c = getc(fp)) != EOF){
            putchar(c);
        }
        fclose(fp);
    }
    exit(EXIT_SUCCESS);

    return 0;
}

```

---

## 13.ps命令

查看当前shell会话正在运行的进程, 支持-aux 显示所有包含其他使用者的进程
