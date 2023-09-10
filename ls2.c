#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <string.h>

#include <pwd.h>
#include <grp.h>

void do_ls(char[]);
void dostat(char*);
void show_file_info(char*, struct stat*);
void mode_to_letters(int, char[]);
char* uid_to_name(uid_t);
char* gid_to_name(gid_t);

int main(int argc, char *argv[])
{
    if(argc == 1)
        do_ls(".");
    else{
        while(--argc){
            printf("%s: \n", *++argv);
            do_ls(*argv);
        }
    }

    return 0;
}

//执行ls
void do_ls(char dirname[]){
    DIR* dir_ptr;
    struct dirent* direntp;

    if((dir_ptr = opendir(dirname)) == NULL)
        fprintf(stderr, "ls2: cannot open %s\n", dirname);
    else{
        while((direntp = readdir(dir_ptr)) != NULL)
            dostat(direntp->d_name);
        
        closedir(dir_ptr);
    }
}

//获取文件信息
void dostat(char* filename){
    struct stat info;
    if(stat(filename, &info) == -1)
        perror(filename);
    else
        show_file_info(filename, &info);
}

//展示最终生成的结果
void show_file_info(char* filename, struct stat* info_p){

    char modestr[11];
    mode_to_letters(info_p->st_mode, modestr);

    printf("%s", modestr);  //文件模式
    printf(" %d ", (int)info_p->st_nlink); //链接
    printf("%-8s",uid_to_name(info_p->st_uid)); //用户
    printf("%-8s", gid_to_name(info_p->st_gid));    //组
    printf("%-8ld",(long)info_p->st_size);       //大小
    printf("%-.12s ", 4+ctime(&info_p->st_mtime));    //最后一次修改时间
    printf("%s\n",filename);        //文件名
}

//文件模式 数字转字符串
void mode_to_letters(int mode, char str[]){
    strcpy(str, "----------");
    
    if(S_ISDIR(mode)) str[0]='d';   //directory
    if(S_ISCHR(mode)) str[0]='c';   //char devices
    if(S_ISBLK(mode)) str[0]='b';   //block devices
    if(S_ISLNK(mode)) str[0]='l';   //symbolic link

    if(mode & S_IRUSR) str[1] = 'r';    //usr
    if(mode & S_IWUSR) str[2] = 'w';
    if(mode & S_IXUSR) str[3] = 'x';

    if(mode & S_IRGRP) str[4] = 'r';    //grp
    if(mode & S_IWGRP) str[5] = 'w';
    if(mode & S_IXGRP) str[6] = 'x';

    if(mode & S_IROTH) str[7] = 'r';    //oth
    if(mode & S_IWOTH) str[8] = 'w';
    if(mode & S_IXOTH) str[9] = 'x';
}

// uid 转换成用户名
char* uid_to_name(uid_t uid){
    struct passwd* getpwuid(), *pw_ptr;
    static char numstr[10];

    if((pw_ptr = getpwuid(uid)) == NULL){
        sprintf(numstr, "%d", uid);
        return numstr;
    }else
        return pw_ptr->pw_name;
}

//gid 转换为组名
char* gid_to_name(gid_t gid){
    struct group* getgrgid(), *grp_ptr;
    static char numstr[10];

    if((grp_ptr = getgrgid(gid)) == NULL){
        sprintf(numstr, "%d", gid);
        return numstr;
    }else
        return grp_ptr->gr_name;
}